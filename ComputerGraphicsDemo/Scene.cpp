#include "Scene.h"

#include "Renderer.h"

#include <DirectXCollision.h>

const Mesh::Material Scene::DefaultHighlightMaterial{
        { 0.5372f, 0.7294f, 0.8745f, 1.0f },
        { 0.f, 0.f, 0.f, 0.f},
        { 0.f, 0.f, 0.f, 0.f},
        0.0f
};

Scene::Scene()
    : m_light{ {100, 100, 100}, {1, 1, 1, 1} },
    m_camera{}, m_root{ std::make_unique<MeshNode>() }
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

    auto renderMeshNode = [&](MeshNode* node) {
        auto finded = m_highlightingMaterials.find(node);
        auto& material = node->GetMesh()->material;

        Mesh::Material materialBackup = material;

        if (finded != m_highlightingMaterials.end()) {
            material = finded->second;
        }

        auto mesh = node->GetMesh();
        renderer.SetMatrices(node->AbsoluteMatrix(), lookAt, projMatrix);
        renderer.RenderMesh(*mesh);
        auto borderColor = m_selectedNode != node ? std::array<float, 4>{ 1.0f, 0.0f, 0.0f, 1.0f } : std::array<float, 4>{ 0.5372f, 0.7294f, 0.8745f, 1.0f };
        renderer.RenderBoundingBox(mesh->GetBoundingBox(), borderColor);

        material = materialBackup;
    };

    for (const auto& pChild : m_root->GetChildren())
        pChild->Traverse(renderMeshNode);

    m_highlightingMaterials.clear();
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

void Scene::SelectNode(MeshNode* node)
{
    m_selectedNode = node;
}

MeshNode* Scene::GetSelectedNode()
{
    return m_selectedNode;
}

void Scene::HighlightNode(MeshNode* n, Mesh::Material m)
{
    m_highlightingMaterials.insert({ n, m });
}

const MeshNode& Scene::Root() const
{
    return *m_root.get();
}

Scene::Light& Scene::GetLight()
{
    return m_light;
}

Camera& Scene::GetCamera()
{
    return m_camera;
}

std::map<float, MeshNode*> Scene::FindIntersected(Ray r) const
{
    std::map<float, MeshNode*> nodes;

    auto f = [&](MeshNode* node) {
        if (!DirectX::Internal::XMVector3IsUnit(r.dir))
            return;

        if (node->GetMesh() == nullptr)
            return;

        float dist{  };

        if (node->GetBoundingBox().Intersects(r.origin, r.dir, dist))
            nodes.insert({ dist, node });
    };

    m_root->Traverse(f);

    return nodes;
}
