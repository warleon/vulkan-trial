#pragma once
#include <vulkan/vulkan.h>
#include <Windows.h>
#include <vulkan/vulkan_win32.h>

typedef struct {
	char* name;
	char* engineName;

	HINSTANCE hinstance;
	HWND hwnd;

	VkInstance vulkanInstance;
	VkApplicationInfo vulkanApplicationInfo;
	VkInstanceCreateInfo vulkanInstanceCreateInfo;

	VkSurfaceKHR vulkanSurface;
	VkWin32SurfaceCreateInfoKHR vulkanSurfaceCreateInfo;

	VkPhysicalDevice vulkanPhysicalDevice;
	VkPhysicalDeviceFeatures vulkanPhysicalDeviceFeatures;

	VkDevice vulkanDevice;
	VkDeviceCreateInfo vulkanDeviceCreateInfo;
	VkQueue vulkanQueue[1];
	VkDeviceQueueCreateInfo vulkanDeviceQueueCreateInfo[1];
	uint32_t graphicsQueueFamilyIndex;// = realindex+1
	float graphicsQueueFamilyPriority;

	uint32_t enabledVulkanGlobalExtensionsSize;
	uint32_t enabledVulkanValidationLayersSize;
	char* enabledVulkanGlobalExtensions[64];
	char* enabledVulkanValidationLayers[64];

	VkExtensionProperties* properties;
	VkLayerProperties* layers;
	VkPhysicalDevice* physicalDevices;


}	Application;


void init(Application*, const char*, const char*, HINSTANCE hinstance, HWND hwnd);
void initInstance(Application*);
void initSurface(Application*);
void initPhisicalDevice(Application*);
void initDevice(Application*);
void destroy(Application*);
