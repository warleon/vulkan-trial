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

	uint32_t enabledVulkanGlobalExtensionsSize;
	uint32_t enabledVulkanValidationLayersSize;
	char* enabledVulkanGlobalExtensions[64];
	char* enabledVulkanValidationLayers[64];

	VkExtensionProperties* properties;
	VkLayerProperties* layers;

}	Application;


void init(Application*, const char*, const char*, HINSTANCE hinstance, HWND hwnd);
void initInstance(Application*);
void initSurface(Application*);
void destroy(Application*);
