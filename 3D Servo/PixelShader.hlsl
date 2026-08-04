cbuffer MaterialBuffer : register(b1)
{
    float3 ambientColor;
    float padding1;
    
    float3 diffuseColor;
    float padding2;
    
    float3 specularColor;
    float shininess;
    
    int hasDiffuseMap; 
    int hasSpecularMap; 
    int hasNormalMap; 
    float padding3;
};

cbuffer PerFrameBuffer : register(b2)
{
    float3 cameraPosition;
    float paddingFrame;
};

Texture2D diffuseMap : register(t0);
Texture2D specularMap : register(t1);
Texture2D normalMap : register(t2);

SamplerState textureSampler : register(s0);

struct PixelInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPos : TEXCOORD1;
};

float4 main(PixelInput input) : SV_TARGET
{
    float4 texColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    if (hasDiffuseMap == 1)
    {
        texColor = diffuseMap.Sample(textureSampler, input.uv);
        texColor.a = 1.0f; //TODO: fix
    }
    
    float4 baseColor = texColor;
    if (hasDiffuseMap == 0)
    {
        baseColor *= input.color;
    }

    float finalAlpha = baseColor.a;

    float3 normal = normalize(input.normal);
    if (hasNormalMap > 0)
    {
        float3 bumpedNormal = normalMap.Sample(textureSampler, input.uv).xyz * 2.0f - 1.0f;
        normal = normalize(normal + bumpedNormal * 0.5f);
    }

    float3 lightDir = normalize(float3(0.5f, 1.0f, -0.5f));
    float3 viewDir = normalize(cameraPosition - input.worldPos);
    
    float3 ambientFactor = ambientColor;
    float3 diffColorFactor = (length(diffuseColor) > 0.001f) ? diffuseColor : float3(1.0f, 1.0f, 1.0f);
    float diffuseIntensity = max(dot(normal, lightDir), 0.0f);
    
    float3 diffuseRGB = diffColorFactor * diffuseIntensity * baseColor.rgb;
    float3 ambientRGB = ambientFactor * baseColor.rgb;
    
    float3 specularRGB = float3(0.0f, 0.0f, 0.0f);
    if (shininess > 0.0f)
    {
        float3 halfDir = normalize(lightDir + viewDir);
        float specIntensity = pow(max(dot(normal, halfDir), 0.0f), shininess);
        
        float3 specTexFactor = float3(1.0f, 1.0f, 1.0f);
        if (hasSpecularMap > 0)
        {
            specTexFactor = specularMap.Sample(textureSampler, input.uv).rgb;
        }
        
        float3 specColorFactor = (length(specularColor) > 0.001f) ? specularColor : float3(1.0f, 1.0f, 1.0f);
        specularRGB = specColorFactor * specIntensity * specTexFactor;
    }
    
    float3 totalColorRGB = ambientRGB + diffuseRGB + specularRGB;
    
    float3 premultipliedRGB = totalColorRGB * finalAlpha;
    
    return float4(premultipliedRGB, finalAlpha);
}