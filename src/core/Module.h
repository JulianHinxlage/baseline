//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#pragma once

#include <string>
#include <unordered_map>

namespace baseline {

	class Module {
	public:
		std::string name;
		std::string file;
		std::string runtimeFile;

		Module();
		void invoke(const std::string& function);

	private:
		friend class ModuleManager;
		typedef void (*Func)();
		void* handle;
		std::unordered_map<std::string, Func> functions;
	};

}
