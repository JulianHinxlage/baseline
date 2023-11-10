//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#pragma once

#include <unordered_map>

namespace baseline {

	class Singleton {
	public:
		template<typename Class>
		static Class *get() {
			static Class** instance = getInstance<Class>();
			if (*instance == nullptr) {
				*instance = new Class();
			}
			return *instance;
		}

		template<typename Class>
		static void clear() {
			Class** instance = getInstance<Class>();
			if (*instance != nullptr) {
				delete *instance;
				*instance = nullptr;
			}
		}

	private:
		static void** getInstance(size_t hashCode);

		template<typename Class>
		static Class** getInstance() {
			return (Class**)getInstance(typeid(Class).hash_code());
		}
	};

}

