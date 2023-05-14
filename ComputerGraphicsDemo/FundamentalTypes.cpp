#include "FundamentalTypes.h"

using namespace DirectX;

DirectX::XMFLOAT4X4 GetFloat4x4(Matrix4x4 m) {
	XMFLOAT4X4 res{};
	XMStoreFloat4x4(&res, m);
	return res;
}