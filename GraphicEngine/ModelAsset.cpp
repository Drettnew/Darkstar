#include "ModelAsset.h"

struct ObjTriVertex
{
	int iPos;
	int iNormal;
	int iTex;

	void Init() { iPos = iNormal = iTex = -1; }
};

struct ObjTriangle
{
	ObjTriVertex vertex[3];
	void Init() { vertex[0].Init(); vertex[1].Init(); vertex[2].Init(); }
};

typedef std::vector<ObjTriangle> objTriangleList;

void AddObjFace(objTriangleList& objTriangleList, const ObjMesh& objMesh,
	int objFaceIndex)
{
	const ObjMesh::Face& objFace = objMesh.faces[objFaceIndex];
	unsigned int triCount = objFace.numVertices - 2;

	for (INT fv = 2; fv < objFace.numVertices; fv++)
	{
		ObjTriangle tri;
		tri.Init();

		tri.vertex[0].iPos = objMesh.faceVertices[objFace.firstVertex];
		tri.vertex[1].iPos = objMesh.faceVertices[objFace.firstVertex + fv - 1];
		tri.vertex[2].iPos = objMesh.faceVertices[objFace.firstVertex + fv];


		if (!objMesh.normals.empty() && objFace.firstNormal >= 0)
		{
			tri.vertex[0].iNormal = objMesh.faceNormals[objFace.firstNormal];
			tri.vertex[1].iNormal = objMesh.faceNormals[objFace.firstNormal + fv - 1];
			tri.vertex[2].iNormal = objMesh.faceNormals[objFace.firstNormal + fv];
		}

		if (!objMesh.texCoords.empty() && objFace.firstTexCoord >= 0)
		{
			tri.vertex[0].iTex = objMesh.faceTexCoords[objFace.firstTexCoord];
			tri.vertex[1].iTex = objMesh.faceTexCoords[objFace.firstTexCoord + fv - 1];
			tri.vertex[2].iTex = objMesh.faceTexCoords[objFace.firstTexCoord + fv];
		}

		objTriangleList.push_back(tri);
	}
}

bool ModelAsset::InitializeBuffers(ID3D11Device *device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	objTriangleList triList;
	triList.reserve(mesh.numTriangles);



	for (int i = 0; i < mesh.faces.size(); i++)
	{
		AddObjFace(triList, mesh, i);
	}

	
	triangleCount = triList.size();
	m_vertexCount = (triangleCount * 3);
	m_indexCount = m_vertexCount;

	//Create the vertex array
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	//Create the index array
	indices = new unsigned long[m_vertexCount];
	if (!indices)
	{
		return false;
	}

	int index = 0;

	for (int i = 0; i < triList.size(); i++)
	{
		ObjTriangle& tri = triList[i];

		for (int j = 0; j < 3; j++)
		{
			vertices[index].position = mesh.vertices[tri.vertex[j].iPos];
			vertices[index].texture = mesh.texCoords[tri.vertex[j].iTex];
			vertices[index].normal = mesh.normals[tri.vertex[j].iNormal];

			indices[index] = index;
			index++;
		}
	}

	////Load the vertex array and index array with data
	//for (int i = 0; i < (mesh.faces.size() * 3); i++)
	//{
	//	vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
	//	vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
	//	vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

	//	indices[i] = i;
	//}
	

	//Set up the description of the static vertex buffer
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//Give the subresource structure a pointer to the vertex data
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//Now create the vertex buffer
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//Set up the description of the static index buffer
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//Give the subsource structure a pointer to the index data
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//Create the index buffer
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//Release the arrays now that the vertex and index buffers have been created and loaded
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	mesh.Free();

	return true;
}

void ModelAsset::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool ModelAsset::LoadModel(const char * modelname)
{
	return LoadObj(modelname, &mesh);
}

ModelAsset::ModelAsset()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}


ModelAsset::~ModelAsset()
{
}

//bool ModelAsset::load(std::string path, Assets * assets)
//{
//	bool result;
//
//	//Initialize the model data
//	result = LoadModel(path.c_str());
//	if (!result)
//	{
//		return false;
//	}
//
//	//Initialize the vertex and index buffers
//	result = InitializeBuffers(assets->GetDevice());
//	if (!result)
//	{
//		return false;
//	}
//
//	return true;
//}
//
//void ModelAsset::unload()
//{
//	//Release the index buffer
//	if (m_indexBuffer)
//	{
//		m_indexBuffer->Release();
//		m_indexBuffer = 0;
//	}
//	//Release the vertex buffer
//	if (m_vertexBuffer)
//	{
//		m_vertexBuffer->Release();
//		m_vertexBuffer = 0;
//	}
//}
//
//void ModelAsset::upload()
//{
//	
//}

bool ModelAsset::LoadModel(std::string path, ID3D11Device* device)
{
	bool result;

	//Initialize the model data
	result = LoadModel(path.c_str());
	if (!result)
	{
		return false;
	}

	//Initialize the vertex and index buffers
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}

bool ModelAsset::Shutdown()
{
	return false;
}

void ModelAsset::Render(ID3D11DeviceContext *deviceContext)
{
	//Put the vertex and index buffers on the graphics pipeline to prepare them for drawing
	RenderBuffers(deviceContext);
}

int ModelAsset::GetIndexCount()
{
	return m_indexCount;
}

ObjMesh * ModelAsset::GetMesh()
{
	return &mesh;
}
