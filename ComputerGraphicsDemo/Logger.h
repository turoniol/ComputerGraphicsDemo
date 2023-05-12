#pragma once

#include <iostream>

#define LOGERRRET(...) { Logger::LogErr(__VA_ARGS__); return; }


namespace Logger {
	template <class... Args>
	static void LogErr(Args&&... args) {
		(std::cerr << ... << args) << std::endl;
	}
}