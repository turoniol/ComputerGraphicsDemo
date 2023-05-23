#include "Scene.h"

#include "Renderer.h"

#include <deque>

Scene::Scene()
    : m_light{ {100, 100, 100}, {1, 1, 1, 1}},
    m_camera{}, m_root{std::make_unique<MeshNode>()}
{
}

void Scene::AddNode(std::unique_ptr<MeshNode> node)
{
    m_root->AddChild(std::move(node));
}

void Scene::RenderScene(Renderer& renderer, Matrix4x4 projMatrix)
{
    const auto lookAt = m_camera.LookAt();
    renderer.SetMatrices(DirectX::XMMatrixIdentity(), lookAt, projMatrix);
    renderer.SetLight(m_light.pos, m_light.color);

    auto renderMeshNode = [&](const MeshNode* node) {
        renderer.SetMatrices(node->AbsoluteMatrix(), lookAt, projMatrix);
        renderer.RenderMesh(*node->GetMesh());
    };

    for (const auto& pChild : m_root->GetChildren())
        pChild->Traverse(renderMeshNode);
}

void Scene::SetupLight(Point pos, Color color)
{
    m_light.color = color;
    m_light.pos = pos;
}

void Scene::SetupCamera(Vector eye, Vector target, Vector up)
{
    m_camera.SetEyeTagerUp(std::move(eye), std::move(target), std::move(up));
}

const MeshNode& Scene::Root() const
{
    return *m_root.get();
}
