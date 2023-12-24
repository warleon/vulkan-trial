#include "Application.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"


void init(Application* app, const char* name, const char* engineName, HINSTANCE hinstance, HWND hwnd) {
	memset(app, 0, sizeof(Application));
	app->name = name;
	app->engineName = engineName;
	app->hinstance = hinstance;
	app->hwnd = hwnd;

	initInstance(app);
	initSurface(app);
	initPhisicalDevice(app);
	initDevice(app);
	initSwapChain(app);
}
void initInstance(Application* app) {
	VkResult err;

	app->vulkanApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app->vulkanApplicationInfo.pNext = NULL;
	app->vulkanApplicationInfo.pApplicationName = app->name;
	app->vulkanApplicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app->vulkanApplicationInfo.pEngineName = app->engineName;
	app->vulkanApplicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	app->vulkanApplicationInfo.apiVersion = VK_API_VERSION_1_0;


	uint32_t count = 0;
	err = vkEnumerateInstanceExtensionProperties(NULL, &count, NULL);
	assert(!err && count);
	app->properties = malloc(sizeof(VkExtensionProperties) * count);
	assert(app->properties);
	err = vkEnumerateInstanceExtensionProperties(NULL, &count, app->properties);
	assert(!err && count < 64);
	for (size_t i = 0; i < count; i++) {
		app->enabledVulkanGlobalExtensions[app->enabledVulkanGlobalExtensionsSize++] = app->properties[i].extensionName;
	}
	count = 0;
	err = vkEnumerateInstanceLayerProperties(&count, NULL);
	assert(!err && count);
	app->layers = malloc(sizeof(VkLayerProperties) * count);
	assert(app->layers);
	err = vkEnumerateInstanceLayerProperties(&count, app->layers);
	assert(!err && count < 64);
	for (size_t i = 0; i < count; i++) {
		app->enabledVulkanValidationLayers[app->enabledVulkanValidationLayersSize++] = app->layers[i].layerName;
	}

	app->vulkanInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	app->vulkanInstanceCreateInfo.pNext = NULL;
	app->vulkanInstanceCreateInfo.pApplicationInfo = &app->vulkanApplicationInfo;
	app->vulkanInstanceCreateInfo.enabledExtensionCount = app->enabledVulkanGlobalExtensionsSize;
	app->vulkanInstanceCreateInfo.enabledLayerCount = app->enabledVulkanValidationLayersSize;
	app->vulkanInstanceCreateInfo.ppEnabledExtensionNames = app->enabledVulkanGlobalExtensions;
	app->vulkanInstanceCreateInfo.ppEnabledLayerNames = app->enabledVulkanValidationLayers;


	err = vkCreateInstance(&(app->vulkanInstanceCreateInfo), NULL, &(app->vulkanInstance));
	assert(!(err == VK_ERROR_OUT_OF_HOST_MEMORY));
	assert(!(err == VK_ERROR_OUT_OF_DEVICE_MEMORY));
	assert(!(err == VK_ERROR_INITIALIZATION_FAILED));
	assert(!(err == VK_ERROR_LAYER_NOT_PRESENT));
	assert(!(err == VK_ERROR_EXTENSION_NOT_PRESENT));
	assert(!(err == VK_ERROR_INCOMPATIBLE_DRIVER));
};

void initSurface(Application* app) {
	VkResult err;
	app->vulkanSurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	app->vulkanSurfaceCreateInfo.hinstance = app->hinstance;
	app->vulkanSurfaceCreateInfo.hwnd = app->hwnd;

	err = vkCreateWin32SurfaceKHR(app->vulkanInstance, &app->vulkanSurfaceCreateInfo, NULL, &app->vulkanSurface);
	assert(!(err == VK_ERROR_OUT_OF_HOST_MEMORY));
	assert(!(err == VK_ERROR_OUT_OF_DEVICE_MEMORY));
}
void initPhisicalDevice(Application* app) {
	VkResult err;
	uint32_t count;
	err = vkEnumeratePhysicalDevices(app->vulkanInstance, &count, NULL);
	assert(!err && count);
	app->physicalDevices = malloc(sizeof(VkPhysicalDevice) * count);
	assert(app->physicalDevices);
	err = vkEnumeratePhysicalDevices(app->vulkanInstance, &count, app->physicalDevices);
	assert(!err);

	uint32_t ok = 0;
	for (size_t i = 0; i < count; i++) {
		ok = isDeviceOkAndSetQueueIndex(app, app->physicalDevices[i]);
		if (ok) {
			app->vulkanPhysicalDevice = app->physicalDevices[i];
		}
	}
	assert(ok);
}

