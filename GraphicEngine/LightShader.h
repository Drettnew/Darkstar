#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <fstream>
#include <time.h>
#include <stdlib.h>

using namespace DirectX;
using namespace std;

class LightShader
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding;
	};

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
			, m_Range(100.0f)
			, m_Intensity(1.0f)
			, m_Enabled(true)
			, m_Type(LightType::Point)
		{}
	};

	struct LightBufferType
	{
		Light light[1];
	};

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_cameraBuffer;

	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT3 cameraPosition);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);
public:
	LightShader();
	LightShader(const LightShader&);
	~LightShader();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT3 cameraPosition);
};