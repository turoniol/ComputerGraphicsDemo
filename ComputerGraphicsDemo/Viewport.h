#pragma once

#include "FundamentalTypes.h"

struct Viewport
{
	float fov = 45.0f;
	float nearZ = 0.1f;
	float farZ = 100.f;
	int width, height;

	Viewport(int w, int h) : width(w), height(h) {}

	float AspectRatio() const;
	Matrix4x4 ProjectionMatrix() const;
};

