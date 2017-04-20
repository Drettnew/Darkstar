#pragma once

#include <DirectXMath.h>
#include <stdlib.h>
#include <time.h>
using namespace DirectX;

class ModelList
{
private:
	struct ModelInfoType
	{
		XMFLOAT4 color;
		float positionX, positionY, positionZ;
	};
public:
	ModelList();
	~ModelList();

	bool Initialize(int numModels);
	void Shutdown();

	int GetModelCount();
	void GetData(int index, float &positionX, float &positionY, float &positionZ, XMFLOAT4 &color);

private:
	int m_modelCount;
	ModelInfoType* m_ModelInfoList;

};