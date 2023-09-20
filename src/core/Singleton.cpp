//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#include "Singleton.h"

namespace baseline {

	void** Singleton::getInstance(size_t hashCode) {
		static std::unordered_map<size_t, void*> instances;
		auto entry = instances.find(hashCode);
		if (entry == instances.end()) {
			instances[hashCode] = nullptr;
			return &instances[hashCode];
		}
		return &entry->second;
	}

}
