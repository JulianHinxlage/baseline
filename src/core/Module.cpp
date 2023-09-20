//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#include "Module.h"

#if WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace baseline {

	Module::Module() {
		handle = nullptr;
		name = "";
		runtimeFile = "";
	}

	void Module::invoke(const std::string& function) {
		auto entry = functions.find(function);
		if (entry != functions.end()) {
			if (entry->second) {
				entry->second();
			}
			return;
		}

		if (handle) {
			Func ptr = (Func)GetProcAddress((HMODULE)handle, function.c_str());
			functions[function] = ptr;
			if (ptr) {
				ptr();
			}
		}
	}

}
