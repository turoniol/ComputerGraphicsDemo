#include "Camera.h"

#include "Viewport.h"
#include "Logger.h"

#include <utility>

using namespace DirectX;

void Camera::Translate(const Vector& tr)
{
	m_eye = m_eye + tr;
	m_target = m_target + tr;
}

void Camera::Zoom(float delta)
{
	auto newEye = m_eye + delta * CalcDir();
	float dist{};
	XMStoreFloat(&dist, XMVector3Length(newEye));

	if (dist > 0.5f && dist < 150.f)
		m_eye = newEye;
}

Ray Camera::CalcCursorRay(float x, float y, const Viewport& vp) {
	auto far = XMVector3Unproject(XMVectorSet(x, y, 1.0f, 0.0f), 0, 0, vp.width, vp.height, 0.0f, 1.0f, vp.ProjectionMatrix(), LookAt(), XMMatrixIdentity());
	auto near = XMVector3Unproject(XMVectorSet(x, y, 0.0f, 0.0f), 0, 0, vp.width, vp.height, 0.0f, 1.0f, vp.ProjectionMatrix(), LookAt(), XMMatrixIdentity());
	return  { near, XMVector3Normalize(far - near) };
}

void Camera::Pan(float prevX, float prevY, float x, float y, const Viewport& vp)
{
	auto dir = CalcDir();

	auto prevRay = CalcCursorRay(prevX, prevY, vp);
	auto newRay = CalcCursorRay(x, y, vp);
	auto targetPlane = XMPlaneFromPointNormal(m_target, dir);
	auto prevPos = XMPlaneIntersectLine(targetPlane, prevRay.origin, prevRay.dir * (vp.farZ - vp.nearZ));
	auto newPos = XMPlaneIntersectLine(targetPlane, newRay.origin, newRay.dir * (vp.farZ - vp.nearZ));

	Translate(prevPos - newPos);
}

void Camera::Orbit(float prevX, float prevY, float x, float y, const Viewport& vp)
{
	auto calcVec = [=](float x, float y) {
		const float xCenter = vp.width / 2.f;
		const float yCenter = vp.height / 2.f;
		x = 2 * (x - xCenter) / vp.width * vp.AspectRatio();
		y = 2 * (y - yCenter) / vp.height;

		
		const float sq = x * x + y * y;

		float z = 0.0f;
		if (sq < 1.0f)
			z = sqrtf(1.0f - sq);

		return XMVectorSet(x, -y, z, 0);
	};

	auto prevVec = calcVec(prevX, prevY);
	auto nextVec = calcVec(x, y);

	auto vangle = XMVector3AngleBetweenVectors(prevVec, nextVec);
	float angle{};
	XMStoreFloat(&angle, vangle);

	auto vreverseLookAt = XMMatrixInverse(nullptr, LookAt());
	auto vaxis = XMVector3TransformNormal(XMVector3Cross(prevVec, nextVec), vreverseLookAt);
	auto vquat = XMQuaternionRotationAxis(vaxis, -angle);
	auto veye = XMVectorSubtract(m_eye, m_target);
	m_eye = XMVectorAdd(XMVector3Rotate(veye, vquat), m_target);

	m_up = XMVector3Rotate(m_up, vquat);
}

void Camera::SetEyeTagerUp(Vector&& eye, Vector&& target, Vector&& up)
{
	m_eye = std::move(eye);
	m_target = std::move(target);
	m_up = std::move(up);
}

Matrix4x4 Camera::LookAt() const
{
	return XMMatrixLookAtRH(m_eye, m_target, m_up);
}

Vector Camera::CalcRight() const
{
	return XMVector3Normalize(XMVector3Cross(CalcDir(), m_up));
}

Vector Camera::CalcDir() const
{
	return XMVector3Normalize(XMVectorSubtract(m_target, m_eye));
}