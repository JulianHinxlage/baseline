//
// Copyright (c) 2024 Julian Hinxlage. All rights reserved.
//

#pragma once

#include <vector>
#include <string>
#include <memory>
#include <map>
#include <functional>

namespace baseline {

	class TypeDescriptor;

	class MemberDescriptor {
	public:
		std::string name;
		int offset = 0;
		std::shared_ptr<TypeDescriptor> type = nullptr;
	};

	class TypeDescriptor {
	public:
		enum class Flags {
			NONE,
			PRIMITIVE = 1 << 0,
			DATA = 1 << 1,
			POINTER = 1 << 2,
			VECTOR = 1 << 3,
			MAP = 1 << 4,
		};

		std::string name;
		int size = 0;
		int hash = 0;
		int index = 0;
		Flags flags = Flags::NONE;
		std::vector<std::shared_ptr<MemberDescriptor>> members;
		std::shared_ptr<TypeDescriptor> valueType = nullptr;
		std::shared_ptr<TypeDescriptor> keyType = nullptr;

		template<typename Type>
		bool isType() const {
			int hashT = (int)typeid(Type).hash_code();
			return hashT == hash;
		}
	};

	class Reflection {
	public:
		template<typename Type>
		static void registerType(const std::string& name) {
			auto type = getTypeImpl<Type>();
			type->name = name;
			getTypesByNameImpl()[name] = type;
		}

		template<typename Type, typename Member>
		static void registerMember(const std::string& name, int offset) {
			auto type = getTypeImpl<Type>();

			std::shared_ptr<MemberDescriptor> member;
			for (auto& m : type->members) {
				if (m->name == name) {
					member = m;
				}
			}
			if (!member) {
				member = std::make_shared<MemberDescriptor>();
				member->name = name;
				type->members.push_back(member);
			}

			member->offset = offset;
			member->type = getTypeImpl<Member>();
			initType<Type>(type);
		}

		template<typename Type>
		static const TypeDescriptor* getType() {
			static TypeDescriptor* type = getTypeImpl<Type>().get();
			return type;
		}

		static const TypeDescriptor* getType(const std::string& name) {
			auto& typesByName = getTypesByNameImpl();
			auto i = typesByName.find(name);
			if (i == typesByName.end()) {
				return nullptr;
			}
			return i->second.get();
		}

		static const std::vector<std::shared_ptr<TypeDescriptor>>& getTypes() {
			return getTypesImpl();
		}

	private:
		static std::vector<std::shared_ptr<TypeDescriptor>>& getTypesImpl();
		static std::map<std::string, std::shared_ptr<TypeDescriptor>>& getTypesByNameImpl();
		static void initTypeImpl(std::shared_ptr<TypeDescriptor> type);

		template<class T, class U, class> struct has_equal_impl : std::false_type {};
		template<class T, class U> struct has_equal_impl<T, U, decltype((bool)(std::declval<T>() == std::declval<U>()), void())> : std::true_type {};
		template<class T, class U> struct has_equal : has_equal_impl<T, U, void> {};
		template <typename T> struct is_vector : std::false_type { };
		template <typename T, typename Alloc> struct is_vector<std::vector<T, Alloc>> : std::true_type {};
		template <typename T> struct is_map : std::false_type { };
		template <typename T, typename V, typename Comp, typename Alloc> struct is_map<std::map<T, V, Comp, Alloc>> : std::true_type {};
		template <typename T, typename V, typename Comp, typename Alloc> struct is_map<std::unordered_map<T, V, Comp, Alloc>> : std::true_type {};

