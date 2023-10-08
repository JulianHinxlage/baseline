//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#include "Config.h"
#include "common/Log.h"
#include <filesystem>

namespace baseline {
	
	void Config::loadFile(const std::string& file) {
		if (!std::filesystem::exists(file)) {
			Log::warning("log file %s not found", file.c_str());
			return;
		}
		Log::info("loading config file %s", file.c_str());
		load(readFile(file));
	}

	void Config::load(const std::string& text) {
		auto lines = split(text, "\n");
		for (auto& line : lines) {
			for (int i = 0; i < line.size(); i++) {
				if (line[i] == '#') {
					line.resize(i);
					break;
				}
			}
			if (!line.empty()) {
				execute(line);
			}
		}
	}

	void Config::loadFirstFileFound(const std::vector<std::string>& files) {
		for (auto& file : files) {
			if (std::filesystem::exists(file)) {
				loadFile(file);
				return;
			}
		}
	}

	Config::Var* Config::getVar(const std::string& name) {
		for (auto& var : vars) {
			if (var) {
				if (var->name == name) {
					return var;
				}
			}
		}
		return nullptr;
	}

	void Config::addCommand(const std::string& name, const Callback& callback) {
		Command command;
		command.name = name;
		command.callback = callback;
		commands.push_back(command);
	}

	std::vector<std::string> Config::getCommandList() {
		std::vector<std::string> list;
		for (auto& command : commands) {
			list.push_back(command.name);
		}
		return list;
	}

	std::vector<std::string> Config::getVarList(){
		std::vector<std::string> list;
		for (auto& var : vars) {
			if (var) {
				list.push_back(var->name);
			}
		}
		return list;
	}

	void Config::execute(const std::string& string) {
		auto parts = split(string, " ");
		if (parts.size() > 0) {
			std::string name = parts[0];
			parts.erase(parts.begin());

			for (auto& command : commands) {
				if (command.name == name) {
					if (command.callback) {
						command.callback(parts);
						return;
					}
				}
			}
			for (auto& var : vars) {
				if (var) {
					if (var->name == name) {
						if (parts.size() > 0) {
							if (parts[0] == "=") {
								if (parts.size() > 1) {
									var->setString(parts[1]);
								}
							}
							else {
								var->setString(parts[0]);
							}
						}
						else {
							Log::info("%s = %s", name.c_str(), var->getString().c_str());
						}
						return;
					}
				}
			}

			if (parts.size() > 0) {
				if (parts[0] == "=") {
					if (parts.size() > 1) {
						addVar<std::string>(name, parts[1]);
						return;
					}
				}
			}

			Log::warning("unknown command: %s", string.c_str());
		}

	}

}
