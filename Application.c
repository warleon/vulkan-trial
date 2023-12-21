#include "Application.h"

void applicationInit(Application* app, char* name, char* engineName) {
	app->name = name;
	app->engineName = engineName;


	app->vulkanApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app->vulkanApplicationInfo.pApplicationName = app->name;
	app->vulkanApplicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app->vulkanApplicationInfo.pEngineName = app->engineName;
	app->vulkanApplicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	app->vulkanApplicationInfo.apiVersion = VK_API_VERSION_1_0;



	app->vulkanInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	app->vulkanInstanceCreateInfo.pApplicationInfo = &app->vulkanApplicationInfo;
	app->vulkanInstanceCreateInfo.enabledExtensionCount = 0;
	app->vulkanInstanceCreateInfo.ppEnabledExtensionNames = NULL;
	app->vulkanInstanceCreateInfo.enabledLayerCount = 0;


};