void initDevice(Application* app) {
	VkResult err;
	uint32_t count;
	assert(app->graphicsQueueFamilyIndex);

	err = vkEnumerateDeviceExtensionProperties(app->vulkanPhysicalDevice, NULL, &count, NULL);
	assert(!err && count);
	app->deviceExtensionProperties = malloc(sizeof(VkExtensionProperties) * count);
	assert(app->deviceExtensionProperties);
	err = vkEnumerateDeviceExtensionProperties(app->vulkanPhysicalDevice, NULL, &count, app->deviceExtensionProperties);
	assert(!err);
	app->deviceExtensionPropertiesNames = malloc(sizeof(char*) * count);
	assert(app->deviceExtensionPropertiesNames);
	for (size_t i = 0; i < count; i++) {
		app->deviceExtensionPropertiesNames[i] = &app->deviceExtensionProperties[i].extensionName;
	}

	app->graphicsQueueFamilyPriority = 1.0f;

	app->vulkanDeviceQueueCreateInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	app->vulkanDeviceQueueCreateInfo[0].queueFamilyIndex = app->graphicsQueueFamilyIndex - 1;
	app->vulkanDeviceQueueCreateInfo[0].queueCount = 1;
	app->vulkanDeviceQueueCreateInfo[0].pQueuePriorities = &app->graphicsQueueFamilyPriority;

	vkGetPhysicalDeviceFeatures(app->vulkanPhysicalDevice, &app->vulkanPhysicalDeviceFeatures);

	app->vulkanDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	app->vulkanDeviceCreateInfo.pQueueCreateInfos = app->vulkanDeviceQueueCreateInfo;
	app->vulkanDeviceCreateInfo.queueCreateInfoCount = ARRAY_SIZE(app->vulkanDeviceQueueCreateInfo);
	app->vulkanDeviceCreateInfo.pEnabledFeatures = &app->vulkanPhysicalDeviceFeatures;
	app->vulkanDeviceCreateInfo.enabledLayerCount = app->enabledVulkanValidationLayersSize;
	app->vulkanDeviceCreateInfo.ppEnabledLayerNames = app->enabledVulkanValidationLayers;
	app->vulkanDeviceCreateInfo.ppEnabledExtensionNames = app->deviceExtensionPropertiesNames;
	app->vulkanDeviceCreateInfo.enabledExtensionCount = count;

	err = vkCreateDevice(app->vulkanPhysicalDevice, &app->vulkanDeviceCreateInfo, NULL, &app->vulkanDevice);
	assert(!err);

	vkGetDeviceQueue(app->vulkanDevice, app->graphicsQueueFamilyIndex - 1, 0, &app->vulkanQueue[0]);
}

void initSwapChain(Application* app) {
	VkResult err = VK_SUCCESS;
	querySwapChainSupport(app, app->vulkanPhysicalDevice);

	app->vulkanSurfaceChosenFormat = app->vulkanSurfaceFormats[0];
	app->VulkanSurfaceChosenPresentMode = VK_PRESENT_MODE_FIFO_KHR;

	for (size_t i = 0; i < app->vulkanSurfaceFormatsSize; i++) {
		if (
			app->vulkanSurfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
			&app->vulkanSurfaceFormats[i].colorSpace
			) {
			app->vulkanSurfaceChosenFormat = app->vulkanSurfaceFormats[i];
		}
	}
	for (size_t i = 0; i < app->vulkanSurfacePresentModesSize; i++) {
		if (app->vulkanSurfacePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
			app->VulkanSurfaceChosenPresentMode = app->vulkanSurfacePresentModes[i];
		}
	}

	if (app->vulkanSurfaceCapabilities.currentExtent.width != (uint32_t)-1) {
		app->vulkanSurfaceExtent = app->vulkanSurfaceCapabilities.currentExtent;
	}
	else {
		unsigned int width = 0, height = 0;
		getWindowSize(app, &width, &height);

		clamp(&width,
			app->vulkanSurfaceCapabilities.minImageExtent.width,
			app->vulkanSurfaceCapabilities.maxImageExtent.width
		);
		clamp(&height,
			app->vulkanSurfaceCapabilities.minImageExtent.height,
			app->vulkanSurfaceCapabilities.maxImageExtent.height
		);
		app->vulkanSurfaceExtent.width = width;
		app->vulkanSurfaceExtent.height = height;
	}

	uint32_t imageCount = app->vulkanSurfaceCapabilities.minImageCount + 1;
	if (app->vulkanSurfaceCapabilities.maxImageCount > 0 &&
		imageCount > app->vulkanSurfaceCapabilities.maxImageCount
		)
		imageCount = app->vulkanSurfaceCapabilities.maxImageCount;


	app->vulkanSwapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	app->vulkanSwapChainCreateInfo.surface = app->vulkanSurface;
	app->vulkanSwapChainCreateInfo.minImageCount = imageCount;
	app->vulkanSwapChainCreateInfo.imageFormat = app->vulkanSurfaceChosenFormat.format;
	app->vulkanSwapChainCreateInfo.imageColorSpace = app->vulkanSurfaceChosenFormat.colorSpace;
	app->vulkanSwapChainCreateInfo.imageExtent = app->vulkanSurfaceExtent;
	app->vulkanSwapChainCreateInfo.imageArrayLayers = 1;
	app->vulkanSwapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	app->vulkanSwapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	app->vulkanSwapChainCreateInfo.preTransform = app->vulkanSurfaceCapabilities.currentTransform;
	app->vulkanSwapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	app->vulkanSwapChainCreateInfo.presentMode = app->VulkanSurfaceChosenPresentMode;
	app->vulkanSwapChainCreateInfo.clipped = VK_TRUE;
	app->vulkanSwapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	err = vkCreateSwapchainKHR(app->vulkanDevice, &app->vulkanSwapChainCreateInfo, NULL, &app->vulkanSwapChain);
	assert(!err);

}

