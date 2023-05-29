#pragma once

#include "PropertryWindow.h"

class MeshNode;
class Loader;

class ScenePropertyWindow : public PropertyWindow
{
public:
    void SetLoader(Loader* loader);
    void Render() override;

private:
    void RenderMeshNode(MeshNode* node);

    Loader* m_loader = nullptr;
};

