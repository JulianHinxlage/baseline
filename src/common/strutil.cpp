//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#include "strutil.h"
#include <fstream>

namespace baseline {

	std::string readFile(const std::string& file) {
		std::ifstream stream(file);
		if (stream.is_open()) {
			return std::string((std::istreambuf_iterator<char>(stream)), (std::istreambuf_iterator<char>()));
		}
		else {
			return "";
		}
	}

	void writeFile(const std::string& file, const std::string& text) {
		std::ofstream stream(file);
		stream << text;
		stream.close();
	}

	std::vector<std::string> split(const std::string& string, const std::string& delimiter, bool includeEmpty) {
		std::vector<std::string> parts;
		std::string token;
		int delimiterIndex = 0;
		for (char c : string) {
			if ((int)delimiter.size() == 0) {
				parts.push_back({ c, 1 });
			}
			else if (c == delimiter[delimiterIndex]) {
				delimiterIndex++;
				if (delimiterIndex == delimiter.size()) {
					if (includeEmpty || (int)token.size() != 0) {
						parts.push_back(token);
					}
					token.clear();
					delimiterIndex = 0;
				}
			}
			else {
				token += delimiter.substr(0, delimiterIndex);
				token.push_back(c);
				delimiterIndex = 0;
			}
		}
		token += delimiter.substr(0, delimiterIndex);
		if (includeEmpty || (int)token.size() != 0) {
			parts.push_back(token);
		}
		return parts;
	}

	std::vector<std::string> split(const std::vector<std::string>& strings, const std::string& delimiter, bool includeEmpty) {
		std::vector<std::string> parts;
		for (auto& s : strings) {
			for (auto& i : split(s, delimiter, includeEmpty)) {
				parts.push_back(i);
			}
		}
		return parts;
	}

	std::string join(const std::vector<std::string>& strings, const std::string& delimiter) {
		std::string result;
		for (int i = 0; i < strings.size(); i++) {
			result += strings[i];
			if (i != strings.size() - 1) {
				result += delimiter;
			}
		}
		return result;
	}

	std::string replace(const std::string& string, const std::string& search, const std::string& replacement) {
		return join(split(string, search), replacement);
	}

	int match(const std::string& string1, const std::string& string2) {
		int matchCount = 0;
		for (int i = 0; i < string1.size(); i++) {
			if (i < string2.size()) {
				if (string1[i] == string2[i]) {
					matchCount++;
				}
				else {
					break;
				}
			}
			else {
				break;
			}
		}
		return matchCount;
	}

	int toInt(const std::string& str, int defaultValue) {
		try {
			int value = std::stoi(str);
			return value;
		}
		catch (...) {
			return defaultValue;
		}
		return defaultValue;
	}

	float toFloat(std::string str, float defaultValue, bool allowCommaAsPoint) {
		if (allowCommaAsPoint) {
			str = replace(str, ",", ".");
		}
		try {
			float value = std::stof(str);
			return value;
		}
		catch (...) {
			return defaultValue;
		}
		return defaultValue;
	}

}