void destroy(Application* app) {
	vkDestroySwapchainKHR(app->vulkanDevice, app->vulkanSwapChain, NULL);
	vkDestroyDevice(app->vulkanDevice, NULL);
	vkDestroySurfaceKHR(app->vulkanInstance, app->vulkanSurface, NULL);
	vkDestroyInstance(app->vulkanInstance, NULL);

	free(app->properties);
	free(app->layers);
	free(app->physicalDevices);
	free(app->deviceExtensionProperties);
}


void getWindowSize(Application* app, int* width, int* height) {
	BOOL succeed = 0;
	RECT rect;
	succeed = GetWindowRect(app->hwnd, &rect);
	assert(succeed);

	*width = rect.right - rect.left;
	*height = rect.bottom - rect.top;
}


int isDeviceOkAndSetQueueIndex(Application* app, VkPhysicalDevice device) {
	VkResult err;
	uint32_t count = 0;
	VkBool32 hasSurfaceSupport = 0;
	VkBool32 hasExtension = 0;
	VkBool32 hasSwapChainSupport = 0;


	//	VkPhysicalDeviceProperties properties;
	//	VkPhysicalDeviceFeatures features;
	//	vkGetPhysicalDeviceProperties(device, &properties);
	//	vkGetPhysicalDeviceFeatures(device, &features);

	err = vkEnumerateDeviceExtensionProperties(device, NULL, &count, NULL);
	assert(!err && count);
	VkExtensionProperties* extensionProperties = malloc(sizeof(VkExtensionProperties) * count);
	assert(extensionProperties);
	err = vkEnumerateDeviceExtensionProperties(device, NULL, &count, extensionProperties);
	assert(!err);
	for (size_t i = 0; i < count; i++) {
		hasExtension = hasExtension || !strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME, extensionProperties[i].extensionName);
	}
	free(extensionProperties);

	count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &count, NULL);
	assert(!err);
	assert(count);
	VkQueueFamilyProperties* queueFamilyProperties = malloc(sizeof(VkQueueFamilyProperties) * count);
	assert(queueFamilyProperties);
	for (size_t i = 0; i < count; i++) {
		if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			err = vkGetPhysicalDeviceSurfaceSupportKHR(device, i, app->vulkanSurface, &hasSurfaceSupport);
			assert(!err);
			if (hasSurfaceSupport) {
				querySwapChainSupport(app, device);
				hasSwapChainSupport = app->vulkanSurfaceFormats && app->vulkanSurfacePresentModes;
				unquerySwapChainSupport(app);
				if (hasSwapChainSupport)
					app->graphicsQueueFamilyIndex = i + 1;
			}
		}
	}
	free(queueFamilyProperties);


	return (app->graphicsQueueFamilyIndex > 0) && hasSurfaceSupport && hasExtension;
}

void querySwapChainSupport(Application* app, VkPhysicalDevice device) {
	VkResult err;
	err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, app->vulkanSurface, &app->vulkanSurfaceCapabilities);
	assert(!err);
	err = vkGetPhysicalDeviceSurfaceFormatsKHR(device, app->vulkanSurface, &app->vulkanSurfaceFormatsSize, NULL);
	assert(!err && app->vulkanSurfaceFormatsSize);
	app->vulkanSurfaceFormats = malloc(sizeof(VkSurfaceFormatKHR) * app->vulkanSurfaceFormatsSize);
	assert(app->vulkanSurfaceFormats);
	err = vkGetPhysicalDeviceSurfaceFormatsKHR(device, app->vulkanSurface, &app->vulkanSurfaceFormatsSize, app->vulkanSurfaceFormats);
	assert(!err);
	err = vkGetPhysicalDeviceSurfacePresentModesKHR(device, app->vulkanSurface, &app->vulkanSurfacePresentModesSize, NULL);
	assert(!err && app->vulkanSurfacePresentModesSize);
	app->vulkanSurfacePresentModes = malloc(sizeof(VkPresentModeKHR) * app->vulkanSurfacePresentModesSize);
	assert(app->vulkanSurfacePresentModes);
	err = vkGetPhysicalDeviceSurfacePresentModesKHR(device, app->vulkanSurface, &app->vulkanSurfacePresentModesSize, app->vulkanSurfacePresentModes);
	assert(!err);
}
void unquerySwapChainSupport(Application* app) {
	if (app->vulkanSurfaceFormats)
		free(app->vulkanSurfaceFormats);
	app->vulkanSurfaceFormats = NULL;
	if (app->vulkanSurfacePresentModes)
		free(app->vulkanSurfacePresentModes);
	app->vulkanSurfacePresentModes = NULL;
}
