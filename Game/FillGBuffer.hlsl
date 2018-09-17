#define CommonSignature \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
    "RootConstants(b0,  num32BitConstants = 1, visibility = SHADER_VISIBILITY_ALL), " \
    "CBV(b1, visibility = SHADER_VISIBILITY_VERTEX), " \
    "CBV(b2, visibility = SHADER_VISIBILITY_VERTEX), " \
    "DescriptorTable(SRV(t0, numDescriptors = unbounded), visibility = SHADER_VISIBILITY_PIXEL)," \
    "StaticSampler(s0, maxAnisotropy = 16, visibility = SHADER_VISIBILITY_PIXEL)," \
    "StaticSampler(s1, visibility = SHADER_VISIBILITY_PIXEL," \
        "addressU = TEXTURE_ADDRESS_CLAMP," \
        "addressV = TEXTURE_ADDRESS_CLAMP," \
        "addressW = TEXTURE_ADDRESS_CLAMP," \
        "comparisonFunc = COMPARISON_GREATER_EQUAL," \
        "filter = FILTER_MIN_MAG_LINEAR_MIP_POINT)"


struct FrameConstantBuffer
{
    float4x4 View;
    float4x4 Project;
};

struct ObjectConstantBuffer
{
    float4x4 WVP;
    float4x4 World;
    float3x3 Normal;
};


struct VS_IN
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 Texcoord : TEXCOORD;
};

struct PS_IN
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;
    float2 Texcoord : TEXCOORD;
};
                   
uint MaterialID : register(b0);
ConstantBuffer<FrameConstantBuffer> FrameConstants : register(b1);
ConstantBuffer<ObjectConstantBuffer> ObjectConstant : register(b2);

Texture2D<float4> Textures[] : register(t0);
SamplerState SamplerAnisotropy : register(s0);
SamplerState SamplerLinear : register(s1);

[RootSignature(CommonSignature)]
 PS_IN VSMain(VS_IN input)
{

    PS_IN result;
  
    result.Position = mul(ObjectConstant.WVP, float4(input.Position, 1.0f));
 
    float3 N = mul((float3x3) ObjectConstant.Normal, input.Normal);
    float3 T = mul((float3x3) ObjectConstant.Normal, input.Tangent);
    float3 B = cross(T, N);

  
    result.Texcoord = input.Texcoord;
    result.Tangent  = T;
    result.Binormal = B;
    result.Normal   = N;
    return result;
}

[RootSignature(CommonSignature)]
float4 PSMain(PS_IN input) : SV_TARGET
{

    float3x3 TBN = float3x3(normalize(input.Tangent), normalize(input.Binormal), normalize(input.Normal));
    float3 normal = normalize(mul(normalize(2.0 * Textures[3 * MaterialID + 2].Sample(SamplerAnisotropy, input.Texcoord).rgb - 1.0), TBN));

  //  float4 diffuse  = g_Textures[3 * input.Material + 0].Sample(g_Sampler[0], input.Texcoord);
  //  float4 specular = g_Textures[3 * input.Material + 1].Sample(g_Sampler[0], input.Texcoord);
 //   float4 normal   = g_Textures[3 * input.Material + 2].Sample(g_Sampler[0], input.Texcoord);

    return float4(0.5*normal + 0.5, 1.0);
}