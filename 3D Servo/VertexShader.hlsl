cbuffer TransformBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VertexInput
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPos : TEXCOORD1;
};

PixelInput main(VertexInput input)
{
    PixelInput output;
    
    float4 worldPosition = mul(float4(input.pos, 1.0f), worldMatrix);
    output.worldPos = worldPosition.xyz;
    
    float4 viewPosition = mul(worldPosition, viewMatrix);
    output.position = mul(viewPosition, projectionMatrix);
    
    output.normal = normalize(mul(input.normal, (float3x3) worldMatrix));
    
    output.uv = input.uv;
    output.color = input.color;
    
    return output;
}