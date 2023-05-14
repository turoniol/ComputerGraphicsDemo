#pragma once

#include <functional>
#include <sstream>
#include <string>
#include <map>

class ReaderBase
{
public:
	using ParseFunc = std::function<void(std::stringstream&)>;
#define ParsePair(name, f) m_actions.insert({ name, [=, this](std::stringstream& str) { f(str); } })
#define IgnorePair(name) m_actions.insert({ name, [](std::stringstream& str) { } })

	void Read(const std::string& filepath);

	std::function<void()> endFunc{};

	std::string workingDir{};
	std::map<std::string, ParseFunc> m_actions;

private:
	void ParseLine(const std::string& line);
};

