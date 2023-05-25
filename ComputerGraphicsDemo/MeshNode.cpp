#include "MeshNode.h"

MeshNode::MeshNode()
    : m_mesh{}, m_transform(DirectX::XMMatrixIdentity())
{
}

MeshNode::MeshNode(std::unique_ptr<Mesh> mesh)
    : m_mesh(std::move(mesh)), m_transform(DirectX::XMMatrixIdentity())
{
}

Matrix4x4 MeshNode::AbsoluteMatrix() const
{
    return m_parent ? DirectX::XMMatrixMultiply(m_transform, m_parent->AbsoluteMatrix()) : m_transform;
}

Matrix4x4 MeshNode::Matrix() const
{
    return m_transform;
}

void MeshNode::SetMatrix(Matrix4x4 m)
{
    m_transform = m;
}

void MeshNode::Transform(Matrix4x4 m)
{
    m_transform = DirectX::XMMatrixMultiply(m_transform, m);
}

const Mesh* MeshNode::GetMesh() const
{
    return m_mesh.get();
}

Mesh* MeshNode::GetMesh()
{
    return m_mesh.get();
}

void MeshNode::AddChild(std::unique_ptr<MeshNode> child)
{
    child->m_parent = this;
    m_children.push_back(std::move(child));
}

void MeshNode::SetParent(MeshNode* parent)
{
    m_parent = parent;
}

const MeshNode* MeshNode::GetParent() const
{
    return m_parent;
}

const MeshNode::ChildrenType& MeshNode::GetChildren() const
{
    return m_children;
}

void MeshNode::TraverseConst(std::function<void(const MeshNode*)> f) const
{
    f(this);

    for (const auto& c : m_children)
        c->Traverse(f);
}

void MeshNode::Traverse(std::function<void(MeshNode*)> f)
{
    f(this);

    for (auto& c : m_children)
        c->Traverse(f);
}

const std::string& MeshNode::GetName() const
{
    return m_name;
}

void MeshNode::SetName(const std::string& name)
{
    m_name = name;
}

DirectX::BoundingBox MeshNode::GetBoundingBox()
{
    if (!m_mesh)
        return {};

    DirectX::BoundingBox out;
    m_mesh->GetBoundingBox().Transform(out, AbsoluteMatrix());
    return out;
}
