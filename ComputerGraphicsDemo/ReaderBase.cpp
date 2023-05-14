#include "ReaderBase.h"

#include <fstream>
#include <filesystem>

#include "Logger.h"

void ReaderBase::Read(const std::string& filepath)
{
    workingDir = std::filesystem::path(filepath).parent_path().string();
    std::ifstream str(filepath);

    std::string line;
    while (std::getline(str, line)) {
        ParseLine(line);
    }

    endFunc();
}

void ReaderBase::ParseLine(const std::string& line) 
{
    std::string prefix;
    std::stringstream sstr(line);
    sstr >> prefix;

    if (prefix.empty())
        return;

    auto finded = m_actions.find(prefix);

    if (finded == m_actions.end())
        LOGERRRET("No such option: ", prefix);

    finded->second(sstr);
}