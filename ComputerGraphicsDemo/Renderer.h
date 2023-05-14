#pragma once

#include "FundamentalTypes.h"

struct Mesh;
struct Viewport;
class Camera;

struct Renderer {
	Renderer();

	void BeginFrame();
	void RenderMesh(const Mesh& mesh);

	Camera* camera = nullptr;
	Viewport* viewport = nullptr;

	Color clearColor;

	struct Light {
		Point position;
		Color color;
	} lightSource;
};