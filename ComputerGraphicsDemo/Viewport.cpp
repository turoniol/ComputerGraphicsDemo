#include "Viewport.h"

float Viewport::AspectRatio() const
{
    return (float) width / height;
}

Matrix4x4 Viewport::ProjectionMatrix() const
{
    return DirectX::XMMatrixPerspectiveFovRH(fov, AspectRatio(), nearZ, farZ);
}


