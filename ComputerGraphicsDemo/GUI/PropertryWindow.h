#pragma once

#include <string>

class PropertyWindow
{
public:
    virtual ~PropertyWindow() = default;

    virtual void Render() = 0;
    //const std::string& GetTitle() const;
    //void SetTitle(const std::string& title);

protected:
    std::string m_title;
};

