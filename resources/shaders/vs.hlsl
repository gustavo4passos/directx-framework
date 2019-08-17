cbuffer cbPerObject : register(	b0 )
{
	float4x4 objectView;
};

void main( 
	float3 pos : POSITION, 
	float3 inCol : COLOR, 
	float3 inTexCoord : TEXCOORD,
	out float4 outPos : SV_POSITION,
	out float4 outCol : COLOR,
	out float3 outTexCoord : TEXCOORD )
{
	outCol = float4(inCol, 1.0);
	outTexCoord = inTexCoord;
	outPos = mul(float4(pos, 1.0), objectView);
}