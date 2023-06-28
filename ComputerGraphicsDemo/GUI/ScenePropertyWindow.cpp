#include "ScenePropertyWindow.h"

#include "imgui.h"
#include "../Loader.h"

void ScenePropertyWindow::SetLoader(Loader* loader)
{
    m_loader = loader;
}

void ScenePropertyWindow::RenderMeshNode(MeshNode* node) {
    auto& scene = m_loader->GetScene();
    auto& children = node->GetChildren();

    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

    if (children.empty()) {
        nodeFlags |= ImGuiTreeNodeFlags_Leaf;
    }

    if (scene.GetSelectedNode() == node)
        nodeFlags |= ImGuiTreeNodeFlags_Selected;

    const bool isOpen = ImGui::TreeNodeEx(node->GetName().data(), nodeFlags);

    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        scene.SelectNode(ImGui::GetIO().KeyCtrl ? nullptr : node);

    if (ImGui::IsItemHovered())
        scene.HighlightNode(node);

    if (isOpen) {
        for (const auto& child : children)
           RenderMeshNode(child.get());

        ImGui::TreePop();
    }
}

void ScenePropertyWindow::Render()
{
    auto& scene = m_loader->GetScene();

    ImGui::Begin("Scene Property");

    ImGui::SeparatorText("Common");
    {
        auto fileList = m_loader->GetFilesList();

        ImGui::Combo("Files", &m_loader->index, 
            [](void* data, int idx, const char** out_text) { *out_text = (static_cast<std::string *>(data) + idx)->data();  return true; }, 
            fileList.data(), static_cast<int>(fileList.size()));

        ImGui::Checkbox("Render bounding boxes", &scene.renderBoundingBoxes);
    }
    ImGui::SeparatorText("Light");
    {
        auto& light = scene.GetLight();

        ImGui::DragFloat3("light position", light.pos.m128_f32, 0.05f, -200.f, 200.f);
        ImGui::ColorEdit3("light color", light.color.m128_f32);
    }
    ImGui::SeparatorText("Camera");
    {
        auto& camera = scene.GetCamera();

        ImGui::DragFloat3("camera position", camera.m_eye.m128_f32, 0.05f, -100.f, 100.f);
        ImGui::DragFloat3("camera target", camera.m_target.m128_f32, 0.05f, -100.f, 100.f);
    }
    ImGui::SeparatorText("Nodes");
    {
        auto& root = scene.Root();

        for (auto& child : root.GetChildren())
            RenderMeshNode(child.get());
    }

    ImGui::End();
}
