#ifndef BLOCK_SIZE
#pragma message( "BLOCK_SIZE undefined. Default to 16.")
#define BLOCK_SIZE 16 // should be defined by the application.
#endif

struct Plane
{
    float3 N; //Plane normal
    float d; //Distance to origin
};

Plane ComputePlane(float3 p0, float3 p1, float3 p2)
{
    Plane plane;
    float3 v0 = p1 - p0;
    float3 v1 = p2 - p0;

    plane.N = normalize(cross(v0, v1));

    //Compute the distance to origin using p0
    plane.d = dot(plane.N, p0);

    return plane;
}

struct Frustrum
{
    Plane planes[4];
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

struct ComputeShaderInput
{
    uint3 groupID : SV_GroupID; // 3D index of the thread group in the dispatch.
    uint3 groupThreadID : SV_GroupThreadID; // 3D index of local thread ID in a thread group.
    uint3 dispatchThreadID : SV_DispatchThreadID; // 3D index of global thread ID in the dispatch.
    uint groupIndex : SV_GroupIndex; // Flattened local index of the thread within a thread group.
};

RWStructuredBuffer<Frustrum> out_Frustums : register(u0);

[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void FrustumComputeShader(ComputeShaderInput input)
{
    out_Frustums[0].planes[0].N = float3(1, 1, 1);

}