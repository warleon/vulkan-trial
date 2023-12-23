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
	app->vulkanDeviceCreateInfo.enabledExtensionCount = ARRAYSIZE(app->deviceExtensionPropertiesNames);

	err = vkCreateDevice(app->vulkanPhysicalDevice, &app->vulkanDeviceCreateInfo, NULL, &app->vulkanDevice);
	assert(!err);

	vkGetDeviceQueue(app->vulkanDevice, app->graphicsQueueFamilyIndex - 1, 0, &app->vulkanQueue[0]);
}


void destroy(Application* app) {
	vkDestroyDevice(app->vulkanDevice, NULL);
	vkDestroySurfaceKHR(app->vulkanInstance, app->vulkanSurface, NULL);
	vkDestroyInstance(app->vulkanInstance, NULL);

	free(app->properties);
	free(app->layers);
	free(app->physicalDevices);
	free(app->deviceExtensionProperties);
}

int isDeviceOkAndSetQueueIndex(Application* app, VkPhysicalDevice device) {
	VkResult err;
	uint32_t count = 0;
	VkBool32 hasSupport = 0;
	VkBool32 hasExtension = 0;
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
			err = vkGetPhysicalDeviceSurfaceSupportKHR(device, i, app->vulkanSurface, &hasSupport);
			assert(!err);
			if (hasSupport)
				app->graphicsQueueFamilyIndex = i + 1;
		}
	}
	free(queueFamilyProperties);

	return (app->graphicsQueueFamilyIndex > 0) && hasSupport && hasExtension;
}
