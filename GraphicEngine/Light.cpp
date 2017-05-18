#include "Light.h"

LightList::LightList()
{
}

LightList::LightList(const LightList &)
{
}

LightList::~LightList()
{
}

void LightList::AddLight(Light light)
{
	m_lightList.push_back(light);
}

void LightList::UpdateLights(XMMATRIX viewMatrix)
{
	XMVECTOR xmvector;

	for (int i = 0; i < m_lightList.size(); i++)
	{
		xmvector = XMLoadFloat4(&m_lightList[i].m_DirectionWS);
		xmvector = XMVector3Transform(xmvector, viewMatrix);
		XMStoreFloat4(&m_lightList[i].m_DirectionVS, xmvector);

		xmvector = XMLoadFloat4(&m_lightList[i].m_PositionWS);
		xmvector = XMVector3Transform(xmvector, viewMatrix);
		XMStoreFloat4(&m_lightList[i].m_PositionVS, xmvector);
	}
}

std::vector<Light> LightList::GetLightList()
{
	return m_lightList;
}

void LightList::Random(float maxX, float maxY, int numLights)
{
	m_lightList.reserve(numLights);

	//Seed the random generator with the current time
	srand((unsigned int)time(NULL));

	for (int i = 0; i < numLights; i++)
	{
		float red = (float)rand() / RAND_MAX;
		float green = (float)rand() / RAND_MAX;
		float blue = (float)rand() / RAND_MAX;

		float dirX = -1 + ((float)rand() / (RAND_MAX / 2.0f));
		float dirZ = -1 + ((float)rand() / (RAND_MAX / 2.0f));

		float x = (-maxX) + static_cast <float> (rand()) / (static_cast<float> (RAND_MAX / (maxX + maxX)));

		float z = (-maxY) + static_cast <float> (rand()) / (static_cast<float> (RAND_MAX / (maxY + maxY)));

		float range = 5 + ((float)rand() / (RAND_MAX / 6.0f));

		float intensity = 2 + ((float)rand() / (RAND_MAX / 5.0f));

		LightType type = LightType::Point;//(LightType)(rand() % 2);

		Light light;

		//Copy the lighting variables into the constant buffer
		light.m_Color = XMFLOAT4(red, green, blue, 1.0f);
		light.m_DirectionWS = XMFLOAT4(dirX, -1, dirZ, 1.0f);

		light.m_PositionWS = XMFLOAT4(x, 5.0f, z, 1.0f);

		light.m_Range = range;
		light.m_Intensity = intensity;
		light.m_Enabled = 1;
		light.m_SpotlightAngle = 45.0f;
		light.m_Type = type;

		m_lightList.push_back(light);
	}
}
