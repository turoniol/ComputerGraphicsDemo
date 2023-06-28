#pragma once

#include "PropertryWindow.h"

class MeshNode;

class MeshNodePropertyWindow : public PropertyWindow
{
public:
    void SetMeshNode(MeshNode* handler);
    void Render() override;

private:
    void NodeInfo();
    void ParentInfo();
    void BoundingBoxInfo();
    void TransformationInfo();
    void MaterialInfo();

    MeshNode* m_nodeHandler = nullptr;
};

