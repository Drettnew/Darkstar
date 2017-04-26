#include "ModelList.h"

ModelList::ModelList()
{
}

ModelList::~ModelList()
{
	m_modelList.clear();
}

bool ModelList::Initialize(int numModels)
{
	return true;
}

void ModelList::Shutdown()
{
}

bool ModelList::Random(Assets* assets, int numModels, const char* filepath, int rangeX, int rangeZ, bool uniform)
{
	//Seed the random generator with the current time
	srand((unsigned int)time(NULL));

	for (int i = 0; i < numModels; i++)
	{
		Model model;

		model.Initialize(assets, filepath);
		m_modelList.push_back(model);

		float x = (-rangeX) + static_cast <float> (rand()) / (static_cast<float> (RAND_MAX / (rangeX + rangeX)));

		float z = (-rangeZ) + static_cast <float> (rand()) / (static_cast<float> (RAND_MAX / (rangeZ + rangeZ)));

		m_modelList[i].SetPosition(x, 1, z);
	}

	return true;
}

bool ModelList::AddModel(std::string filepath, Assets* assets)
{
	Model model;
	bool result = false;

	result = model.Initialize(assets, filepath.c_str());

	m_modelList.push_back(model);

	return result;
}

bool ModelList::RemoveModel(int index)
{
	m_modelList.erase(m_modelList.begin() + index);

	return true;
}

int ModelList::GetModelCount()
{
	return m_modelList.size();
}

Model * ModelList::GetModel(int index)
{
	return &m_modelList.at(index);
}

std::vector<Model> ModelList::GetModelList()
{
	return m_modelList;
}
