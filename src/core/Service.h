//
// Copyright (c) 2024 Julian Hinxlage. All rights reserved.
//

#pragma once

#include <functional>

class Service {
public:
	static std::function<void()> callback;
	static std::function<void()> onStop;
	static bool running;
	static bool start(const std::function<void()> &callback);
};