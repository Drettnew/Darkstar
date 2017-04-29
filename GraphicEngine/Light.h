#pragma once
#include <vector>
#include <DirectXMath.h>
#include <time.h>
#include <stdlib.h>
using namespace DirectX;

enum class LightType : uint32_t
{
	Point = 0,
	Spot = 1,
	Directional = 2
};

struct Light
{
	XMFLOAT4 m_PositionWS;
	//--------------------------------------------------------------( 16 bytes )
	XMFLOAT4 m_DirectionWS;
	//--------------------------------------------------------------( 16 bytes )

	XMFLOAT4 m_PositionVS;
	//--------------------------------------------------------------( 16 bytes )
	XMFLOAT4 m_DirectionVS;
	//--------------------------------------------------------------( 16 bytes )

	XMFLOAT4 m_Color;
	//--------------------------------------------------------------( 16 bytes )

	float m_SpotlightAngle;

	float m_Range;

	float m_Intensity;

	uint32_t m_Enabled;
	//--------------------------------------------------------------( 16 bytes )

	LightType m_Type;

	XMFLOAT3 m_Padding;
	//--------------------------------------------------------------( 16 bytes )
	//--------------------------------------------------------------( 16 * 7 = 112 bytes )

	Light::Light()
		: m_PositionWS(0, 0, 0, 1)
		, m_DirectionWS(0, 0, -1, 0)
		, m_PositionVS(0, 0, 0, 1)
		, m_DirectionVS(0, 0, 1, 0)
		, m_Color(1, 1, 1, 1)
		, m_SpotlightAngle(45.0f)
		, m_Range(20.0f)
		, m_Intensity(1.0f)
		, m_Enabled(true)
		, m_Type(LightType::Point)
	{}
};

class LightList
{
private:
	std::vector<Light> m_lightList;
public:
	LightList();
	LightList(const LightList&);
	~LightList();

	void AddLight(Light light);
	std::vector<Light> GetLightList();

	void Random(float maxX, float maxY, int numLights);
};