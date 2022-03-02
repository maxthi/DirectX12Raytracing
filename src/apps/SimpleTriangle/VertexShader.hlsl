struct ModelViewProjection
{
    matrix MVP;
};

ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b0);

struct VertexInputAttributes
{
    float3 Position : POSITION;
    float3 Color : COLOR;
};

struct VertexShaderOutput
{
    float4 Color : COLOR;
    float4 Position : SV_Position;
};

VertexShaderOutput main(VertexInputAttributes input) {
    VertexShaderOutput output;

    output.Position = mul(ModelViewProjectionCB.MVP, float4(input.Position, 1.0f));
    output.Color = float4(input.Color, 1.0f);

    return output;
}