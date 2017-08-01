/**
 * TeamSpeak 3 Client Dock Widget Plugin
 *
 * Copyright (c) Sven Paulsen. All rights reserved.
 */

#include "plugin.h"
#include "widgethelper.h"

/**
 * Returns the name of the plugin.
 */
const char* ts3plugin_name()
{
    return PLUGIN_NAME;
}

/**
 * Returns the version number of the plugin.
 */
const char* ts3plugin_version()
{
    return PLUGIN_VER;
}

/**
 * Returns the author of the plugin.
 */
const char* ts3plugin_author()
{
    return "Sven 'ScP' Paulsen";
}

/**
 * Returns the description of the plugin.
 */
const char* ts3plugin_description()
{
    return "Plugin to adjust the TeamSpeak 3 Client UI by adding dock widgets.";
}

/**
 * Returns the API version number of the plugin.
 */
int ts3plugin_apiVersion()
{
    return PLUGIN_API;
}

/**
 * Loads the plugin.
 */
int ts3plugin_init()
{
    if(WidgetHelper::instance()->hook())
    {
        pluginSDK.logMessage("Failed to initialize plugin; no QMainWindow available", LogLevel_ERROR, PLUGIN_NAME, 0);

        return 1;
    }

    pluginSDK.logMessage(QString("Plugin initialized; version %1 (API %2) built on %3 %4").arg(ts3plugin_version(), QString::number(ts3plugin_apiVersion()), __DATE__, __TIME__).toUtf8().data(), LogLevel_INFO, PLUGIN_NAME, 0);

    return 0;
}

/**
 * Unloads the plugin.
 */
void ts3plugin_shutdown()
{
    WidgetHelper::instance()->unhook();

    if(pluginID)
    {
        free(pluginID);
        pluginID = NULL;
    }
}

/**
 * Registers TeamSpeak 3 callback functions.
 */
void ts3plugin_setFunctionPointers(const struct TS3Functions functions)
{
    pluginSDK = functions;
}

/**
 * Registers an auto-generated plugin ID used for return codes, commands, menus and hotkeys.
 */
void ts3plugin_registerPluginID(const char* id)
{
    pluginID = (char*) malloc((strlen(id) +1)*sizeof(char));

    strncpy(pluginID, id, strlen(id)+1);
}
