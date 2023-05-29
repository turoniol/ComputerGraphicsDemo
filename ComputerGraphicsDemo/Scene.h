#pragma once

#include <memory>
#include <unordered_map>
#include <map>

#include "FundamentalTypes.h"
#include "Camera.h"
#include "MeshNode.h"

class MeshNode;
struct Renderer;

class Scene
{
    static const Mesh::Material DefaultHighlightMaterial;
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
    void HighlightNode(MeshNode* n, Mesh::Material m = DefaultHighlightMaterial);
    void SelectNode(MeshNode* node);
    MeshNode* GetSelectedNode();

    const MeshNode& Root() const;

    Light& GetLight();
    Camera& GetCamera();

    std::map<float, MeshNode*> FindIntersected(Ray r) const;

    bool renderBoundingBoxes = false;
private:
    Light m_light;
    Camera m_camera;
    std::unique_ptr<MeshNode> m_root;
    std::unordered_map<MeshNode*, Mesh::Material> m_highlightingMaterials;

    MeshNode* m_selectedNode = nullptr;
};