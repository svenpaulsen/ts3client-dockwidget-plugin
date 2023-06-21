/**
 * TeamSpeak 3 Client Dock Widget Plugin
 *
 * Copyright (c) Sven Paulsen. All rights reserved.
 */

#ifndef PLUGIN_H
#define PLUGIN_H

#include "shared.h"

#ifdef __cplusplus
extern "C" {
#endif

// required functions
Q_DECL_EXPORT const char* ts3plugin_name();
Q_DECL_EXPORT const char* ts3plugin_version();
Q_DECL_EXPORT const char* ts3plugin_author();
Q_DECL_EXPORT const char* ts3plugin_description();
Q_DECL_EXPORT int         ts3plugin_apiVersion();
Q_DECL_EXPORT int         ts3plugin_init();
Q_DECL_EXPORT void        ts3plugin_shutdown();
Q_DECL_EXPORT void        ts3plugin_setFunctionPointers(const struct TS3Functions functions);

// optional functions
Q_DECL_EXPORT void        ts3plugin_registerPluginID(const char* id);

#ifdef __cplusplus
}
#endif

#endif // PLUGIN_H
