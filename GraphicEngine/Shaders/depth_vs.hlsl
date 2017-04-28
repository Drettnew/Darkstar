//GLOBALS
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

//TYPEDEFS
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_Position;
};

//Vertex Shader
PixelInputType depthVertexShader(VertexInputType input)
{
    PixelInputType output;

    float4x4 modelViewProjection = mul(mul(worldMatrix, viewMatrix), projectionMatrix);

    //Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    //Calcualte the postion of the vertex against the world, view and projection matrices.
    output.position = mul(input.position, modelViewProjection);

    return output;
}
