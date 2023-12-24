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
	VkSurfaceCapabilitiesKHR vulkanSurfaceCapabilities;
	VkSurfaceFormatKHR vulkanSurfaceChosenFormat;
	VkPresentModeKHR VulkanSurfaceChosenPresentMode;
	VkSurfaceFormatKHR* vulkanSurfaceFormats;
	VkPresentModeKHR* vulkanSurfacePresentModes;
	uint32_t vulkanSurfaceFormatsSize;
	uint32_t vulkanSurfacePresentModesSize;
	VkExtent2D vulkanSurfaceExtent;

	VkSwapchainKHR vulkanSwapChain;
	VkSwapchainCreateInfoKHR vulkanSwapChainCreateInfo;

	VkImage* vulkanSwapChainImages;
	VkImageView* vulkanSwapChainImageViews;
	uint32_t vulkanSwapChainImagesSize;
	uint32_t vulkanSwapChainImageViewsSize;

	VkRenderPass vulkanRenderPass;
	VkPipeline vulkanGraphicsPipeline;
	VkPipelineLayout vulkanPipelineLayout;
	VkShaderModule vulkanShaderModules[2];


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
	VkExtensionProperties* deviceExtensionProperties;
	char** deviceExtensionPropertiesNames;


}	Application;


void init(Application*, const char*, const char*, HINSTANCE hinstance, HWND hwnd);
void initInstance(Application*);
void initSurface(Application*);
void initPhisicalDevice(Application*);
void initDevice(Application*);
void initSwapChain(Application*);
void initImageViews(Application*);
void initRenderPass(Application*);
void initGraphicsPipeline(Application*);
void destroy(Application*);

void getWindowSize(Application*, int*, int*);
void loadShaders(Application*);



int isDeviceOkAndSetQueueIndex(Application*, VkPhysicalDevice);
void querySwapChainSupport(Application*, VkPhysicalDevice);
void unquerySwapChainSupport(Application*);
