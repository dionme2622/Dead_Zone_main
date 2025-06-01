#ifndef _SHADOW_FX_
#define _SHADOW_FX_

#include "params.fx"
#include "utils.fx"

struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float4 weight : WEIGHT;
    float4 indices : INDICES;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 clipPos : POSITION; 
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0.f;

    if (g_int_3 == 1)       // Skinning이 적용되었는가?
        Skinning(input.pos, input.normal, input.tangent, input.weight, input.indices);
    
    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.clipPos = output.pos;

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    return float4(input.clipPos.z, 0.f, 0.f, 0.f);
}

#endif