struct PSInput {
	float4 position : SV_POSITION;
};


PSInput main( float3 pos : POSITION)
{
	PSInput input;
	input.position = float4(pos, 1.0f);
	return input;
}