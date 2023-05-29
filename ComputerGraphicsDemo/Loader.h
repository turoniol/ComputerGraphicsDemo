#pragma once

#include "Readers/ReaderBase.h"
#include "Scene.h"

#include <filesystem>

class Loader {
public:
    void SetWorkingDir(std::filesystem::path path);

    Scene& GetScene();

    void LoadScene();

    void Update();
    void UpdateFileList();

    template <std::derived_from<ReaderBase> ReaderT> 
    void LoadReader() {
        if (index > m_fileList.size())
            return;

        auto filePath = std::filesystem::path(m_workingDir).append(m_fileList.at(index));
        m_reader = std::make_unique<ReaderT>(filePath.string());
    }

    const std::vector<std::string>& GetFilesList() const;

    int index = 0;

private:
    int m_prevIndex = -1;

    std::vector<std::string> m_fileList;

    std::unique_ptr<ReaderBase> m_reader;
    Scene m_scene;

    std::filesystem::path m_workingDir;
};