#include "MeshNodePropertyWindow.h"

#include "../MeshNode.h"

#include "imgui.h"

#include <DirectXMath.h>

#include "../Logger.h"

void MeshNodePropertyWindow::SetMeshNode(MeshNode* handler)
{
    m_nodeHandler = handler;
}

void MeshNodePropertyWindow::Render()
{
    if (!m_nodeHandler)
        return;

    ImGui::Begin("Node Properties");
    NodeInfo();
    ImGui::Separator();
    ParentInfo();
    ImGui::Separator();
    MaterialInfo();
    ImGui::Separator();
    BoundingBoxInfo();
    ImGui::Separator();
    TransformationInfo();
    ImGui::End();
}

void MeshNodePropertyWindow::MaterialInfo()
{
    auto mesh = m_nodeHandler->GetMesh();

    auto& diffuse = mesh->material.diffuse;
    auto& ambient = mesh->material.ambient;
    auto& specular = mesh->material.specular;
    auto& shininess = mesh->material.shininess;

    {
        float vec[]{ EXPAND3f(ambient) };
        if (ImGui::ColorEdit3("ambient", vec)) 
            ambient = DirectX::XMVectorSet(vec[0], vec[1], vec[2], 1.0f);
    }
    {
        float vec[]{ EXPAND3f(diffuse) };
        if (ImGui::ColorEdit3("diffuse", vec))
            diffuse = DirectX::XMVectorSet(vec[0], vec[1], vec[2], 1.0f);
    }
    {
        float vec[]{ EXPAND3f(specular) };
        if (ImGui::ColorEdit3("specular", vec))
            specular = DirectX::XMVectorSet(vec[0], vec[1], vec[2], 1.0f);
    }
    {
        ImGui::DragFloat("shininess", &shininess, 0.1f, 1.0f, 256.0f);
    }
}

void MeshNodePropertyWindow::NodeInfo()
{
    ImGui::Text("Name: %s", m_nodeHandler->GetName().data());
}

void MeshNodePropertyWindow::ParentInfo()
{
    auto parent = m_nodeHandler->GetParent();
    std::string parentName = parent ? parent->GetName() : std::string("No parent");
    ImGui::Text("Parent: %s", parentName.data());
}

void MeshNodePropertyWindow::BoundingBoxInfo()
{
    const auto& bb = m_nodeHandler->GetBoundingBox();
    float bbCenter[3]{ bb.Center.x, bb.Center.y, bb.Center.z };
    ImGui::Text("Bounding box center");
    ImGui::InputFloat3("", bbCenter, "%.3f");
    ImGui::Text("Bounding box extent");
    float bbExtent[3]{ bb.Extents.x, bb.Extents.y, bb.Extents.z };
    ImGui::InputFloat3("", bbExtent, "%.3f");
}

void MeshNodePropertyWindow::TransformationInfo()
{
    auto matrix = m_nodeHandler->Matrix();

    DirectX::XMVECTOR scale, rotation, translation;
    DirectX::XMMatrixDecompose(&scale, &rotation, &translation, matrix);

    bool isChanged = false;
    
    {
        ImGui::Text("Translation");
        float tVec[3]{ EXPAND3f(translation) };

        if (ImGui::DragFloat3("local (t)", tVec, 0.1f, -50.f, 50.f)) {
            translation = DirectX::XMVectorSet(tVec[0], tVec[1], tVec[2], 1.0f);
            isChanged = true;
        }
    }
    {
        ImGui::Text("Rotation");
        DirectX::XMVECTOR axis;
        float angle;
        DirectX::XMQuaternionToAxisAngle(&axis, &angle, rotation);
        float rVec[4]{ EXPAND3f(axis), angle };
        ImGui::InputFloat4("local (r)", rVec, "%.3f", ImGuiInputTextFlags_ReadOnly);
    }
    {
        ImGui::Text("Scale");
        float sVec[3]{ EXPAND3f(scale) };
        if (ImGui::DragFloat3("local (s)", sVec, 0.1f, 0.5f, 5.0f)) {
            scale = DirectX::XMVectorSet(sVec[0], sVec[1], sVec[2], 1.0f);
            isChanged = true;
        }
    }
    {
        ImGui::Text("Matrix");

        for (std::size_t i = 0; i < 4; ++i) {
            float row[4]{ EXPAND4f(matrix.r[i]) };
            ImGui::InputFloat4(std::to_string(i).data(), row, "%.3f", ImGuiInputTextFlags_ReadOnly);
        }
    }

    if (isChanged) {
        const auto origin = DirectX::XMVectorSet(0, 0, 0, 1);
        m_nodeHandler->SetMatrix(DirectX::XMMatrixAffineTransformation(scale, origin, rotation, translation));
    }
}
