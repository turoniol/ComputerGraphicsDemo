#pragma once

#include <string>
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
    Matrix4x4 Matrix() const;
    void SetMatrix(Matrix4x4 m);
    void Transform(Matrix4x4 m);
    const Mesh* GetMesh() const;
    Mesh* GetMesh();
    void AddChild(std::unique_ptr<MeshNode> child);
    void SetParent(MeshNode* parent);
    const MeshNode* GetParent() const;
    const ChildrenType& GetChildren() const;
    void TraverseConst(std::function<void(const MeshNode*)> f) const;
    void Traverse(std::function<void(MeshNode*)> f);
    const std::string& GetName() const;
    void SetName(const std::string& name);

    DirectX::BoundingBox GetBoundingBox();
private:
    std::string m_name;
    Matrix4x4 m_transform;
    MeshNode* m_parent = nullptr;
    std::unique_ptr<Mesh> m_mesh;
    std::vector<std::unique_ptr<MeshNode>> m_children;
};

