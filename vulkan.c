#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <vulkan/vulkan.h>
#include <assert.h>
#include <string.h>



//#define GET_INSTANCE_PROC_ADDR(inst, entrypoint)                                                              \
//    {                                                                                                         \
//        demo->fp##entrypoint = (PFN_vk##entrypoint)vkGetInstanceProcAddr(inst, "vk" #entrypoint);             \
//        if (demo->fp##entrypoint == NULL) {                                                                   \
//            ERR_EXIT("vkGetInstanceProcAddr failed to find vk" #entrypoint, "vkGetInstanceProcAddr Failure"); \
//        }                                                                                                     \
//    }

const char APP_NAME[] = "Hello Triangle";
const char ENGINE_NAME[] = "No Engine";

//const int validate = 1;
//const char VALIDATION_LAYER[] = "VK_LAYER_KHRONOS_validation";
//static int validationError = 0;



//static VkBool32 check_layers(uint32_t, char**, uint32_t, VkLayerProperties*);
//VKAPI_ATTR VkBool32 VKAPI_CALL debug_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT,
//	VkDebugUtilsMessageTypeFlagsEXT,
//	const VkDebugUtilsMessengerCallbackDataEXT*,
//	void*);


void createVulkanInstance() {
	VkResult err;
	VkInstance instance;
	//char* instanceValidationLayers[] = { VALIDATION_LAYER };
	uint32_t instanceExtensionCount = 0;
	uint32_t enabledExtensionCount = 0;
	uint32_t enabledLayerCount = 0;
	uint32_t isMinimized = 0;
	VkCommandPool cmdPool = VK_NULL_HANDLE;
	char* extensionNames[64] = { 0 };
	char* enabledLayers[64] = { 0 };

	VkApplicationInfo appInfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = APP_NAME,
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = ENGINE_NAME,
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = VK_API_VERSION_1_0
	};
	VkInstanceCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &appInfo,
		.enabledExtensionCount = 0,
		.ppEnabledExtensionNames = NULL,
		.enabledLayerCount = 0,
	};

	//	uint32_t instanceLayerCount = 0;
	//	VkBool32 validationFound = 0;
	//	if (validate) {
	//		err = vkEnumerateInstanceLayerProperties(&instanceLayerCount, NULL);
	//		assert(!err);
	//		if (instanceLayerCount) {
	//			VkLayerProperties* instanceLayers = malloc(sizeof(VkLayerProperties) * instanceLayerCount);
	//			err = vkEnumerateInstanceLayerProperties(&instanceLayerCount, instanceLayers);
	//			assert(!err);
	//
	//			validationFound = check_layers(ARRAY_SIZE(instanceValidationLayers), instanceValidationLayers, instanceLayerCount, instanceLayers);
	//			if (validationFound) {
	//				enabledLayerCount = ARRAY_SIZE(instanceValidationLayers);
	//				enabledLayers[0] = VALIDATION_LAYER;
	//			}
	//			free(instanceLayers);
	//		}
	//		assert(validationFound);
	//	}

	VkBool32 surfaceExtFound = 0;
	VkBool32 platformSurfaceExtFound = 0;

	err = vkEnumerateInstanceExtensionProperties(NULL, &instanceExtensionCount, NULL);
	assert(!err);

	if (instanceExtensionCount) {
		VkExtensionProperties* instanceExtensions = malloc(sizeof(VkExtensionProperties) * instanceExtensionCount);
		err = vkEnumerateInstanceExtensionProperties(NULL, &instanceExtensionCount, instanceExtensions);
		assert(!err);

		for (size_t i = 0; i < instanceExtensionCount; i++) {
			if (!strcmp(VK_KHR_SURFACE_EXTENSION_NAME, instanceExtensions[i].extensionName)) {
				surfaceExtFound = 1;
				extensionNames[enabledExtensionCount++] = VK_KHR_SURFACE_EXTENSION_NAME;
			}
#if defined(VK_USE_PLATFORM_WIN32_KHR)
			if (!strcmp(VK_KHR_WIN32_SURFACE_EXTENSION_NAME, instanceExtensions[i].extensionName)) {
				platformSurfaceExtFound = 1;
				extensionNames[enabledExtensionCount++] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
			}
#endif
			if (!strcmp(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, instanceExtensions[i].extensionName)) {
				extensionNames[enabledExtensionCount++] = VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME;
			}
			//			if (!strcmp(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, instanceExtensions[i].extensionName)) {
			//				if (validate) {
			//					extensionNames[enabledExtensionCount++] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
			//				}
			//			}
			assert(enabledExtensionCount < 64);
		}
		free(instanceExtensions);
	}
	assert(surfaceExtFound);
