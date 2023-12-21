#pragma once
#include <vulkan/vulkan.h>

typedef struct {
	char const* name;
	char const* engineName;
	VkInstance vulkanInstance;
	VkApplicationInfo vulkanApplicationInfo;
	VkInstanceCreateInfo vulkanInstanceCreateInfo;
	char* enabledVulkanGlobalExtensions[64];
	char* enabledVulkanValidationLayers[64];
	uint32_t enabledVulkanGlobalExtensionsSize;
	uint32_t enabledVulkanValidationLayersSize;
}
Application;


void applicationInit(Application*, char*, char*);
