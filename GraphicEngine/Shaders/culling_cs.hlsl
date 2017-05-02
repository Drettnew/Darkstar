#ifndef BLOCK_SIZE
#pragma message( "BLOCK_SIZE undefined. Default to 16.")
#define BLOCK_SIZE 16 // should be defined by the application.
#endif

#ifndef NUM_LIGHTS
#pragma message( "NUM_LIGHTS undefined. Default to 8.")
#define NUM_LIGHTS 1 // should be defined by the application.
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

struct Plane
{
    float3 N; //Plane normal
    float d; //Distance to origin
};

struct Sphere
{
    float3 c; // Center point.
    float r; // Radius.
};

struct Cone
{
    float3 T; // Cone tip.
    float h; // Height of the cone.
    float3 d; // Direction of the cone.
    float r; // bottom radius of the cone.
};

struct Frustum
{
    Plane planes[4];
};

struct ComputeShaderInput
{
    uint3 groupID : SV_GroupID; // 3D index of the thread group in the dispatch.
    uint3 groupThreadID : SV_GroupThreadID; // 3D index of local thread ID in a thread group.
    uint3 dispatchThreadID : SV_DispatchThreadID; // 3D index of global thread ID in the dispatch.
    uint groupIndex : SV_GroupIndex; // Flattened local index of the thread within a thread group.
};

cbuffer DispatchParams : register(b0)
{
    // Number of groups dispatched. (This parameter is not available as an HLSL system value!)
    uint3 numThreadGroups;
    // Total number of threads dispatched. (Also not available as an HLSL system value!)
    // Note: This value may be less than the actual number of threads executed 
    // if the screen size is not evenly divisible by the block size.
    uint3 numThreads;
    uint2 padding; // implicit padding to 16 bytes.
}

// Parameters required to convert screen space coordinates to view space params.
cbuffer ScreenToViewParams : register(b1)
{
    float4x4 InverseProjection;
    float2 ScreenDimensions;
}

// Convert clip space coordinates to view space
float4 ClipToView(float4 clip)
{
    // View space position.
    float4 view = mul(InverseProjection, clip);
    // Perspecitive projection.
    view = view / view.w;

    return view;
}

// Convert screen space coordinates to view space.
float4 ScreenToView(float4 screen)
{
    // Convert to normalized texture coordinates
    float2 texCoord = screen.xy / ScreenDimensions;

    // Convert to clip space
    float4 clip = float4(float2(texCoord.x, 1.0f - texCoord.y) * 2.0f - 1.0f, screen.z, screen.w);

    return ClipToView(clip);
}

bool SphereInsidePlane(Sphere sphere, Plane plane)
{
    return dot(plane.N, sphere.c) - plane.d < -sphere.r;
}

// Check to see if a point is fully behind (inside the negative halfspace of) a plane.
bool PointInsidePlane(float3 p, Plane plane)
{
    return dot(plane.N, p) - plane.d < 0;
}

// Check to see if a cone if fully behind (inside the negative halfspace of) a plane.
// Source: Real-time collision detection, Christer Ericson (2005)
bool ConeInsidePlane(Cone cone, Plane plane)
{
    // Compute the farthest point on the end of the cone to the positive space of the plane.
    float3 m = cross(cross(plane.N, cone.d), cone.d);
    float3 Q = cone.T + cone.d * cone.h - m * cone.r;

    // The cone is in the negative halfspace of the plane if both
    // the tip of the cone and the farthest point on the end of the cone to the 
    // positive halfspace of the plane are both inside the negative halfspace 
    // of the plane.
    return PointInsidePlane(cone.T, plane) && PointInsidePlane(Q, plane);
}

// Check to see of a light is partially contained within the frustum.
bool SphereInsideFrustum(Sphere sphere, Frustum frustum, float zNear, float zFar)
{
    bool result = true;

    // First check depth
    // Note: Here, the view vector points in the -Z axis so the 
    // far depth value will be approaching -infinity.
    if (sphere.c.z - sphere.r > zNear || sphere.c.z + sphere.r < zFar)
    {
        result = false;
    }

    // Then check frustum planes
    for (int i = 0; i < 4 && result; i++)
    {
        if (SphereInsidePlane(sphere, frustum.planes[i]))
        {
            result = false;
        }
    }

    return result;
}

bool ConeInsideFrustum(Cone cone, Frustum frustum, float zNear, float zFar)
{
    bool result = true;

    Plane nearPlane = { float3(0, 0, -1), -zNear };
    Plane farPlane = { float3(0, 0, 1), zFar };

    // First check the near and far clipping planes.
    if (ConeInsidePlane(cone, nearPlane) || ConeInsidePlane(cone, farPlane))
    {
        result = false;
    }

    // Then check frustum planes
    for (int i = 0; i < 4 && result; i++)
    {
        if (ConeInsidePlane(cone, frustum.planes[i]))
        {
            result = false;
        }
    }

    return result;
}

SamplerState SampleType : register(s0);

Texture2D DepthTextureVS : register(t0);

StructuredBuffer<Frustum> in_Frustums : register(t1);

StructuredBuffer<Light> LightBuffer : register(t2);

globallycoherent RWStructuredBuffer<uint> LightIndexCounter : register(u0);

RWStructuredBuffer<uint> LightIndexList : register(u1);

