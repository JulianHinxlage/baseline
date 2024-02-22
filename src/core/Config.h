//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#pragma once

#include "common/strutil.h"
#include <string>
#include <vector>
#include <functional>

namespace baseline {

	class Config {
	public:
		typedef std::function<void(const std::vector<std::string>&)> Callback;

		void loadFile(const std::string& file);
		void load(const std::string& text);
		void loadFirstFileFound(const std::vector<std::string>& files);
		std::string getConfigFilename();
		std::string getFilename(const std::string& varname, const std::string& defaultValue = "");

		class Var {
		public:
			std::string name;

			virtual void setString(const std::string& string) = 0;
			virtual std::string getString() = 0;

			template<typename T>
			void set(T value) {
				setString(toString(value));
			}

			template<typename T>
			T get() {
				return fromString<T>(getString());
			}
		};

		Var* getVar(const std::string& name);

		template<typename T>
		T getValue(const std::string& name, T defaultValue = T()) {
			if (auto* var = getVar(name)) {
				return var->get<T>();
			}
			else {
				return defaultValue;
			}
		}

		template<typename T>
		void setValue(const std::string& name, T value) {
			if (auto* var = getVar(name)) {
				var->set<T>(value);
			}
			else {
				VarT<T>* newVar = new VarT<T>();
				newVar->data = value;
				newVar->name = name;
				vars.push_back(newVar);
			}
		}

		template<typename T>
		void addVar(const std::string& name, const T& value) {
			VarT<T>* newVar = new VarT<T>();
			newVar->data = value;
			newVar->name = name;
			vars.push_back(newVar);
		}
		template<typename T>
		void addVar(const std::string& name, T* value) {
			VarT<T>* newVar = new VarT<T>();
			newVar->value = value;
			newVar->name = name;
			vars.push_back(newVar);
		}

		void addCommand(const std::string& name, const Callback& callback);
		void execute(const std::string& string, bool canCreateVariables = false);

		std::vector<std::string> getCommandList();
		std::vector<std::string> getVarList();

	private:

		class Command {
		public:
			std::string name;
			Callback callback;
		};
		template<typename T>
		class VarT : public Var {
		public:
			T *value;
			T data;

			VarT() {
				value = &data;
			}

			virtual void setString(const std::string& string) {
				*value = fromString<T>(string);
			}

			virtual std::string getString() {
				return toString(*value);
			}
		};

		std::vector<Var*> vars;
		std::vector<Command> commands;
		std::string loadedFile;
		std::string prevLoadedFile;
	};

}

