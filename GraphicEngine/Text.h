#pragma once
#include "Font.h"
#include "FontShader.h"

class Text
{
private:
	struct SentenceType
	{
		ID3D11Buffer *vertexBuffer, *indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

	const static int maxSentences = 10;
	const static int maxLength = 32;

	Font* m_Font;
	FontShader* m_FontShader;
	int m_screenWidth, m_screenHeight;
	XMMATRIX m_baseViewMatrix;

	SentenceType* m_sentencesList[maxSentences];
	bool m_sentencesUsed[maxSentences];

	bool InitializeSentence(SentenceType** sentence, ID3D11Device* device);
	bool UpdateSentence(SentenceType* sentence, const char* text, int positionX, int positionY, float red, float green, float blue,
		ID3D11DeviceContext* deviceContext);
	void ReleaseSentence(SentenceType** sentence);
	bool RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, XMMATRIX worldMatrix,
		XMMATRIX orthoMatrix);
public:
	Text();
	~Text();

	bool Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext, HWND hwnd, int screenWidth, int screenHeight, Assets* assets);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX orthoMatrix);

	bool QueueString(std::string text, float x, float y, float red, float green, float blue, ID3D11DeviceContext* deviceContext);

	bool SetFps(int, ID3D11DeviceContext* deviceContext);
};