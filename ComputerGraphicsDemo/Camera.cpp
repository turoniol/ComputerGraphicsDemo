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

void Camera::Transform(Matrix4x4 matrix)
{
	m_eye = XMVector3Transform(m_eye, matrix);
	m_target = XMVector3Transform(m_target, matrix);
	m_up = XMVector3TransformNormal(m_up, matrix);
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