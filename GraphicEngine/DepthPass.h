#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <fstream>
#include <DirectXMath.h>

using namespace DirectX;
using namespace std;

class DepthPass
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};
public:
	DepthPass();
	~DepthPass();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd);
	void Shutdown();

	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix);

private:

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

	bool InitializeShader(ID3D11Device* device, HWND hwnd);
	bool InitializeDepthStencilView(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);

	void ShutdownDepthStencilView();

	ID3D11Texture2D* m_depthTexture;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11ShaderResourceView* m_resourceView;

	ID3D11VertexShader* m_vertexShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;

};