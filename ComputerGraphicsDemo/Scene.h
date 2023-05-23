#pragma once

#include <memory>

#include "FundamentalTypes.h"
#include "Camera.h"
#include "MeshNode.h"

class MeshNode;
struct Renderer;

class Scene
{
public:
    struct Light {
        Point pos;
        Color color;
    };

    Scene();

    void AddNode(std::unique_ptr<MeshNode> node);
    void RenderScene(Renderer& renderer, Matrix4x4 projMatrix);
    void SetupLight(Point pos, Color color);
    void SetupCamera(Vector eye, Vector target, Vector up);

    const MeshNode& Root() const;

private:
    Light m_light;
    Camera m_camera;
    std::unique_ptr<MeshNode> m_root;
};