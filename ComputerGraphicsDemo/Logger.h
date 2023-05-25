#pragma once

#include <iostream>

#define LOGERRRET(...) { Logger::LogErr(__VA_ARGS__); return; }
#define LOGXMFLOAT(msg, vec) { Logger::Log(msg, DirectX::XMVectorGetX(vec)); }
#define LOGXMFLOAT3(msg, vec) { Logger::Log(msg, DirectX::XMVectorGetX(vec), " ", DirectX::XMVectorGetY(vec), " ", DirectX::XMVectorGetZ(vec)); }



namespace Logger {
	template <class... Args>
	static void LogErr(Args&&... args) {
		(std::cerr << ... << args) << std::endl;
	}

	template <class... Args>
	static void Log(Args&&... args) {
		(std::cout << ... << args) << std::endl;
	}
}