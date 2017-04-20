#include "ModelList.h"

ModelList::ModelList()
{
	m_ModelInfoList = 0;
}

ModelList::~ModelList()
{
}

bool ModelList::Initialize(int numModels)
{
	float red, green, blue;

	//Store number of models
	m_modelCount = numModels;

	//Create a list array of the model information
	m_ModelInfoList = new ModelInfoType[m_modelCount];
	if (!m_ModelInfoList)
	{
		return false;
	}

	//Seed the random generator with the current time
	srand((unsigned int)time(NULL));

	//Go through all the models and randomly generate model color and posiiton
	for (int i = 0; i < m_modelCount; i++)
	{
		//Renderate a random colorfo the model
		red = (float)rand() / RAND_MAX;
		green = (float)rand() / RAND_MAX;
		blue = (float)rand() / RAND_MAX;

		m_ModelInfoList[i].color = XMFLOAT4(red, green, blue, 1.0f);

		// Generate a random position in front of the viewer for the mode.
		m_ModelInfoList[i].positionX = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		m_ModelInfoList[i].positionY = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		m_ModelInfoList[i].positionZ = ((((float)rand() - (float)rand()) / RAND_MAX) * 10.0f) + 5.0f;
	}

	return true;
}

void ModelList::Shutdown()
{
	// Release the model information list.
	if (m_ModelInfoList)
	{
		delete[] m_ModelInfoList;
		m_ModelInfoList = 0;
	}

}

int ModelList::GetModelCount()
{
	return m_modelCount;
}

void ModelList::GetData(int index, float &positionX, float &positionY, float &positionZ, XMFLOAT4 &color)
{
	positionX = m_ModelInfoList[index].positionX;
	positionY = m_ModelInfoList[index].positionY;
	positionZ = m_ModelInfoList[index].positionZ;

	color = m_ModelInfoList[index].color;
}
