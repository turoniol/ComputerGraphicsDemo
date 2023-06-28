#include "ReaderBase.h"


#include "../Logger.h"
#include "../Scene.h"

ReaderBase::ReaderBase(const std::string& filepath) 
    : workingDir(std::filesystem::path(filepath).parent_path()),
    m_fileStr(filepath) 
{ }

void ReaderBase::Read() {
    std::string chunk{};
    while (GetChunk(chunk)) {
        ParseTokens(chunk);
    }

    endFunc();
}

Scene ReaderBase::GetScene() const
{
    return Scene();
}

bool ReaderBase::GetChunk(std::string& chunk) {
    return static_cast<bool>(m_fileStr >> chunk);
}

void ReaderBase::Ignore()
{
    std::string data{};
    std::getline(m_fileStr, data);
}

void ReaderBase::ParseTokens(const std::string& chunk) {
    std::string prefix;
    std::stringstream sstr(chunk);
    sstr >> prefix;

    if (prefix.empty())
        return;

    auto finded = actions.find(prefix);

    if (finded == actions.end()) {
        Ignore();
        LOGERRRET("No such option: ", prefix);
    }

    finded->second();
}