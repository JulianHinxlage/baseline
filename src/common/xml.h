//
// Copyright (c) 2024 Julian Hinxlage. All rights reserved.
//

#pragma once

#include <string>
#include <vector>

namespace baseline {

	class XmlNode {
	public:
		std::string attributes;
		std::string content;
		int contentRootOffset = 0;

		XmlNode(const std::string& text = "");

		std::string value();

		std::string attribute(const std::string& name);

		std::string readIdentifier(const std::string& str, int offset);

		std::vector<XmlNode> childs(const std::string& name, bool justFirst = false);

		XmlNode child(const std::string& name);
	};

}
