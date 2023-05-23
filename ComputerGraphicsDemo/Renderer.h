#pragma once

#include "FundamentalTypes.h"

struct Mesh;
struct Viewport;
class Camera;
class Scene;

struct Renderer {
	Renderer();

	void BeginFrame(int vpWidth, int vpHeight);
	void SetMatrices(Matrix4x4 world, Matrix4x4 view, Matrix4x4 proj);
	void RenderMesh(const Mesh& mesh);

	void SetLight(Point position, Color color);

	Color clearColor;
};