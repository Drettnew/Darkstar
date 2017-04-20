//GLOBALS
Texture2D shaderTexture;
SamplerState SampleType;

cbuffer LightBuffer
{
    float4 ambientColor;
    //-------------------------- ( 16 bytes )
    float4 diffuseColor;
    //-------------------------- ( 16 bytes )
    float3 lightDirection;
    float specularPower;
    //-------------------------- ( 16 bytes )
    float4 specularColor;
    //-------------------------- ( 16 bytes )
};  //-------------------------- ( 16 * 4 = 64 bytes )

//TYPEDEFS
struct PixelInputType
{
    float4 position : SV_Position;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewDirection : TEXCOORD1;
};

//Pixel Shader
float4 LightPixelShader(PixelInputType input) : SV_Target
{
    float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    float4 color;
    float3 reflection;
    float4 specular;

    //Sample the pixel color from the texture using the sampler at this texture coordinate location
    textureColor = shaderTexture.Sample(SampleType, input.tex);

    //Set the default output color to the ambient light value for all pixels
    color = ambientColor;

    //Initialize the specular color
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

    //Invert the light dir for calculations
    lightDir = -lightDirection;

    //Calculate the amount of light on this pixel
    lightIntensity = saturate(dot(input.normal, lightDir));

    if(lightIntensity > 0.0f)
    {
        //Determin the final amount of diffuse color based on the diffuse color combined with the light intensity
        color += diffuseColor * lightIntensity;

        //Saturate the ambient and diffuse color
        color = saturate(color);

        //Calcualte the refelction vector based on the light intensity, normal vector and light direction
        reflection = normalize(2 * lightIntensity * input.normal - lightDir);

        //Determine the amount of specular light based on the reflection vector, viewing direction and specular power
        specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
    }

    color = color * textureColor;

    //Satureate the final color
    color = saturate(color + specular);

    return color;
}