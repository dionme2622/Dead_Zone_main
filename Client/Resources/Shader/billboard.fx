// billboard.fx

#ifndef _BILLBOARD_FX_
#define _BILLBOARD_FX_

#include "params.fx"
#include "utils.fx"

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    // 월드 변환 (중심점)
    float4 worldPos = mul(float4(0, 0, 0, 1), g_matWorld);

    // 카메라 뷰 행렬에서 오른쪽/위쪽 벡터 추출
    float3 camRight = float3(g_matView._11, g_matView._12, g_matView._13);
    float3 camUp = float3(g_matView._21, g_matView._22, g_matView._23);

    // 쿼드의 각 정점 오프셋 (uv: (0,0)~(1,1) → (-0.5, -0.5)~(0.5, 0.5))
    float2 offset = input.uv - float2(0.5f, 0.5f);
    float scale = 1.0f; // 필요시 상수버퍼로 전달
    worldPos.xyz += (camRight * offset.x + camUp * offset.y) * scale;

    // 투영
    output.pos = mul(worldPos, mul(g_matView, g_matProjection));
    output.uv = input.uv;
    return output;
}

struct GS_OUT
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD;
};

float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 color = float4(1, 1, 1, 1);
    if (g_tex_on_0)
        color = g_tex_0.Sample(g_sam_0, input.uv);

    // Additive(가산) 효과: 알파를 곱하지 않고 색상만 더함
    // 필요시 color.rgb *= color.a; // 밝기 조절
    // color.a = 1; // 블렌딩은 렌더스테이트에서 Additive로 설정

    if (color.a < 0.01f)
        discard;

    // 머즐 플래시 특유의 밝은 효과를 위해 알파를 곱해도 되고, 안 곱해도 됨
    //color.rgb *= color.a;

    return color;
}

#endif