#include "ScenePropertyWindow.h"

#include "../Scene.h"
#include "imgui.h"

void ScenePropertyWindow::SetScene(Scene* scene)
{
    m_sceneHandler = scene;
}

void ScenePropertyWindow::RenderMeshNode(MeshNode* node) {
    auto& children = node->GetChildren();

    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

    if (children.empty()) {
        nodeFlags |= ImGuiTreeNodeFlags_Leaf;
    }

    if (m_sceneHandler->GetSelectedNode() == node)
        nodeFlags |= ImGuiTreeNodeFlags_Selected;

    const bool isOpen = ImGui::TreeNodeEx(node->GetName().data(), nodeFlags);

    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        m_sceneHandler->SelectNode(ImGui::GetIO().KeyCtrl ? nullptr : node);

    if (ImGui::IsItemHovered())
        m_sceneHandler->HighlightNode(node);

    if (isOpen) {
        for (const auto& child : children)
           RenderMeshNode(child.get());

        ImGui::TreePop();
    }
}

void ScenePropertyWindow::Render()
{
    ImGui::Begin("Scene Property");

    ImGui::SeparatorText("Light");
    {
        auto& light = m_sceneHandler->GetLight();

        ImGui::DragFloat3("light position", light.pos.m128_f32, 0.05f, -200.f, 200.f);
        ImGui::ColorEdit3("light color", light.color.m128_f32);
    }
    ImGui::SeparatorText("Camera");
    {
        auto& camera = m_sceneHandler->GetCamera();

        ImGui::DragFloat3("camera position", camera.m_eye.m128_f32, 0.05f, -100.f, 100.f);
        ImGui::DragFloat3("camera target", camera.m_target.m128_f32, 0.05f, -100.f, 100.f);
    }
    ImGui::SeparatorText("Nodes");
    {
        auto& root = m_sceneHandler->Root();

        for (auto& child : root.GetChildren())
            RenderMeshNode(child.get());
    }

    ImGui::End();
}
