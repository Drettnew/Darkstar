#include "ForwardPlusCPURenderer.h"

ForwardPlusCPU::ForwardPlusCPU()
{
	m_Shader = 0;
}

ForwardPlusCPU::~ForwardPlusCPU()
{
}

bool ForwardPlusCPU::Initialize(ID3D11Device * device, ID3D11DeviceContext* deviceContext, HWND hwnd)
{
	bool result;

	// Create the light shader object.
	m_Shader = new ForwardPlusLightShader;
	if (!m_Shader)
	{
		return false;
	}

	// Initialize the texture shader object.
	result = m_Shader->Initialize(device, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	m_lightList.Random(50, 50, NUM_LIGHTS);

	m_StructuredLightBuffer.Initialize(NUM_LIGHTS, sizeof(Light), true, false, NULL, device);
	m_StructuredLightBuffer.InitializeResourceView(device);

	return true;
}

void ForwardPlusCPU::Shutdown()
{
	// Release the color shader object.
	if (m_Shader)
	{
		m_Shader->Shutdown();
		delete m_Shader;
		m_Shader = 0;
	}

	m_StructuredLightBuffer.Shutdown();
}

XMFLOAT4 ClipToView(XMFLOAT4 clip, XMMATRIX invProjectionMatrix)
{
	XMFLOAT4 view;
	XMStoreFloat4(&view, XMVector3Transform(XMLoadFloat4(&clip), invProjectionMatrix));

	view.x / view.w;
	view.y / view.w;
	view.z / view.w;

	return view;
}

XMFLOAT3 CreatePlaneEquation(XMFLOAT3 p0, XMFLOAT3 p1)
{
	return XMFLOAT3();
}

struct parameter
{
	XMMATRIX invProj;
};

unsigned int _stdcall myThread(LPVOID lpParameter)
{
	parameter param = *((parameter*)lpParameter);

	return 0;
}

bool ForwardPlusCPU::Render(D3D * directX, Camera * camera, Model* model)
{
	XMMATRIX viewMatrix, projectionMatrix;
	bool result;
	HRESULT hr;

	camera->GetViewMatrix(viewMatrix);
	directX->GetProjectionMatrix(projectionMatrix);

	m_lightList.UpdateLights(viewMatrix);

	XMMATRIX invProjMatrix = XMMatrixInverse(nullptr, projectionMatrix);

	XMFLOAT4 clip = XMFLOAT4(-1, 1, 1, 1);

	ClipToView(clip, invProjMatrix);

	parameter param;

	param.invProj = invProjMatrix;

	unsigned int myCounter = 0;
	DWORD myThreadID;
	HANDLE myHandle = (HANDLE)_beginthreadex(0, 0, &myThread , &param, 0, 0);

	WaitForSingleObject(myHandle, INFINITE);
	CloseHandle(myHandle);

	return true;
}
