// Copyright (c) 2011-2012, Zeex
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met: 
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer. 
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <algorithm>
#include <cstring>
#include <iterator>
#include <list>
#include <string>

#include "configreader.h"
#include "plugin.h"

#include "sdk/plugincommon.h"
#include "sdk/amx/amx.h"

#ifdef WIN32
	#define PLUGIN_EXT ".dll"
#else
	#define PLUGIN_EXT ".so"
#endif

typedef void (*logprintf_t)(const char *format, ...);
static logprintf_t logprintf;

static void **ppPluginData;

static std::list<Plugin*> plugins;

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) {
	logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];
	::ppPluginData = ppData;

	ConfigReader server_cfg("server.cfg");

	std::stringstream name_stream;
	name_stream << server_cfg.GetOption("my_plugins", std::string());

	std::list<std::string> names;

	std::copy(
		std::istream_iterator<std::string>(name_stream),
		std::istream_iterator<std::string>(),
		std::back_inserter(names)
	);

	for (std::list<std::string>::iterator iterator = names.begin(); iterator != names.end(); ++iterator) {
		std::string &name = *iterator;
		logprintf("  Loading plugin: %s", name.c_str());

		Plugin *plugin = new Plugin;
		if (plugin == 0) {
			logprintf("   Failed.");
			continue;
		}

		std::string path = "plugins/" + name;
		PluginError error = plugin->Load(path, ppPluginData);

		if (!plugin->IsLoaded()) {
			path.append(PLUGIN_EXT);
			error = plugin->Load(path, ppPluginData);
		}

		if (!plugin->IsLoaded()) {
			delete plugin;

			switch (error) {
				case PLUGIN_ERROR_LOAD: {
					logprintf("   Failed.");
					break;
				}
				case PLUGIN_ERROR_VERSION:
					logprintf("   Unsupported version.");
					break;
				case PLUGIN_ERROR_API:
					logprintf("   Plugin does not conform to acrhitecture.");
					break;
			}

			continue;
		}

		plugins.push_back(plugin);
		logprintf("   Loaded.");
	}

	logprintf("  Loaded %d plugins.", plugins.size());

	return true;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) {
	for (std::list<Plugin*>::iterator iterator = plugins.begin();
			iterator != plugins.end(); ++iterator) {
		(*iterator)->AmxLoad(amx);
	}
	return AMX_ERR_NONE;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) {
	for (std::list<Plugin*>::iterator iterator = plugins.begin();
			iterator != plugins.end(); ++iterator) {
		(*iterator)->AmxUnload(amx);
	}
	return AMX_ERR_NONE;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() {
	return;
}

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick() {
	for (std::list<Plugin*>::iterator iterator = plugins.begin();
			iterator != plugins.end(); ++iterator) {
		(*iterator)->ProcessTick();
	}
}
