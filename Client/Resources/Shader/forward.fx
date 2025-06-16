#ifndef _DEFAULT_FX_
#define _DEFAULT_FX_

#include "params.fx"
#include "utils.fx"

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    float3 viewPos : POSITION;
    float3 viewNormal : NORMAL;
    float3 viewTangent : TANGENT;
    float3 viewBinormal : BINORMAL;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.uv = input.uv;

    output.viewPos = mul(float4(input.pos, 1.f), g_matWV).xyz;
    output.viewNormal = normalize(mul(float4(input.normal, 0.f), g_matWV).xyz);
    output.viewTangent = normalize(mul(float4(input.tangent, 0.f), g_matWV).xyz);
    output.viewBinormal = normalize(cross(output.viewTangent, output.viewNormal));

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    if (g_tex_on_0)
        color = g_tex_0.Sample(g_sam_0, input.uv);

    float3 viewNormal = input.viewNormal;
    if (g_tex_on_1)
    {
        // [0,255] 범위에서 [0,1]로 변환
        float3 tangentSpaceNormal = g_tex_1.Sample(g_sam_0, input.uv).xyz;
        // [0,1] 범위에서 [-1,1]로 변환
        tangentSpaceNormal = (tangentSpaceNormal - 0.5f) * 2.f;
        float3x3 matTBN = { input.viewTangent, input.viewBinormal, input.viewNormal };
        viewNormal = normalize(mul(tangentSpaceNormal, matTBN));
    }

    LightColor totalColor = (LightColor) 0.f;

    for (int i = 0; i < g_lightCount; ++i)
    {
        LightColor color = CalculateLightColor(i, viewNormal, input.viewPos);
        totalColor.diffuse += color.diffuse;
        totalColor.ambient += color.ambient;
        totalColor.specular += color.specular;
    }

    color.xyz = (totalColor.diffuse.xyz * color.xyz)
        + totalColor.ambient.xyz * color.xyz
        + totalColor.specular.xyz;

    return color;
}

// [Texture Shader]
// g_tex_0 : Output Texture
// AlphaBlend : true
struct VS_TEX_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_TEX_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

VS_TEX_OUT VS_Tex(VS_TEX_IN input)
{
    VS_TEX_OUT output = (VS_TEX_OUT) 0;

    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.uv = input.uv;

    return output;
}

float4 PS_Tex(VS_TEX_OUT input) : SV_Target
{
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    if (g_tex_on_0)
        color = g_tex_0.Sample(g_sam_0, input.uv);

    return color;
}




VS_TEX_OUT VS_UI(VS_TEX_IN input)
{
    VS_TEX_OUT output = (VS_TEX_OUT) 0;

    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.uv = input.uv;

    return output;
}

float4 PS_UI(VS_TEX_OUT input) : SV_Target
{
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    if (g_tex_on_0)
        color = g_tex_0.Sample(g_sam_0, input.uv);
        
    if (color.a <= 0.01f)
        discard;

    
    return color;
}


VS_TEX_OUT VS_Number_UI(VS_TEX_IN input)
{
    VS_TEX_OUT output = (VS_TEX_OUT) 0;
    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.uv = input.uv;
    return output;
}

float4 PS_Number_UI(VS_TEX_OUT input) : SV_Target
{
    // 숫자 인덱스 (0~9)
    int number = g_int_1;
    // 한 숫자가 차지하는 UV 폭
    float digitWidth = 1.0 / 10.0;

    // 현재 사각형의 uv를 숫자 영역으로 변환
    float2 uv = input.uv;
    uv.x = uv.x * digitWidth + number * digitWidth;

    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    if (g_tex_on_0)
        color = g_tex_0.Sample(g_sam_0, uv);

    //if (color.a <= 0.01f)
    //    discard;

    return color;
}


VS_TEX_OUT VS_Background_UI(VS_TEX_IN input)
{
    VS_TEX_OUT output = (VS_TEX_OUT) 0;

    output.pos = mul(float4(input.pos, 1.f), g_matWVP);

    // 시간에 따라 UV x 좌표를 이동 (왼쪽으로 움직이게)
    output.uv = input.uv;

    output.uv.x += g_float_1 * 0.1f; // 속도 조절

    return output;
}

float4 PS_Background_UI(VS_TEX_OUT input) : SV_Target
{
    float4 color = g_tex_0.Sample(g_sam_0, input.uv); // wrap 모드여야 함

    return color;
}



VS_TEX_OUT VS_Button_UI(VS_TEX_IN input)
{
    VS_TEX_OUT output = (VS_TEX_OUT) 0;

    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.uv = input.uv;

    return output;
}

float4 PS_Button_UI(VS_TEX_OUT input) : SV_Target
{
    float4 color = g_tex_0.Sample(g_sam_0, input.uv);

    if (color.a <= 0.01)
        discard;
    
    if (g_int_1 == 1)
    {
    
        float glowIntensity = 0.9;
        float glowRadius = 0.5;
        
        float4 glow = float4(0.0, 0.0, 0.0, 0.0);
        float samples = 1.0;
        glow += g_tex_0.Sample(g_sam_0, input.uv + float2(glowRadius, 0.0));
        glow += g_tex_0.Sample(g_sam_0, input.uv + float2(-glowRadius, 0.0));
        glow += g_tex_0.Sample(g_sam_0, input.uv + float2(0.0, glowRadius));
        glow += g_tex_0.Sample(g_sam_0, input.uv + float2(0.0, -glowRadius));
        samples += 4.0;
        
        glow /= samples;
        
        // Apply grayscale to the base color
        float grayscale = dot(color.rgb, float3(0.299, 0.587, 0.114));
        color.rgb = float3(grayscale, grayscale, grayscale);
        
        // Apply grayscale to the glow color
        float glowGrayscale = dot(glow.rgb, float3(0.299, 0.587, 0.114));
        glow.rgb = float3(glowGrayscale, glowGrayscale, glowGrayscale);
        
        // Combine grayscale color with grayscale glow
        color.rgb = color.rgb - glow.rgb * glowIntensity;
        color.a = max(color.a, glow.a);
        
    }
    return color;
}

VS_TEX_OUT VS_Transparent_UI(VS_TEX_IN input)
{
    VS_TEX_OUT output = (VS_TEX_OUT) 0;

    output.pos = mul(float4(input.pos, 1.f), g_matWVP);
    output.uv = input.uv;

    return output;
}

float4 PS_Transparent_UI(VS_TEX_OUT input) : SV_Target
{
    float4 color = g_tex_0.Sample(g_sam_0, input.uv);
    
    color.a = 0.7;
    
    return color;
}





struct VS_COLLIDER_IN
{
    float3 pos : POSITION;
};

struct VS_COLLIDER_OUT
{
    float4 pos : SV_POSITION;
};

VS_COLLIDER_OUT VS_Collider(VS_COLLIDER_IN input)
{
    VS_COLLIDER_OUT output = (VS_COLLIDER_OUT) 0;

    output.pos = mul(float4(input.pos, 1.f), g_matWVP);

    return output;
}

float4 PS_Collider(VS_COLLIDER_OUT input) : SV_Target
{
    float4 color = float4(0.f, 1.f, 0.f, 1.f);

    return color;
}


#endif