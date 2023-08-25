//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#pragma once

#include <string>

namespace baseline {

	enum class LogLevel {
		TRACE,
		DEBUG,
		INFO,
		WARNING,
		ERROR,
		FATAL,
	};

	class Log {
	public:
		static void log(LogLevel level, const char* fmt, va_list args);
		static void log(LogLevel level, const char* fmt, ...);
		static void trace(const char *fmt, ...);
		static void debug(const char *fmt, ...);
		static void info(const char *fmt, ...);
		static void warning(const char* fmt, ...);
		static void error(const char *fmt, ...);
		static void fatal(const char *fmt, ...);
	};

}
