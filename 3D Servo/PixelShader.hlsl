struct PS_INPUT
{
    float4 Position : SV_POSITION; // interpolated vertex position (system value)
    float4 Color : COLOR0; // interpolated diffuse color
};
struct PS_OUTPUT
{
    float4 RGBAColor : SV_TARGET;
};

PS_OUTPUT main(PS_INPUT In)
{
    PS_OUTPUT Output;

    Output.RGBAColor = In.Color;
    Output.RGBAColor.a = 1.0f;
    Output.RGBAColor.rgb *= Output.RGBAColor.a;

    return Output;
}