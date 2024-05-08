//
// Copyright (c) 2024 Julian Hinxlage. All rights reserved.
//

#include "xml.h"
#include "strutil.h"
#include <string>
#include <vector>

namespace baseline {

	XmlNode::XmlNode(const std::string& text) {
		this->content = text;
	}

	std::string XmlNode::value() {
		return content;
	}

	std::string XmlNode::attribute(const std::string& name) {
		auto parts = split(attributes, name, true);
		if (parts.size() > 1) {
			std::string str = parts[1];
			std::string result = "";
			bool in = false;
			for (char c : str) {
				if (c == '\"') {
					if (in) {
						break;
					}
					in = !in;
				}
				else if (in) {
					result.push_back(c);
				}
			}
			return result;
		}
		return "";
	}

	static bool isCharackter(char c) {
		return (c >= 'a' && c <= 'z') ||
			(c >= 'A' && c <= 'Z') ||
			(c >= '0' && c <= '9');
	}

	std::string XmlNode::readIdentifier(const std::string& str, int offset) {
		std::string identifier = "";
		for (int i = offset; i < content.size(); i++) {
			char c = content[i];
			if (isCharackter(c)) {
				identifier.push_back(c);
			}
			else {
				break;
			}
		}
		return identifier;
	}

	std::vector<XmlNode> XmlNode::childs(const std::string& name, bool justFirst) {
		bool inTag = false;
		bool inNode = false;
		bool inQuotes = false;
		int depth = 0;
		int beginTagIndex = 0;
		int beginOpenIndex = 0;
		int beginCloseIndex = 0;

		std::vector<XmlNode> nodes;
		XmlNode node;

		for (int i = 0; i < content.size(); i++) {
			char c = content[i];

			if (!inNode) {
				if (c == '<') {
					beginOpenIndex = i;
					std::string ident = readIdentifier(content, i + 1);
					if (ident == name) {
						i += ident.size();
						inTag = true;
						inNode = true;
						depth = 1;
						beginTagIndex = i + 1;
					}
				}
			}
			else {
				if (c == '<' && !inQuotes) {
					beginOpenIndex = i;
					depth++;
				}
				if (c == '>' && !inQuotes) {
					if (inTag) {
						node.attributes = content.substr(beginTagIndex, i - beginTagIndex);
						beginCloseIndex = i;
					}
					inTag = false;
					depth--;
				}
				if (c == '\"') {
					inQuotes = !inQuotes;
				}
				if (c == '/' && !inQuotes) {
					if (inTag) {
						node.attributes = content.substr(beginTagIndex, i - beginTagIndex);
						node.contentRootOffset = i + contentRootOffset;
						nodes.push_back(node);
						node = XmlNode();
						inNode = false;
						inTag = false;
						depth = 0;
						beginTagIndex = 0;
						beginOpenIndex = 0;
						beginCloseIndex = 0;
						if (justFirst) {
							break;
						}
					}
					else {
						if (depth == 1) {
							std::string ident = readIdentifier(content, i + 1);
							if (ident == name) {
								node.content = content.substr(beginCloseIndex + 1, beginOpenIndex - (beginCloseIndex + 1));
								node.contentRootOffset = beginCloseIndex + 1 + contentRootOffset;
								nodes.push_back(node);
								node = XmlNode();
								inNode = false;
								inTag = false;
								depth = 0;
								beginTagIndex = 0;
								beginOpenIndex = 0;
								beginCloseIndex = 0;
								if (justFirst) {
									break;
								}
							}
						}
					}
				}
			}

		}
		return nodes;
	}

	XmlNode XmlNode::child(const std::string& name) {
		auto nodes = childs(name, true);
		if (nodes.size() > 0) {
			return nodes[0];
		}
		else {
			return XmlNode();
		}
	}

}
