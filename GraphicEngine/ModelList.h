#pragma once

#include <DirectXMath.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include "Model.h"

using namespace DirectX;

class ModelList
{
private:

public:
	ModelList();
	~ModelList();

	bool Initialize(int numModels);
	void Shutdown();

	bool Random(Assets* assets, int numModels, const char* filepath, int rangeX, int rangeZ, bool uniform);

	bool AddModel(std::string filepath, Assets* assets);
	bool RemoveModel(int index);

	int GetModelCount();

	Model* GetModel(int index);
	std::vector<Model> GetModelList();

private:
	std::vector<Model> m_modelList;
};