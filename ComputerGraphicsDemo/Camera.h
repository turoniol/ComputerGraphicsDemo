#pragma once

#include "FundamentalTypes.h"

struct Viewport;

class Camera
{
public:
	void Translate(const Vector& tr);
	void Zoom(float delta);
	Ray CalcCursorRay(float x, float y, const Viewport& vp);
	void Pan(float prevX, float prevY, float x, float y, const Viewport& vp);
	void Orbit(float prevX, float prevY, float x, float y, const Viewport& vp);

	void SetEyeTagerUp(Vector&& eye, Vector&& target, Vector&& up);

	Matrix4x4 LookAt() const;

	Vector CalcRight() const;
	Vector CalcDir() const;

	Vector m_eye;
	Vector m_target;
	Vector m_up;
};

