#pragma once

#include "FundamentalTypes.h"

#include <DirectXCollision.h>
#include <array>

struct Mesh;
struct Viewport;
class Camera;
class Scene;

struct Renderer {
	Renderer();

	void BeginFrame(int vpWidth, int vpHeight);
	void SetMatrices(Matrix4x4 world, Matrix4x4 view, Matrix4x4 proj);
	void RenderBoundingBox(DirectX::BoundingBox bb, std::array<float, 4> color = {1.0f, 0.0f, 0.0f, 1.0f});
	void RenderMesh(const Mesh& mesh);

	void SetLight(Point position, Color color);

	Color clearColor;
};