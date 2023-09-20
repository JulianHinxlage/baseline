//
// Copyright (c) 2023 Julian Hinxlage. All rights reserved.
//

#include "Log.h"
#include "DateTime.h"
#include <cstdarg>

namespace baseline {

	const char* logLevelName(LogLevel level) {
		switch (level)
		{
		case baseline::LogLevel::TRACE:
			return "TRACE";
		case baseline::LogLevel::DEBUG:
			return "DEBUG";
		case baseline::LogLevel::INFO:
			return "INFO";
		case baseline::LogLevel::WARNING:
			return "WARNING";
		case baseline::LogLevel::ERROR:
			return "ERROR";
		case baseline::LogLevel::FATAL:
			return "FATAL";
		default:
			return "DEFAULT";
		}
	}

	void Log::log(LogLevel level, const char* fmt, va_list args) {
		DateTime time;
		time.now();
		printf("[%s] ", time.toStringTime().c_str());
		printf("[%s] ", logLevelName(level));
		vprintf(fmt, args);
		printf("\n");
	}

	void Log::log(LogLevel level, const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		log(level, fmt, args);
		va_end(args);
	}

	void Log::trace(const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		log(LogLevel::TRACE, fmt, args);
		va_end(args);
	}
	
	void Log::debug(const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		log(LogLevel::DEBUG, fmt, args);
		va_end(args);
	}
	
	void Log::info(const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		log(LogLevel::INFO, fmt, args);
		va_end(args);
	}
	
	void Log::warning(const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		log(LogLevel::WARNING, fmt, args);
		va_end(args);
	}
	
	void Log::error(const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		log(LogLevel::ERROR, fmt, args);
		va_end(args);
	}
	
	void Log::fatal(const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		log(LogLevel::FATAL, fmt, args);
		va_end(args);
	}

}
