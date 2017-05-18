//TYPEDEFS
#ifndef BLOCK_SIZE
#pragma message( "BLOCK_SIZE undefined. Default to 16.")
#define BLOCK_SIZE 16 // should be defined by the application.
#endif

#define POINT_LIGHT 0
#define SPOT_LIGHT 1
#define DIRECTIONAL_LIGHT 2

struct Light
{
    /**
    * Position for point and spot lights (World space).
    */
    float4 PositionWS;
    //--------------------------------------------------------------( 16 bytes )
    /**
    * Direction for spot and directional lights (World space).
    */
    float4 DirectionWS;
    //--------------------------------------------------------------( 16 bytes )
    /**
    * Position for point and spot lights (View space).
    */
    float4 PositionVS;
    //--------------------------------------------------------------( 16 bytes )
    /**
    * Direction for spot and directional lights (View space).
    */
    float4 DirectionVS;
    //--------------------------------------------------------------( 16 bytes )
    /**
    * Color of the light. Diffuse and specular colors are not seperated.
    */
    float4 Color;
    //--------------------------------------------------------------( 16 bytes )
    /**
    * The half angle of the spotlight cone.
    */
    float SpotlightAngle;
    /**
    * The range of the light.
    */
    float Range;
 
    /**
     * The intensity of the light.
     */
    float Intensity;
 
    /**
    * Disable or enable the light.
    */
    bool Enabled;
    //--------------------------------------------------------------( 16 bytes )
 
    /**
    * The type of the light.
    */
    uint Type;
    float3 Padding;
    //--------------------------------------------------------------( 16 bytes )
    //--------------------------------------------------------------( 16 * 7 = 112 bytes )
};

struct PixelInputType
{
    float3 positionVS : TEXCOORD0;
    float2 tex : TEXCOORD1;
    float3 viewDirection : TEXCOORD2;
    float3 normal : NORMAL;
    float3 normalVS : TEXCOORD3;
    float4 position : SV_Position;
    matrix mat : TEXCOORD4;
};

//GLOBALS
Texture2D shaderTexture : register(t0);
SamplerState SampleType;

struct LightingResult
{
    float4 Diffuse;
    float4 Specular;
};

float4 DoDiffuse(Light light, float3 lightDirVS, float3 normalVS)
{
    float NdotL = saturate(dot(normalVS, lightDirVS));
    return light.Color * NdotL;
}

float4 DoSpecular(Light light, float3 viewDirVS, float3 lightDirVS, float3 normalVS)
{
    float3 R = normalize(reflect(-lightDirVS, normalVS));
    float RdotV = saturate(dot(R, viewDirVS));
    //Add material specular
    return light.Color * pow(RdotV, 8);
}

float DoAttenuation(Light light, float distance)
{
    return 1.0f - smoothstep(light.Range * 0.50f, light.Range, distance);
}

float DoSpotCone(Light light, float3 lightDir, float3 directionVS)
{
    float minCos = cos(radians(light.SpotlightAngle));

    float maxCos = lerp(minCos, 1, 0.5f);
    float cosAngle = dot(directionVS, -lightDir);

    return smoothstep(minCos, maxCos, cosAngle);
}

LightingResult DoPointLight(Light light, float3 viewDir, float3 position, float3 normal)
{
    LightingResult result;

    //Fix this later on
    //float3 L = light.positionVS - position
    //float distance = length(L)

    float distance = length(position);
    position = normalize(position);

    float attenutation = DoAttenuation(light, distance);

    result.Diffuse = DoDiffuse(light, position, normal) *
                        attenutation * light.Intensity;

    result.Specular = DoSpecular(light, viewDir, position, normal) *
                        attenutation * light.Intensity;

    return result;
}

LightingResult DoSpotLight(Light light, float3 viewDir, float3 position, float3 normal, float3 directionVS)
{
    LightingResult result;

    float3 L = position; //light.PositionVS - position;
    float distance = length(L);

    L = normalize(L);

    float attenutation = DoAttenuation(light, distance);
    float spotIntensity = DoSpotCone(light, L, directionVS);

    result.Diffuse = DoDiffuse(light, L, normal) * 
                        attenutation * spotIntensity * light.Intensity;
    
    result.Specular = DoSpecular(light, viewDir, position, normal) *
                        attenutation * spotIntensity * light.Intensity;

    return result;
}

LightingResult DoDirectionalLight(Light light, float3 viewDir, float3 position, float3 normal, float3 directionVS)
{
    LightingResult result;

    float3 L = normalize(-directionVS);

    result.Diffuse = DoDiffuse(light, L, normal) * light.Intensity;
    result.Specular = DoSpecular(light, viewDir, L, normal);

    return result;
}

StructuredBuffer<Light> LightBuffer : register(t1);
StructuredBuffer<uint> LightIndexList : register(t2);
Texture2D<uint2> LightGrid : register(t3);

//Pixel Shader
float4 ForwardPlusLightPixelShader(PixelInputType input) : SV_Target
{
    float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    float4 color;
    float3 reflection;
    float4 specular;

    float4 positionWS = float4(input.positionVS, 1.0f);
    float4 normalWS = float4(input.normal, 0.0f);
    float4 viewDir = float4(input.viewDirection, 0.0f);

    float3 positionVS = mul(positionWS, input.mat);
    float3 normalVS = normalize(mul(normalWS, input.mat));
    float3 viewDirVS = normalize(mul(viewDir, input.mat));

    

    //Sample the pixel color from the texture using the sampler at this texture coordinate location
    textureColor = shaderTexture.Sample(SampleType, input.tex);

    LightingResult totalResult = (LightingResult) 0;

    uint2 tileIndex = uint2(floor(input.position.xy / BLOCK_SIZE));

    uint startOffset = LightGrid[tileIndex].x;
    uint lightCount = LightGrid[tileIndex].y;

    for (int i = 0; i < lightCount; i++)
    {
        
        //float3 lightPositionVS = mul(LightBuffer[i].PositionWS.xyz, (float3x3) input.mat);
        //float3 DirectionVS = mul(LightBuffer[i].DirectionWS.xyz, (float3x3) input.mat);

        uint lightIndex = LightIndexList[startOffset + i];
        Light light = LightBuffer[lightIndex];

        lightDir = light.PositionVS.xyz - positionVS;

        LightingResult result = (LightingResult) 0;

        //if (!light.Enabled)
        //    continue;

        //if (light.Type != DIRECTIONAL_LIGHT && length(lightDir) > LightBuffer[i].Range)
        //    continue;

        switch (light.Type)
        {
                case POINT_LIGHT:
                {
                    result = DoPointLight(light, viewDirVS, lightDir, normalVS);
                }
                break;
                case SPOT_LIGHT:
                {
                    result = DoSpotLight(light, viewDirVS, lightDir, normalVS, light.DirectionVS.xyz);
                }
                break;
                case DIRECTIONAL_LIGHT:
                {
                    result = DoDirectionalLight(light, viewDirVS, lightDir, normalVS, light.DirectionVS.xyz);
                }
                break;
        }

        totalResult.Diffuse += result.Diffuse;
        totalResult.Specular += result.Specular;
    }

    totalResult.Diffuse = float4(totalResult.Diffuse.rgb, 1.0f);

    //Set the default output color to the ambient light value for all pixels
    color = float4(0.2f, 0.2f, 0.2f, 1.0f);

    color = saturate(totalResult.Diffuse) * textureColor;

    //Satureate the final color
    color = (float4(0.2f, 0.2f, 0.2f, 1.0f) * textureColor) + color + saturate(totalResult.Specular);

    return color;
}