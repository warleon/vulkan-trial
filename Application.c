#define _CRT_SECURE_NO_WARNINGS
#include "Application.h"
#include <assert.h>
#include<stdio.h>
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
	initRenderPass(app);
	initGraphicsPipeline(app);
	initFrameBuffer(app);
	initCommandPool(app);
	initCommandBuffer(app);
	initSyncObjects(app);
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
	assert(!err && count <= 256);
	for (size_t i = 0; i < count; i++) {
		app->enabledVulkanGlobalExtensions[app->enabledVulkanGlobalExtensionsSize++] = app->properties[i].extensionName;
	}
	count = 0;
	err = vkEnumerateInstanceLayerProperties(&count, NULL);
	assert(!err && count);
	app->layers = malloc(sizeof(VkLayerProperties) * count);
	assert(app->layers);
	err = vkEnumerateInstanceLayerProperties(&count, app->layers);
	assert(!err && count <= 256);
	for (size_t i = 0; i < count; i++) {
		if (!strcmp(app->layers[i].layerName, "VK_LAYER_LUNARG_gfxreconstruct"))continue;
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
	for (uint32_t i = 0; i < count; i++) {
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
	for (uint32_t i = 0; i < count; i++) {
		if (!strcmp(app->deviceExtensionProperties[i].extensionName, "VK_EXT_buffer_device_address"))continue;
		if (!strcmp(app->deviceExtensionProperties[i].extensionName, "VK_EXT_image_2d_view_of_3d"))continue;
		if (!strcmp(app->deviceExtensionProperties[i].extensionName, "VK_KHR_dynamic_rendering"))continue;

		app->deviceExtensionPropertiesNames[app->deviceExtensionPropertiesNamesSize++] = &app->deviceExtensionProperties[i].extensionName;
	}

	app->graphicsQueueFamilyPriority = 1.0f;

	app->vulkanDeviceQueueCreateInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	app->vulkanDeviceQueueCreateInfo[0].queueFamilyIndex = app->graphicsQueueFamilyIndex - 1;
	app->vulkanDeviceQueueCreateInfo[0].queueCount = 1;
	app->vulkanDeviceQueueCreateInfo[0].pQueuePriorities = &app->graphicsQueueFamilyPriority;

	vkGetPhysicalDeviceFeatures(app->vulkanPhysicalDevice, &app->vulkanPhysicalDeviceFeatures);

	app->vulkanDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	app->vulkanDeviceCreateInfo.queueCreateInfoCount = ARRAY_SIZE(app->vulkanDeviceQueueCreateInfo);
	app->vulkanDeviceCreateInfo.pQueueCreateInfos = app->vulkanDeviceQueueCreateInfo;
	app->vulkanDeviceCreateInfo.pEnabledFeatures = &app->vulkanPhysicalDeviceFeatures;
	app->vulkanDeviceCreateInfo.enabledLayerCount = app->enabledVulkanValidationLayersSize;
	app->vulkanDeviceCreateInfo.ppEnabledLayerNames = app->enabledVulkanValidationLayers;
	app->vulkanDeviceCreateInfo.ppEnabledExtensionNames = app->deviceExtensionPropertiesNames;
	app->vulkanDeviceCreateInfo.enabledExtensionCount = app->deviceExtensionPropertiesNamesSize;

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

	if (app->vulkanSurfaceCapabilities.currentExtent.width != UINT32_MAX) {
		app->vulkanSurfaceExtent.height = app->vulkanSurfaceCapabilities.currentExtent.height;
		app->vulkanSurfaceExtent.width = app->vulkanSurfaceCapabilities.currentExtent.width;
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
	app->vulkanSwapChainCreateInfo.imageExtent.height = app->vulkanSurfaceExtent.height;
	app->vulkanSwapChainCreateInfo.imageExtent.width = app->vulkanSurfaceExtent.width;
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

	err = vkGetSwapchainImagesKHR(app->vulkanDevice, app->vulkanSwapChain, &app->vulkanSwapChainImagesSize, NULL);
	assert(!err && app->vulkanSwapChainImagesSize);
	app->vulkanSwapChainImages = malloc(sizeof(VkImage) * app->vulkanSwapChainImagesSize);
	assert(app->vulkanSwapChainImages);
	err = vkGetSwapchainImagesKHR(app->vulkanDevice, app->vulkanSwapChain, &app->vulkanSwapChainImagesSize, app->vulkanSwapChainImages);
	assert(!err);
	//	for (uint32_t i = 0; i < app->vulkanSwapChainImagesSize; i++){
	//		app->vulkanSwapChainImages[i].usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	//	}
}

void initImageViews(Application* app) {
	VkResult err = VK_SUCCESS;

	app->vulkanSwapChainImageViewsSize = app->vulkanSwapChainImagesSize;
	app->vulkanSwapChainImageViews = malloc(sizeof(VkImageView) * app->vulkanSwapChainImageViewsSize);
	assert(app->vulkanSwapChainImageViews);
	for (size_t i = 0; i < app->vulkanSwapChainImageViewsSize; i++) {
		VkImageViewCreateInfo createInfo;
		memset(&createInfo, 0, sizeof(createInfo));
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = app->vulkanSwapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = app->vulkanSurfaceChosenFormat.format;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;


		err = vkCreateImageView(app->vulkanDevice, &createInfo, NULL, &app->vulkanSwapChainImageViews);
		assert(!err);

	}
}
void initRenderPass(Application* app) {
	VkResult err = VK_SUCCESS;

	VkAttachmentDescription colorAttachment;
	memset(&colorAttachment, 0, sizeof(colorAttachment));
	colorAttachment.format = app->vulkanSurfaceChosenFormat.format;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef;
	memset(&colorAttachmentRef, 0, sizeof(colorAttachmentRef));
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass;
	memset(&subpass, 0, sizeof(subpass));
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency;
	memset(&dependency, 0, sizeof(dependency));
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassCreateInfo;
	memset(&renderPassCreateInfo, 0, sizeof(renderPassCreateInfo));
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = 1;
	renderPassCreateInfo.pAttachments = &colorAttachment;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;
	renderPassCreateInfo.dependencyCount = 1;
	renderPassCreateInfo.pDependencies = &dependency;


	err = vkCreateRenderPass(app->vulkanDevice, &renderPassCreateInfo, NULL, &app->vulkanRenderPass);
	assert(!err);

}

void initGraphicsPipeline(Application* app) {
	VkResult err = VK_SUCCESS;
	loadShaders(app);

	VkPipelineShaderStageCreateInfo shaderStageInfos[2];
	memset(shaderStageInfos, 0, sizeof(shaderStageInfos));
	shaderStageInfos[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageInfos[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	shaderStageInfos[0].module = app->vulkanShaderModules[0];
	shaderStageInfos[0].pName = "main";

	shaderStageInfos[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageInfos[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderStageInfos[1].module = app->vulkanShaderModules[1];
	shaderStageInfos[1].pName = "main";


	VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT,VK_DYNAMIC_STATE_SCISSOR };
	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo;
	memset(&dynamicStateCreateInfo, 0, sizeof(dynamicStateCreateInfo));
	dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateCreateInfo.dynamicStateCount = ARRAY_SIZE(dynamicStates);
	dynamicStateCreateInfo.pDynamicStates = dynamicStates;

	VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
	memset(&vertexInputStateCreateInfo, 0, sizeof(vertexInputStateCreateInfo));
	vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0;
	vertexInputStateCreateInfo.pVertexBindingDescriptions = NULL;
	vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0;
	vertexInputStateCreateInfo.pVertexAttributeDescriptions = NULL;

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;
	memset(&inputAssemblyStateCreateInfo, 0, sizeof(inputAssemblyStateCreateInfo));
	inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

	//	VkViewport viewPort;
	//	memset(&viewPort, 0, sizeof(viewPort));
	//	viewPort.x = 0.0f;
	//	viewPort.y = 0.0f;
	//	viewPort.width = app->vulkanSurfaceExtent.width;//implicit conversion
	//	viewPort.height = app->vulkanSurfaceExtent.height;//implicit conversion
	//	viewPort.minDepth = 0.0f;
	//	viewPort.maxDepth = 1.0f;
	//
	//	VkRect2D scissors;
	//	memset(&scissors, 0, sizeof(scissors));
	//	scissors.offset.x = 0;
	//	scissors.offset.y = 0;
	//	scissors.extent = app->vulkanSurfaceExtent;

	VkPipelineViewportStateCreateInfo  viewPortState;
	memset(&viewPortState, 0, sizeof(viewPortState));
	viewPortState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewPortState.viewportCount = 1;
	//	viewPortState.pViewports = &viewPort;
	viewPortState.scissorCount = 1;
	//	viewPortState.pScissors = &scissors;

	VkPipelineRasterizationStateCreateInfo rasterizer;
	memset(&rasterizer, 0, sizeof(rasterizer));
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f;
	rasterizer.depthBiasClamp = 0.0f;
	rasterizer.depthBiasSlopeFactor = 0.0f;

	VkPipelineMultisampleStateCreateInfo multisampling;
	memset(&multisampling, 0, sizeof(multisampling));
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = NULL;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState colorBlend;
	memset(&colorBlend, 0, sizeof(colorBlend));
	colorBlend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlend.blendEnable = VK_FALSE;
	colorBlend.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlend.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlend.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlend.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlend.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlend.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

	VkPipelineColorBlendStateCreateInfo colorBlending;
	memset(&colorBlending, 0, sizeof(colorBlending));
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlend;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
	memset(&pipelineLayoutCreateInfo, 0, sizeof(pipelineLayoutCreateInfo));
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 0;
	pipelineLayoutCreateInfo.pSetLayouts = NULL;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	pipelineLayoutCreateInfo.pPushConstantRanges = NULL;

	err = vkCreatePipelineLayout(app->vulkanDevice, &pipelineLayoutCreateInfo, NULL, &app->vulkanPipelineLayout);
	assert(!err);

	VkGraphicsPipelineCreateInfo pipelineCreateInfo;
	memset(&pipelineCreateInfo, 0, sizeof(pipelineCreateInfo));
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.stageCount = ARRAY_SIZE(shaderStageInfos);
	pipelineCreateInfo.pStages = shaderStageInfos;
	pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
	pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
	pipelineCreateInfo.pViewportState = &viewPortState;
	pipelineCreateInfo.pRasterizationState = &rasterizer;
	pipelineCreateInfo.pMultisampleState = &multisampling;
	pipelineCreateInfo.pDepthStencilState = NULL;
	pipelineCreateInfo.pColorBlendState = &colorBlending;
	pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
	pipelineCreateInfo.layout = app->vulkanPipelineLayout;
	pipelineCreateInfo.renderPass = app->vulkanRenderPass;
	pipelineCreateInfo.subpass = 0;
	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineCreateInfo.basePipelineIndex = -1;

	err = vkCreateGraphicsPipelines(app->vulkanDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, NULL, &app->vulkanGraphicsPipeline);
	assert(!err);
}
void initFrameBuffer(Application* app) {
	VkResult err = VK_SUCCESS;
	app->vulkanFrameBuffersSize = app->vulkanSwapChainImageViewsSize;
	app->vulkanFramebuffers = malloc(sizeof(VkFramebuffer) * app->vulkanFrameBuffersSize);
	assert(app->vulkanFramebuffers);

	VkFramebufferCreateInfo frameBufferInfo;
	memset(&frameBufferInfo, 0, sizeof(frameBufferInfo));
	frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferInfo.renderPass = app->vulkanRenderPass;
	frameBufferInfo.attachmentCount = 1;
	frameBufferInfo.width = app->vulkanSurfaceExtent.width;
	frameBufferInfo.height = app->vulkanSurfaceExtent.height;
	frameBufferInfo.layers = 1;


	for (size_t i = 0; i < app->vulkanFrameBuffersSize; i++) {
		VkImageView const* attachments = &app->vulkanSwapChainImageViews[i];
		frameBufferInfo.pAttachments = attachments;

		err = vkCreateFramebuffer(app->vulkanDevice, &frameBufferInfo, NULL, app->vulkanFramebuffers[i]);
		assert(!err);
	}
}
void initCommandPool(Application* app) {
	VkResult err = VK_SUCCESS;
	VkCommandPoolCreateInfo commandPoolInfo;
	memset(&commandPoolInfo, 0, sizeof(commandPoolInfo));
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolInfo.queueFamilyIndex = app->graphicsQueueFamilyIndex - 1;
	err = vkCreateCommandPool(app->vulkanDevice, &commandPoolInfo, NULL, &app->vulkanCommandPool);
	assert(!err);
}

void initCommandBuffer(Application* app) {
	VkResult err = VK_SUCCESS;
	VkCommandBufferAllocateInfo bufferInfo;
	memset(&bufferInfo, 0, sizeof(bufferInfo));
	bufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	bufferInfo.commandPool = app->vulkanCommandPool;
	bufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	bufferInfo.commandBufferCount = 1;

	err = vkAllocateCommandBuffers(app->vulkanDevice, &bufferInfo, &app->vulkanCommandBuffer);
	assert(!err);

}
void initSyncObjects(Application* app) {
	VkResult err = VK_SUCCESS;
	VkSemaphoreCreateInfo semaphoreInfo;
	memset(&semaphoreInfo, 0, sizeof(semaphoreInfo));
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	VkFenceCreateInfo fenceInfo;
	memset(&fenceInfo, 0, sizeof(fenceInfo));
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	err = vkCreateSemaphore(app->vulkanDevice, &semaphoreInfo, NULL, &app->imageAvailable);
	assert(!err);
	err = vkCreateSemaphore(app->vulkanDevice, &semaphoreInfo, NULL, &app->renderFinished);
	assert(!err);
	err = vkCreateFence(app->vulkanDevice, &fenceInfo, NULL, &app->inFlight);
	assert(!err);
}

void destroy(Application* app) {
	vkDestroySemaphore(app->vulkanDevice, app->imageAvailable, NULL);
	vkDestroySemaphore(app->vulkanDevice, app->renderFinished, NULL);
	vkDestroyFence(app->vulkanDevice, app->inFlight, NULL);

	vkDestroyCommandPool(app->vulkanDevice, app->vulkanCommandPool, NULL);
	for (size_t i = 0; i < app->vulkanFrameBuffersSize; i++) {
		vkDestroyFramebuffer(app->vulkanDevice, app->vulkanFramebuffers[i], NULL);
	}
	vkDestroyPipeline(app->vulkanDevice, app->vulkanGraphicsPipeline, NULL);
	vkDestroyPipelineLayout(app->vulkanDevice, app->vulkanPipelineLayout, NULL);
	vkDestroyRenderPass(app->vulkanDevice, app->vulkanRenderPass, NULL);
	for (size_t i = 0; i < app->vulkanSwapChainImageViewsSize; i++) {
		vkDestroyImageView(app->vulkanDevice, app->vulkanSwapChainImageViews[i], NULL);
	}
	vkDestroySwapchainKHR(app->vulkanDevice, app->vulkanSwapChain, NULL);
	vkDestroyDevice(app->vulkanDevice, NULL);
	vkDestroySurfaceKHR(app->vulkanInstance, app->vulkanSurface, NULL);
	vkDestroyInstance(app->vulkanInstance, NULL);

	free(app->properties);
	free(app->layers);
	free(app->physicalDevices);
	free(app->deviceExtensionProperties);
	free(app->vulkanSwapChainImages);
}


void getWindowSize(Application* app, int* width, int* height) {
	BOOL succeed = 0;
	RECT rect;
	succeed = GetWindowRect(app->hwnd, &rect);
	assert(succeed);

	*width = rect.right - rect.left;
	*height = rect.bottom - rect.top;
	//assert(*width > 0 && *height > 0);
}

void loadShaders(Application* app) {
	VkResult err = VK_SUCCESS;

	const char vertexPath[] = PROJECT_PATH("shader\\vert.spv");
	const char fragmePath[] = PROJECT_PATH("shader\\frag.spv");

	FILE* vertexFile = fopen(vertexPath, "rb");
	FILE* fragmeFile = fopen(fragmePath, "rb");
	assert(vertexFile && fragmeFile);
	fseek(vertexFile, 0L, SEEK_END);
	fseek(fragmeFile, 0L, SEEK_END);
	size_t vertexSize = ftell(vertexFile);
	size_t fragmeSize = ftell(fragmeFile);
	assert(vertexSize && fragmeSize);
	char* vertexProgram = malloc(sizeof(char) * vertexSize);
	char* fragmeProgram = malloc(sizeof(char) * vertexSize);
	assert(vertexProgram && fragmeProgram);
	rewind(vertexFile);
	for (size_t i = 0; i < vertexSize; i++) {
		vertexProgram[i] = getc(vertexFile);
	}
	rewind(fragmeFile);
	for (size_t i = 0; i < fragmeSize; i++) {
		fragmeProgram[i] = getc(fragmeFile);
	}
	VkShaderModuleCreateInfo shadersInfo[2];
	memset(shadersInfo, 0, sizeof(shadersInfo));
	shadersInfo[0].sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shadersInfo[1].sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

	shadersInfo[0].codeSize = vertexSize;
	shadersInfo[0].pCode = vertexProgram;
	shadersInfo[1].codeSize = fragmeSize;
	shadersInfo[1].pCode = fragmeProgram;

	err = vkCreateShaderModule(app->vulkanDevice, &shadersInfo[0], NULL, &app->vulkanShaderModules[0]);
	assert(!err);
	err = vkCreateShaderModule(app->vulkanDevice, &shadersInfo[1], NULL, &app->vulkanShaderModules[1]);
	assert(!err);

	free(vertexProgram);
	free(fragmeFile);
}

int isDeviceOkAndSetQueueIndex(Application* app, VkPhysicalDevice device) {
	VkResult err = VK_SUCCESS;
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
	for (uint32_t i = 0; i < count; i++) {
		hasExtension = !strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME, extensionProperties[i].extensionName);
		if (hasExtension)break;
	}
	free(extensionProperties);
	extensionProperties = NULL;

	count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &count, NULL);
	assert(count);
	VkQueueFamilyProperties* queueFamilyProperties = malloc(sizeof(VkQueueFamilyProperties) * count);
	assert(queueFamilyProperties);
	memset(queueFamilyProperties, 0, sizeof(VkQueueFamilyProperties) * count);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &count, queueFamilyProperties);
	assert(count);
	for (uint32_t i = 0; i < count; i++) {
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
	queueFamilyProperties = NULL;


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
void recordCommandbuffer(Application* app, uint32_t frame) {
	VkResult err = VK_SUCCESS;
	VkCommandBufferBeginInfo beginInfo;
	memset(&beginInfo, 0, sizeof(beginInfo));
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	err = vkBeginCommandBuffer(app->vulkanCommandBuffer, &beginInfo);
	assert(!err);


	VkClearValue clearColor;
	memset(&clearColor, 0, sizeof(clearColor));
	clearColor.color.float32[3] = 1.0f;

	VkRenderPassBeginInfo renderPassInfo;
	memset(&renderPassInfo, 0, sizeof(renderPassInfo));

	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = app->vulkanRenderPass;
	renderPassInfo.framebuffer = app->vulkanFramebuffers[frame];
	renderPassInfo.renderArea.offset.x = 0;
	renderPassInfo.renderArea.offset.y = 0;
	renderPassInfo.renderArea.extent.height = app->vulkanSurfaceExtent.height;
	renderPassInfo.renderArea.extent.width = app->vulkanSurfaceExtent.width;
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	VkViewport viewPort;
	memset(&viewPort, 0, sizeof(viewPort));
	viewPort.x = 0.0f;
	viewPort.y = 0.0f;
	viewPort.width = app->vulkanSurfaceExtent.width;
	viewPort.height = app->vulkanSurfaceExtent.height;
	viewPort.minDepth = 0.0f;
	viewPort.maxDepth = 1.0f;

	VkRect2D scissors;
	memset(&scissors, 0, sizeof(scissors));
	scissors.offset.x = 0;
	scissors.offset.y = 0;
	scissors.extent.height = app->vulkanSurfaceExtent.height;
	scissors.extent.width = app->vulkanSurfaceExtent.width;

	vkCmdBeginRenderPass(app->vulkanCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(app->vulkanCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, app->vulkanGraphicsPipeline);
	vkCmdSetViewport(app->vulkanCommandBuffer, 0, 1, &viewPort);
	vkCmdSetScissor(app->vulkanCommandBuffer, 0, 1, &scissors);
	vkCmdDraw(app->vulkanCommandBuffer, 3, 1, 0, 0);
	vkCmdEndRenderPass(app->vulkanCommandBuffer);

	err = vkEndCommandBuffer(app->vulkanCommandBuffer);
	assert(!err);

}
void drawFrame(Application* app) {
	VkResult err = VK_SUCCESS;
	uint32_t frame = 0;
	vkWaitForFences(app->vulkanDevice, 1, &app->inFlight, VK_TRUE, UINT64_MAX);
	vkResetFences(app->vulkanDevice, 1, &app->inFlight);

	err = vkAcquireNextImageKHR(app->vulkanDevice, app->vulkanSwapChain, UINT64_MAX, app->imageAvailable, VK_NULL_HANDLE, &frame);
	assert(!err);
	err = vkResetCommandBuffer(app->vulkanCommandBuffer, 0);
	assert(!err);
	recordCommandbuffer(app, frame);

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSubmitInfo submitInfo;
	memset(&submitInfo, 0, sizeof(submitInfo));
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &app->imageAvailable;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &app->vulkanCommandBuffer;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &app->renderFinished;
	err = vkQueueSubmit(app->vulkanQueue, 1, &submitInfo, app->inFlight);
	assert(!err);

	VkPresentInfoKHR presentInfo;
	memset(&presentInfo, 0, sizeof(presentInfo));
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &app->renderFinished;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &app->vulkanSwapChain;
	presentInfo.pResults = &err;
	vkQueuePresentKHR(app->vulkanQueue, &presentInfo);
	assert(!err);


}