#if defined(VK_USE_PLATFORM_WIN32_KHR)
	assert(platformSurfaceExtFound);
#endif

	//	createInfo.enabledLayerCount = enabledLayerCount;
	//	createInfo.ppEnabledLayerNames = instanceValidationLayers;
	createInfo.enabledExtensionCount = enabledExtensionCount;
	createInfo.ppEnabledExtensionNames = extensionNames;

	//	VkDebugUtilsMessengerCreateInfoEXT dbgMessengerCreateInfo =
	//	{
	//		// VK_EXT_debug_utils style
	//		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
	//		.pNext = NULL,
	//		.flags = 0,
	//		.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
	//		.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
	//		.pfnUserCallback = debug_messenger_callback,
	//		.pUserData = NULL
	//	};
	//	createInfo.pNext = &dbgMessengerCreateInfo;

	err = vkCreateInstance(&createInfo, NULL, &instance);
	assert(!err);

	//	uint32_t gpuCount = 0;
	//	err = vkEnumeratePhysicalDevices(instance, &gpuCount, NULL);
	//	assert(!err && gpuCount);
	//
	//	VkPhysicalDevice* physicalDevices = malloc(sizeof(VkPhysicalDevice) * gpuCount);
	//	err = vkEnumeratePhysicalDevices(instance, &gpuCount, physicalDevices);
	//	assert(!err);
	//
	//	int gpuNumber = -1;
	//	int selectedGpu = -1;
	//
	//	/* Try to auto select most suitable device */
	//	uint32_t countDeviceType[VK_PHYSICAL_DEVICE_TYPE_CPU + 1] = 0;
	//
	//
	//	VkPhysicalDeviceProperties physicalDeviceProperties;
	//	for (uint32_t i = 0; i < gpuCount; i++) {
	//		vkGetPhysicalDeviceProperties(physicalDevices[i], &physicalDeviceProperties);
	//		assert(physicalDeviceProperties.deviceType <= VK_PHYSICAL_DEVICE_TYPE_CPU);
	//		countDeviceType[physicalDeviceProperties.deviceType]++;
	//	}
	//
	//	VkPhysicalDeviceType searchForDeviceType = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
	//	if (countDeviceType[VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU]) {
	//		searchForDeviceType = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
	//	}
	//	else if (countDeviceType[VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU]) {
	//		searchForDeviceType = VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
	//	}
	//	else if (countDeviceType[VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU]) {
	//		searchForDeviceType = VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU;
	//	}
	//	else if (countDeviceType[VK_PHYSICAL_DEVICE_TYPE_CPU]) {
	//		searchForDeviceType = VK_PHYSICAL_DEVICE_TYPE_CPU;
	//	}
	//	else if (countDeviceType[VK_PHYSICAL_DEVICE_TYPE_OTHER]) {
	//		searchForDeviceType = VK_PHYSICAL_DEVICE_TYPE_OTHER;
	//	}
	//
	//	for (uint32_t i = 0; i < gpuCount; i++) {
	//		vkGetPhysicalDeviceProperties(physicalDevices[i], &physicalDeviceProperties);
	//		if (physicalDeviceProperties.deviceType == searchForDeviceType) {
	//			gpuNumber = i;
	//			break;
	//		}
	//	}
	//	assert(gpuNumber >= 0);
	//	selectedGpu = physicalDevices[gpuNumber];
	//	{
	//		vkGetPhysicalDeviceProperties(selectedGpu, &physicalDeviceProperties);
	//	}
	//	free(physicalDevices);
	//
	//	uint32_t deviceExtensionCount = 0;
	//	VkBool32 swapchainExtFound = 0;
	//	VkBool32 khrIncrementalPresent = 0;
	//	VkBool32 googleDisplayTimerPresent = 0;
	//	enabledExtensionCount = 0;
	//	memset(extensionNames, 0, sizeof(extensionNames));
	//
	//	err = vkEnumerateDeviceExtensionProperties(selectedGpu, NULL, &deviceExtensionCount, NULL);
	//	assert(!err);
	//
	//	if (deviceExtensionCount) {
	//		VkExtensionProperties* deviceExtensions = malloc(sizeof(VkExtensionProperties) * deviceExtensionCount);
	//		err = vkEnumerateDeviceExtensionProperties(selectedGpu, NULL, &deviceExtensionCount, deviceExtensions);
	//		assert(!err);
	//		for (size_t i = 0; i < deviceExtensionCount; i++) {
	//			if (!strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME, deviceExtensions[i].extensionName)) {
	//				swapchainExtFound = 1;
	//				extensionNames[enabledExtensionCount++] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
	//			}
	//			if (!strcmp("VK_KHR_portability_subset", deviceExtensions[i].extensionName)) {
	//				extensionNames[enabledExtensionCount++] = "VK_KHR_portability_subset";
	//			}
	//			if (!strcmp(VK_KHR_INCREMENTAL_PRESENT_EXTENSION_NAME, deviceExtensions[i].extensionName)) {
	//				khrIncrementalPresent = 1;
	//				extensionNames[enabledExtensionCount++] = VK_KHR_INCREMENTAL_PRESENT_EXTENSION_NAME;
	//			}
	//			if (!strcmp(VK_GOOGLE_DISPLAY_TIMING_EXTENSION_NAME, deviceExtensions[i].extensionName)) {
	//				googleDisplayTimerPresent = 1;
	//				extensionNames[enabledExtensionCount++] = VK_GOOGLE_DISPLAY_TIMING_EXTENSION_NAME;
	//			}
	//			assert(enabledExtensionCount < 64);
	//
	//		}
	//		free(deviceExtensions);
	//	}

	//	assert(swapchainExtFound);
	//	PFN_vkCreateDebugUtilsMessengerEXT CreateDebugUtilsMessengerEXT;
	//	PFN_vkDestroyDebugUtilsMessengerEXT	DestroyDebugUtilsMessengerEXT;
	//	PFN_vkSubmitDebugUtilsMessageEXT	SubmitDebugUtilsMessageEXT;
	//	PFN_vkCmdBeginDebugUtilsLabelEXT	CmdBeginDebugUtilsLabelEXT;
	//	PFN_vkCmdEndDebugUtilsLabelEXT	CmdEndDebugUtilsLabelEXT;
	//	PFN_vkCmdInsertDebugUtilsLabelEXT	CmdInsertDebugUtilsLabelEXT;
	//	PFN_vkSetDebugUtilsObjectNameEXT	SetDebugUtilsObjectNameEXT;
	//	if (validate) {
	//		// Setup VK_EXT_debug_utils function pointers always (we use them for
	//		// debug labels and names).
	//		CreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	//		DestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	//		SubmitDebugUtilsMessageEXT = (PFN_vkSubmitDebugUtilsMessageEXT)vkGetInstanceProcAddr(instance, "vkSubmitDebugUtilsMessageEXT");
	//		CmdBeginDebugUtilsLabelEXT = (PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance, "vkCmdBeginDebugUtilsLabelEXT");
	//		CmdEndDebugUtilsLabelEXT = (PFN_vkCmdEndDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance, "vkCmdEndDebugUtilsLabelEXT");
	//		CmdInsertDebugUtilsLabelEXT = (PFN_vkCmdInsertDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance, "vkCmdInsertDebugUtilsLabelEXT");
	//		SetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(inst, "vkSetDebugUtilsObjectNameEXT");
	//		if (NULL == CreateDebugUtilsMessengerEXT || NULL == DestroyDebugUtilsMessengerEXT ||
	//			NULL == SubmitDebugUtilsMessageEXT || NULL == CmdBeginDebugUtilsLabelEXT ||
	//			NULL == CmdEndDebugUtilsLabelEXT || NULL == CmdInsertDebugUtilsLabelEXT ||
	//			NULL == SetDebugUtilsObjectNameEXT) {
	//			ERR_EXIT("GetProcAddr: Failed to init VK_EXT_debug_utils\n", "GetProcAddr: Failure");
	//		}
	//		err = CreateDebugUtilsMessengerEXT(instance, &dbg_messenger_create_info, NULL, &dbg_messenger);
	//		assert(!err);

	//	VkPhysicalDeviceProperties gpuProperties;
	//	uint32_t queueFamilyCount = 0;
	//	vkGetPhysicalDeviceProperties(selectedGpu, &gpuProperties);
	//	vkGetPhysicalDeviceQueueFamilyProperties(selectedGpu, &queueFamilyCount, NULL);
	//	assert(queueFamilyCount);
	//	VkQueueFamilyProperties* queueProperties = malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
	//	VkPhysicalDeviceFeatures physicalDeviceFeatures;
	//	vkGetPhysicalDeviceFeatures(selectedGpu, &physicalDeviceFeatures);
	//
	//	PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR = vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
	//	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR = vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
	//	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR = vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
	//	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR = vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");
	//	PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR = vkGetInstanceProcAddr(instance, "vkGetSwapchainImagesKHR");
	//
	//	assert(fpGetPhysicalDeviceSurfaceSupportKHR);
	//	assert(fpGetPhysicalDeviceSurfaceCapabilitiesKHR);
	//	assert(fpGetPhysicalDeviceSurfaceFormatsKHR);
	//	assert(fpGetPhysicalDeviceSurfacePresentModesKHR);
	//	assert(fpGetSwapchainImagesKHR);

}

