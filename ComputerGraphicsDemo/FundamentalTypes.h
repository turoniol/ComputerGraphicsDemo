#pragma once

#include <DirectXMath.h>
#include <concepts>

#define EXPAND3f(vec) DirectX::XMVectorGetX(vec), DirectX::XMVectorGetY(vec), DirectX::XMVectorGetZ(vec)
#define EXPAND4f(vec) EXPAND3f(vec), DirectX::XMVectorGetW(vec)

using Point = DirectX::XMVECTOR;
using Vector = DirectX::XMVECTOR;
using Color = DirectX::XMVECTOR;
using Matrix4x4 = DirectX::XMMATRIX;

struct Ray {
	Point origin;
	Vector dir;
};

using DirectX::operator+;
using DirectX::operator+=;
using DirectX::operator*;
using DirectX::operator*=;
using DirectX::operator-;
using DirectX::operator-=;

DirectX::XMFLOAT4X4 GetFloat4x4(Matrix4x4 m);