Texture2D sprite : register( t0 );
SamplerState samLinear : register( s0 );

float4 main(
    float4 outPos : SV_POSITION, 
    float4 col : COLOR, 
    float3 texCoord : TEXCOORD) : SV_TARGET
{
    float4 texColor = 0;
    texColor = sprite.Sample(samLinear, texCoord.xy);
    return texColor;
}
