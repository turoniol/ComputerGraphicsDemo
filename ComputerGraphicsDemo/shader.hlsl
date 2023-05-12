struct PSInput {
	float4 pos : SV_POSITION;
};

PSInput VSMain(float3 pos : POSITION)
{
	PSInput res;
	res.pos = float4(pos, 1);
	return res;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	return input.pos;
}