//
// Copyright (c) 2024 Julian Hinxlage. All rights reserved.
//

#include "Reflection.h"
#include "common/strutil.h"

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

	std::string toString(const TypeDescriptor* desc, void* value) {
		if (desc->isType<std::string>()) {
			return *(std::string*)value;
		}
		else if ((int)desc->flags & (int)TypeDescriptor::Flags::DATA) {
			if (desc->isType<float>()) {
				return toString(*(float*)value);
			}
			if (desc->isType<double>()) {
				return toString(*(double*)value);
			}
			if (desc->size == 8) {
				return toString(*(int64_t*)value);
			}
			else {
				return toString(*(int*)value);
			}
		}
		else {
			std::string str;
			for (auto& m : desc->members) {
				if (!str.empty()) {
					str += ",";
				}
				str += toString(m->type.get(), (uint8_t*)value + m->offset);
			}
			return str;
		}
	}

	void fromString(const TypeDescriptor* desc, void* dest, const std::string& src) {
		if (desc->isType<std::string>()) {
			*(std::string*)dest = src;
		}
		else if ((int)desc->flags & (int)TypeDescriptor::Flags::DATA) {
			if (desc->isType<float>()) {
				*(float*)dest = fromString<float>(src);
			}
			else if (desc->isType<double>()) {
				*(double*)dest = fromString<double>(src);
			}
			else if (desc->size == 8) {
				*(int64_t*)dest = fromString<int>(src);
			}
			else {
				*(int*)dest = fromString<int>(src);
			}
		}
	}

	std::vector<MemberDescriptor> flatMemberList(const TypeDescriptor* desc, const std::string& prefix, int offset) {
		std::vector<MemberDescriptor> list;
		for (auto& m : desc->members) {
			if (m->type->members.size() > 0) {
				auto childs = flatMemberList(m->type.get(), prefix + m->name + "_", offset + m->offset);
				for (auto& c : childs) {
					list.push_back(c);
				}
			}
			else {
				MemberDescriptor desc = *m;
				desc.name = prefix + desc.name;
				desc.offset += offset;
				list.push_back(desc);
			}
		}
		return list;
	}

}
