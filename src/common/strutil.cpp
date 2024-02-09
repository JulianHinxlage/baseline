//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#include "strutil.h"
#include <fstream>

namespace baseline {

	std::string readFile(const std::string& file, bool binary) {
		FILE *f = fopen(file.c_str(), binary ? "rb" : "r");
		if (f) {
			fseek(f, 0, SEEK_END);
			int size = ftell(f);
			fseek(f, 0, SEEK_SET);
			std::string text;
			text.resize(size);
			int bytes = fread(text.data(), 1, text.size(), f);
			text.resize(bytes);
			fclose(f);
			return text;
		}
		else {
			return "";
		}
	}

	bool writeFile(const std::string& file, const std::string& text, bool binary) {
		FILE* f = fopen(file.c_str(), binary ? "wb" : "w");
		if (f) {
			int res = fwrite(text.data(), 1, text.size(), f);
			if (res != text.size()) {
				fclose(f);
				return false;
			}
			fclose(f);
			return true;
		}
		else {
			return false;
		}
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
		return join(split(string, search, true), replacement);
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

	void trimFront(std::string& string, const std::string& prefix, bool trimAllIfNotFound) {
		if (prefix.empty()) {
			return;
		}

		int matchIndex = 0;
		int trimIndex = 0;
		for (int i = 0; i < string.size(); i++) {
			char c = string[i];
			if (c == prefix[matchIndex]) {
				matchIndex++;
				if (matchIndex == prefix.size()) {
					matchIndex = 0;
					trimIndex = i + 1;
					break;
				}
			}
			else {
				matchIndex = 0;
			}
		}

		if (trimIndex != 0) {
			string = string.substr(trimIndex);
		}
		else if (trimAllIfNotFound) {
			string = "";
		}
	}

	void trimBack(std::string& string, const std::string& suffix) {
		if (suffix.empty()) {
			return;
		}

		int matchIndex = 0;
		int trimIndex = 0;
		for (int i = 0; i < string.size(); i++) {
			char c = string[i];
			if (c == suffix[matchIndex]) {
				matchIndex++;
				if (matchIndex == suffix.size()) {
					matchIndex = 0;
					trimIndex = i - suffix.size() + 1;
					break;
				}
			}
			else {
				matchIndex = 0;
			}
		}

		if (trimIndex != 0) {
			string = string.substr(0, trimIndex);
		}
	}

	std::string subString(const std::string& string, const std::string& prefix, const std::string& suffix) {
		std::string str = string;
		trimFront(str, prefix, true);
		trimBack(str, suffix);
		return str;
	}

	std::string leftPadd(const std::string& string, char character, int targetLength) {
		std::string str = string;
		while (str.size() < targetLength) {
			str.insert(str.begin(), character);
		}
		return str;
	}

	bool stringContains(const std::string& string, const std::string& subString) {
		int matchIndex = 0;
		for (int i = 0; i < string.size(); i++) {
			char c = string[i];
			if (c == subString[matchIndex]) {
				matchIndex++;
				if (matchIndex == subString.size()) {
					return true;
				}
			}
			else {
				matchIndex = 0;
			}
		}
		return false;
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

	uint8_t hexCharToInt(char hex) {
		if (hex >= '0' && hex <= '9') {
			return hex - '0';
		}
		else if (hex >= 'a' && hex <= 'f') {
			return hex - 'a' + 10;
		}
		else if (hex >= 'A' && hex <= 'F') {
			return hex - 'A' + 10;
		}
		return 0;
	}

	uint64_t hexToInt(const std::string& hex) {
		int result = 0;
		for (char c : hex) {
			result <<= 4;
			result |= hexCharToInt(c);
		}
		return result;
	}

	char intToHexChar(uint8_t value) {
		return "0123456789ABCDEF"[value & 0x0f];
	}

	std::string intToHex(uint64_t value) {
		std::string result;
		while (value > 0) {
			result = std::string(1, intToHexChar(value % 16)) + result;
			value >>= 4;
		}
		return result;
	}

	bool isNumber(const std::string& str) {
		for (char c : str) {
			if (c < '0' && c > '9') {
				if (c != '.') {
					return false;
				}
			}
		}
		return true;
	}

}
