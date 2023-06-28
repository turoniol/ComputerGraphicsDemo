#include "Loader.h"

#include "Readers/MOSDReader.h"

void Loader::SetWorkingDir(std::filesystem::path path)
{
    m_workingDir = path;
}

Scene& Loader::GetScene()
{
    return m_scene;
}

void Loader::LoadScene()
{
    m_reader->Read();
    m_scene = m_reader->GetScene();
}

void Loader::Update()
{
    if (index == m_prevIndex)
        return;

    UpdateFileList();
    LoadReader<MOSDReader>();
    LoadScene();

    m_prevIndex = index;
}

void Loader::UpdateFileList()
{
    m_fileList.clear();

    for (const auto& file : std::filesystem::directory_iterator{ m_workingDir }) {
        if (file.path().extension() == ".mosd")
            m_fileList.push_back(file.path().filename().string());
    }
}

const std::vector<std::string>& Loader::GetFilesList() const
{
    return m_fileList;
}

