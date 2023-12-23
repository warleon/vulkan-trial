#include "Application.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"

static int isDeviceOkAndSetQueueIndex(Application*, VkPhysicalDevice);

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

	uint32_t queueIndex = 0;
	for (size_t i = 0; i < count; i++) {
		queueIndex = isDeviceOkAndSetQueueIndex(app, app->physicalDevices[i]);
		if (queueIndex) {
			app->vulkanPhysicalDevice = app->physicalDevices[0];
		}
	}


}

void initDevice(Application* app) {
	VkResult err;
	assert(app->graphicsQueueFamilyIndex);

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

	err = vkCreateDevice(app->vulkanPhysicalDevice, &app->vulkanDeviceCreateInfo, NULL, &app->vulkanDevice);
	assert(!err);

	vkGetDeviceQueue(app->vulkanDevice, app->graphicsQueueFamilyIndex - 1, 0, &app->vulkanQueue[0]);
}


void destroy(Application* app) {
	vkDestroyDevice(app->vulkanDevice, NULL);
	vkDestroySurfaceKHR(app->vulkanInstance, app->vulkanSurface, NULL);
	vkDestroyInstance(app->vulkanInstance, NULL);
	//free(app->vulkanInstance);
	free(app->properties);
	free(app->layers);
}

int isDeviceOkAndSetQueueIndex(Application* app, VkPhysicalDevice device) {
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceProperties(device, &properties);
	vkGetPhysicalDeviceFeatures(device, &features);
	//check for specific properties or features
	//end of check

	uint32_t count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &count, NULL);
	assert(count);
	VkQueueFamilyProperties* queueFamilyProperties = malloc(sizeof(VkQueueFamilyProperties) * count);
	assert(queueFamilyProperties);
	for (size_t i = 0; i < count; i++) {
		if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			app->graphicsQueueFamilyIndex = i + 1;
		}
	}
	free(queueFamilyProperties);
	return app->graphicsQueueFamilyIndex;
}
