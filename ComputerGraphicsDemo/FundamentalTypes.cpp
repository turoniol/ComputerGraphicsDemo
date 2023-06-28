#include "FundamentalTypes.h"

using namespace DirectX;

DirectX::XMFLOAT4X4 GetFloat4x4(Matrix4x4 m) {
	XMFLOAT4X4 res{};
	XMStoreFloat4x4(&res, m);
	return res;
}

bool VectorIsUnit(Vector v)
{
	static const Vector unitVectorEpsilon = XMVectorSet(1.0e-4f, 1.0e-4f, 1.0e-4f, 1.0e-4f);

	XMVECTOR Difference = XMVectorSubtract(XMVector3Length(v), XMVectorSplatOne());
	return XMVector4Less(XMVectorAbs(Difference), unitVectorEpsilon);
}
