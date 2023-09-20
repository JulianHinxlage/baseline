//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#pragma once

#include <string>
#include <vector>

namespace baseline {

	std::string readFile(const std::string& file);

	void writeFile(const std::string& file, const std::string& text);

	std::vector<std::string> split(const std::string& string, const std::string& delimiter, bool includeEmpty = false);

	std::vector<std::string> split(const std::vector<std::string>& strings, const std::string& delimiter, bool includeEmpty = false);

	std::string join(const std::vector<std::string>& strings, const std::string& delimiter);

	std::string replace(const std::string& string, const std::string& search, const std::string& replacement);

	int toInt(const std::string& str, int defaultValue = -1);

	float toFlot(std::string str, float defaultValue = 0, bool allowCommaAsPoint = false);

	template<typename T>
	static T fromString(const std::string& string, T defaultValue = T()) {
		if (std::is_same<T, int>()) {
			try {
				return std::stoi(string);
			}
			catch (...) {}
		}
		else if (std::is_same<T, float>()) {
			try {
				return std::stof(string);
			}
			catch (...) {}
		}
		else if (std::is_same<T, double>()) {
			try {
				return std::stod(string);
			}
			catch (...) {}
		}
		else if (std::is_same<T, bool>()) {
			try {
				if (string == "true") {
					return true;
				}else if(string == "false") {
					return false;
				}
				else {
					return std::stoi(string);
				}
			}
			catch (...) {}
		}
		return defaultValue;
	}

	template<>
	static std::string fromString<std::string>(const std::string& string, std::string defaultValue) {
		return string;
	}

	template<typename T>
	static std::string toString(T value) {
		return std::to_string(value);
	}

	template<>
	static std::string toString<std::string>(std::string value) {
		return value;
	}

}
