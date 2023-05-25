#pragma once

#include "PropertryWindow.h"

class Scene;
class MeshNode;

class ScenePropertyWindow : public PropertyWindow
{
public:
    void SetScene(Scene* scene);
    void Render() override;

private:
    void RenderMeshNode(MeshNode* node);

    Scene* m_sceneHandler = nullptr;
};