//static VkBool32 check_layers(uint32_t check_count, char** check_names, uint32_t layer_count, VkLayerProperties* layers) {
//	for (uint32_t i = 0; i < check_count; i++) {
//		VkBool32 found = 0;
//		for (uint32_t j = 0; j < layer_count; j++) {
//			if (!strcmp(check_names[i], layers[j].layerName)) {
//				found = 1;
//				break;
//			}
//		}
//		if (!found) {
//			return 0;
//		}
//	}
//	return 1;
//}

//VKAPI_ATTR VkBool32 VKAPI_CALL debug_messenger_callback(
//	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
//	VkDebugUtilsMessageTypeFlagsEXT messageType,
//	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
//	void* pUserData) {
//
//	char prefix[64] = "";
//	char* message = (char*)malloc(strlen(pCallbackData->pMessage) + 5000);
//	assert(message);
//
//#ifndef WIN32
//	raise(SIGTRAP);
//#else
//	DebugBreak();
//#endif
//
//	if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
//		strcat(prefix, "VERBOSE : ");
//	}
//	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
//		strcat(prefix, "INFO : ");
//	}
//	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
//		strcat(prefix, "WARNING : ");
//	}
//	else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
//		strcat(prefix, "ERROR : ");
//	}
//
//	if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
//		strcat(prefix, "GENERAL");
//	}
//	else {
//		if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
//			strcat(prefix, "VALIDATION");
//			//validationError = 1;
//		}
//		if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
//			if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
//				strcat(prefix, "|");
//			}
//			strcat(prefix, "PERFORMANCE");
//		}
//	}
//
//	sprintf(message, "%s - Message Id Number: %d | Message Id Name: %s\n\t%s\n", prefix, pCallbackData->messageIdNumber,
//		pCallbackData->pMessageIdName, pCallbackData->pMessage);
//	if (pCallbackData->objectCount > 0) {
//		char tmp_message[500];
//		sprintf(tmp_message, "\n\tObjects - %d\n", pCallbackData->objectCount);
//		strcat(message, tmp_message);
//		for (uint32_t object = 0; object < pCallbackData->objectCount; ++object) {
//			if (NULL != pCallbackData->pObjects[object].pObjectName && strlen(pCallbackData->pObjects[object].pObjectName) > 0) {
//				sprintf(tmp_message, "\t\tObject[%d] - %s, Handle %p, Name \"%s\"\n", object,
//					string_VkObjectType(pCallbackData->pObjects[object].objectType),
//					(void*)(pCallbackData->pObjects[object].objectHandle), pCallbackData->pObjects[object].pObjectName);
//			}
//			else {
//				sprintf(tmp_message, "\t\tObject[%d] - %s, Handle %p\n", object,
//					string_VkObjectType(pCallbackData->pObjects[object].objectType),
//					(void*)(pCallbackData->pObjects[object].objectHandle));
//			}
//			strcat(message, tmp_message);
//		}
//	}
//	if (pCallbackData->cmdBufLabelCount > 0) {
//		char tmp_message[500];
//		sprintf(tmp_message, "\n\tCommand Buffer Labels - %d\n", pCallbackData->cmdBufLabelCount);
//		strcat(message, tmp_message);
//		for (uint32_t cmd_buf_label = 0; cmd_buf_label < pCallbackData->cmdBufLabelCount; ++cmd_buf_label) {
//			sprintf(tmp_message, "\t\tLabel[%d] - %s { %f, %f, %f, %f}\n", cmd_buf_label,
//				pCallbackData->pCmdBufLabels[cmd_buf_label].pLabelName, pCallbackData->pCmdBufLabels[cmd_buf_label].color[0],
//				pCallbackData->pCmdBufLabels[cmd_buf_label].color[1], pCallbackData->pCmdBufLabels[cmd_buf_label].color[2],
//				pCallbackData->pCmdBufLabels[cmd_buf_label].color[3]);
//			strcat(message, tmp_message);
//		}
//	}
//
//#ifdef _WIN32
//
//	MessageBox(NULL, message, "Alert", MB_OK);
//
//#endif
//
//	free(message);
//
//	// Don't bail out, but keep going.
//	return 0;
//}
