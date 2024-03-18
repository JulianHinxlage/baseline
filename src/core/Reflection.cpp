//
// Copyright (c) 2024 Julian Hinxlage. All rights reserved.
//

#include "Reflection.h"

namespace baseline {

	std::vector<std::shared_ptr<TypeDescriptor>>& Reflection::getTypesImpl() {
		static std::vector<std::shared_ptr<TypeDescriptor>> types;
		return types;
	}

	std::map<std::string, std::shared_ptr<TypeDescriptor>>& Reflection::getTypesByNameImpl() {
		static std::map<std::string, std::shared_ptr<TypeDescriptor>> typesByName;
		return typesByName;
	}

	void Reflection::initTypeImpl(std::shared_ptr<TypeDescriptor> type) {
		bool isPrimitive = false;
		if (type->hash == (int)typeid(float).hash_code()) {
			isPrimitive = true;
			if (type->name.empty()) {
				type->name = "float";
			}
		}
		if (type->hash == (int)typeid(double).hash_code()) {
			isPrimitive = true;
			if (type->name.empty()) {
				type->name = "double";
			}
		}
		if (type->hash == (int)typeid(int8_t).hash_code()) {
			isPrimitive = true;
			if (type->name.empty()) {
				type->name = "int8";
			}
		}
		if (type->hash == (int)typeid(uint8_t).hash_code()) {
			isPrimitive = true;
			if (type->name.empty()) {
				type->name = "uint8";
			}
		}
		if (type->hash == (int)typeid(int16_t).hash_code()) {
			isPrimitive = true;
			if (type->name.empty()) {
				type->name = "int16";
			}
		}
		if (type->hash == (int)typeid(uint16_t).hash_code()) {
			isPrimitive = true;
			if (type->name.empty()) {
				type->name = "uint16";
			}
		}
		if (type->hash == (int)typeid(int32_t).hash_code()) {
			isPrimitive = true;
			if (type->name.empty()) {
				type->name = "int32";
			}
		}
		if (type->hash == (int)typeid(uint32_t).hash_code()) {
			isPrimitive = true;
			if (type->name.empty()) {
				type->name = "uint32";
			}
		}
		if (type->hash == (int)typeid(int64_t).hash_code()) {
			isPrimitive = true;
			if (type->name.empty()) {
				type->name = "int64";
			}
		}
		if (type->hash == (int)typeid(uint64_t).hash_code()) {
			isPrimitive = true;
			if (type->name.empty()) {
				type->name = "uint64";
			}
		}
		if (type->hash == (int)typeid(std::string).hash_code()) {
			if (type->name.empty()) {
				type->name = "string";
			}
		}

		if (isPrimitive) {
			(int&)type->flags |= (int)TypeDescriptor::Flags::PRIMITIVE;
			(int&)type->flags |= (int)TypeDescriptor::Flags::DATA;
		}

		int totalSize = 0;
		bool isData = true;
		for (auto& member : type->members) {
			totalSize += member->type->size;
			if (!((int)member->type->flags & (int)TypeDescriptor::Flags::DATA)) {
				isData = false;
			}
		}
		if (totalSize == type->size && isData) {
			(int&)type->flags |= (int)TypeDescriptor::Flags::DATA;
		}
	}

}