RWTexture2D<uint2> LightGrid : register(u2);

// Group shared variables.
groupshared uint uMinDepth;
groupshared uint uMaxDepth;
groupshared Frustum GroupFrustum;

// Opaque geometry light lists.
groupshared uint LightCount;
groupshared uint LightIndexStartOffset;
groupshared uint LightList[1024];

// Add the light to the visible light list for opaque geometry.
void AppendLight(uint lightIndex)
{
    uint index; // Index into the visible lights array.
    InterlockedAdd(LightCount, 1, index);
    if (index < 1024)
    {
        LightList[index] = lightIndex;
    }
}

[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void CullingComputeShader(ComputeShaderInput input)
{
    // Calculate min & max depth in threadgroup / tile.
    int2 texCoord = input.dispatchThreadID.xy;
    float fDepth = DepthTextureVS.Load(int3(texCoord, 0)).r;

    uint uDepth = asuint(fDepth);

    if (input.groupIndex == 0) // Avoid contention by other threads in the group.
    {
        uMinDepth = 0xffffffff;
        uMaxDepth = 0;
        LightCount = 0;
        GroupFrustum = in_Frustums[input.groupID.x + (input.groupID.y * numThreadGroups.x)];
    }

    GroupMemoryBarrierWithGroupSync();

    InterlockedMin(uMinDepth, uDepth);
    InterlockedMax(uMaxDepth, uDepth);

    GroupMemoryBarrierWithGroupSync();

    float fMinDepth = asfloat(uMinDepth);
    float fMaxDepth = asfloat(uMaxDepth);

    // Convert depth values to view space.
    float minDepthVS = ScreenToView(float4(0, 0, fMinDepth, 1)).z;
    float maxDepthVS = ScreenToView(float4(0, 0, fMaxDepth, 1)).z;
    float nearClipVS = ScreenToView(float4(0, 0, 0, 1)).z;

    // Clipping plane for minimum depth value 
    // (used for testing lights within the bounds of opaque geometry).
    Plane minPlane = { float3(0, 0, -1), -minDepthVS };

    // Cull lights
    // Each thread in a group will cull 1 light until all lights have been culled.
    for (uint i = input.groupIndex; i < NUM_LIGHTS; i += BLOCK_SIZE * BLOCK_SIZE)
    {
        if (LightBuffer[i].Enabled)
        {
            Light light = LightBuffer[i];

            switch (light.Type)
            {
                case POINT_LIGHT:
            {
                        Sphere sphere = { light.PositionVS.xyz, light.Range };
                        if (SphereInsideFrustum(sphere, GroupFrustum, nearClipVS, maxDepthVS))
                        {
                            if (!SphereInsidePlane(sphere, minPlane))
                            {
                        // Add light to light list for opaque geometry.
                                AppendLight(i);
                            }
                        }
                    }
                    break;
                case SPOT_LIGHT:
            {
                        float coneRadius = tan(radians(light.SpotlightAngle)) * light.Range;
                        Cone cone = { light.PositionVS.xyz, light.Range, light.DirectionVS.xyz, coneRadius };
                        if (ConeInsideFrustum(cone, GroupFrustum, nearClipVS, maxDepthVS))
                        {
                            if (!ConeInsidePlane(cone, minPlane))
                            {
                        // Add light to light list for opaque geometry.
                                AppendLight(i);
                            }
                        }
                    }
                    break;
                case DIRECTIONAL_LIGHT:
            {
                // Directional lights always get added to our light list.
                // (Hopefully there are not too many directional lights!)
                        AppendLight(i);
                    }
                    break;
            }
        }
    }

    // Wait till all threads in group have caught up.
    GroupMemoryBarrierWithGroupSync();

    // Update global memory with visible light buffer.
    // First update the light grid (only thread 0 in group needs to do this)
    if (input.groupIndex == 0)
    {
        // Update light grid for opaque geometry.
        InterlockedAdd(LightIndexCounter[0], LightCount, LightIndexStartOffset);
        LightGrid[input.groupID.xy] = uint2(LightIndexStartOffset, LightCount);
    }

    GroupMemoryBarrierWithGroupSync();

    // Now update the light index list (all threads).
    // For opaque goemetry.
    for (i = input.groupIndex; i < LightCount; i += BLOCK_SIZE * BLOCK_SIZE)
    {
        LightIndexList[LightIndexStartOffset + i] = LightList[i];
    }

    //// Update the debug texture output.
    //if (input.groupThreadID.x == 0 || input.groupThreadID.y == 0)
    //{
    //    DebugTexture[texCoord] = float4(0, 0, 0, 0.9f);
    //}
    //else if (input.groupThreadID.x == 1 || input.groupThreadID.y == 1)
    //{
    //    DebugTexture[texCoord] = float4(1, 1, 1, 0.5f);
    //}
    //else if (o_LightCount > 0)
    //{
    //    float normalizedLightCount = o_LightCount / 50.0f;
    //    float4 lightCountHeatMapColor = LightCountHeatMap.SampleLevel(LinearClampSampler, float2(normalizedLightCount, 0), 0);
    //    DebugTexture[texCoord] = lightCountHeatMapColor;
    //}
    //else
    //{
    //    DebugTexture[texCoord] = float4(0, 0, 0, 1);
    //}
}