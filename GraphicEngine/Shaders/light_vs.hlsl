//GLOBALS
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer CameraBuffer
{
    float3 cameraPosition;
    float padding;
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
    float3 positionVS   : TEXCOORD0;
    float2 tex          : TEXCOORD1;
    float3 viewDirection: TEXCOORD2;
    float3 normal       : NORMAL;
    float3 normalVS : TEXCOORD3;
    float4 position     : SV_Position;
    matrix mat : TEXCOORD4;
};

//Vertex Shader
PixelInputType LightVertexShader(VertexInputType input)
{
    PixelInputType output;
    float4 worldPosition;

    float4x4 modelView = mul(worldMatrix, viewMatrix);
    float4x4 modelViewProjection = mul(modelView, projectionMatrix);

    output.mat = viewMatrix;

    //Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    //Calcualte the postion of the vertex against the world, view and projection matrices.
    output.position = mul(input.position, modelViewProjection);

    //Store the texture coordinates for the pixel shader
    output.tex = input.tex;

    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);

    //Calcuate the normal vector against the world matrix only.
    output.normalVS = mul(input.normal, (float3x3) modelView);
 
    //Normalize the normal vector
    output.normalVS = normalize(output.normalVS);

    //Calculate the position of the vertex in the world
    worldPosition = mul(input.position, worldMatrix);
    output.positionVS = worldPosition;

    //output.positionVS = mul(input.position, worldMatrix).xyz;
    //output.positionVS = mul(output.positionVS, (float3x3)viewMatrix);

    //Determine the viewing direction based on the position of the camera and the position of the vertex in the world
    output.viewDirection = cameraPosition.xyz - worldPosition.xyz;

    //Normalize the viewing direction vector
    output.viewDirection = normalize(output.viewDirection);

    return output;
}