		template<typename Type>
		static void initType(std::shared_ptr<TypeDescriptor> type) {
			if constexpr (std::is_pointer_v<Type>) {
				(int&)type->flags |= (int)TypeDescriptor::Flags::POINTER;
				type->valueType = getTypeImpl<std::remove_pointer<Type>::type>();
				if (type->name.empty()) {
					type->name = type->valueType->name + "*";
				}
			}
			if constexpr (is_vector<Type>::value) {
				(int&)type->flags |= (int)TypeDescriptor::Flags::VECTOR;
				type->valueType = getTypeImpl<Type::value_type>();
				if (type->name.empty()) {
					type->name = "vector<" + type->valueType->name + ">";
				}
			}
			if constexpr (is_map<Type>::value) {
				(int&)type->flags |= (int)TypeDescriptor::Flags::MAP;
				type->valueType = getTypeImpl<decltype(Type::value_type::second)>();
				type->keyType = getTypeImpl<std::remove_const<decltype(Type::value_type::first)>::type>();
				if (type->name.empty()) {
					type->name = "map<" + type->keyType->name + "," + type->valueType->name + ">";
				}
			}
			initTypeImpl(type);
		}

		template<typename Type>
		static std::shared_ptr<TypeDescriptor> getTypeImpl() {
			auto& types = getTypesImpl();
			int hash = (int)typeid(Type).hash_code();

			for (auto& type : types) {
				if (type && type->hash == hash) {
					return type;
				}
			}

			std::shared_ptr<TypeDescriptor> type = std::make_shared<TypeDescriptor>();
			type->hash = hash;
			type->size = sizeof(Type);
			type->index = types.size();
			types.push_back(type);

			initType<Type>(type);

			if (!type->name.empty()) {
				getTypesByNameImpl()[type->name] = type;
			}
			return type;
		}
	};

}

namespace impl {

	class GlobalInitializationCallback {
	public:
		std::function<void()> onUninitialization;

		GlobalInitializationCallback(const std::function<void()>& onInitialization = nullptr, const std::function<void()>& onUninitialization = nullptr) {
			this->onUninitialization = onUninitialization;
			if (onInitialization) {
				onInitialization();
			}
		}

		~GlobalInitializationCallback() {
			if (onUninitialization) {
				onUninitialization();
			}
		}
	};

}

#define REG_CONCAT_IMPL(a, b) a##b
#define REG_CONCAT(a, b) REG_CONCAT_IMPL(a, b)
#define REG_UNIQUE_IDENTIFIER_IMPL_2(base, counter, line) base##_##line##_##counter
#define REG_UNIQUE_IDENTIFIER_IMPL(base, counter, line) REG_UNIQUE_IDENTIFIER_IMPL_2(base, counter, line)
#define REG_UNIQUE_IDENTIFIER(base) REG_UNIQUE_IDENTIFIER_IMPL(base, __COUNTER__, __LINE__)

#define REG_TYPE(type) static impl::GlobalInitializationCallback REG_UNIQUE_IDENTIFIER(init)([](){ Reflection::registerType<type>(#type); });
#define REG_MEMBER(type, name) static impl::GlobalInitializationCallback REG_UNIQUE_IDENTIFIER(init)([](){ Reflection::registerMember<type, decltype(type::name)>(#name, offsetof(type, name)); });

#define REG_TYPE_1(type, member1) REG_TYPE(type) REG_MEMBER(type, member1)
#define REG_TYPE_2(type, member1, member2) REG_TYPE_1(type, member1) REG_MEMBER(type, member2)
#define REG_TYPE_3(type, member1, member2, member3) REG_TYPE_2(type, member1, member2) REG_MEMBER(type, member3)
#define REG_TYPE_4(type, member1, member2, member3, member4) REG_TYPE_3(type, member1, member2, member3) REG_MEMBER(type, member4)
#define REG_TYPE_5(type, member1, member2, member3, member4, member5) REG_TYPE_4(type, member1, member2, member3, member4) REG_MEMBER(type, member5)
#define REG_TYPE_6(type, member1, member2, member3, member4, member5, member6) REG_TYPE_5(type, member1, member2, member3, member4, member5) REG_MEMBER(type, member6)
#define REG_TYPE_7(type, member1, member2, member3, member4, member5, member6, member7) REG_TYPE_6(type, member1, member2, member3, member4, member5, member6) REG_MEMBER(type, member7)
#define REG_TYPE_8(type, member1, member2, member3, member4, member5, member6, member7, member8) REG_TYPE_7(type, member1, member2, member3, member4, member5, member6, member7) REG_MEMBER(type, member8)
