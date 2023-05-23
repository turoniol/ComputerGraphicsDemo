#pragma once

#include <memory>
#include <vector>
#include <functional>

#include "FundamentalTypes.h"
#include "Mesh.h"

class MeshNode
{
public:
    using ChildType = std::unique_ptr<MeshNode>;
    using ChildrenType = std::vector<ChildType>;

    MeshNode();
    MeshNode(std::unique_ptr<Mesh> mesh);
    Matrix4x4 AbsoluteMatrix() const;
    void Transform(Matrix4x4 m);
    const Mesh* GetMesh() const;
    void AddChild(std::unique_ptr<MeshNode> child);
    void SetParent(MeshNode* parent);
    const ChildrenType& GetChildren() const;
    void Traverse(std::function<void(const MeshNode*)> f) const;

private:
    Matrix4x4 m_transform;
    MeshNode* m_parent = nullptr;
    std::unique_ptr<Mesh> m_mesh;
    std::vector<std::unique_ptr<MeshNode>> m_children;
};

