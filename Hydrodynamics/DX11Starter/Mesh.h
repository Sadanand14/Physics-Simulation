#pragma once
#include "Vertex.h"
#include "types.h"

//creating mesh class
class Mesh
{
	ID3D11Buffer *vertexPointer = nullptr, *indexPointer = nullptr;
	int indexCount=NULL;
	//Vertex*VertexArr=nullptr;
	//unsigned int* indexarr=nullptr;
public: 
	template <typename T>
	Mesh(T* vertextArray, unsigned int * intArray, int totalVertices, int totalIndices, ID3D11Device* device);
	
	Mesh(const char* objFile, ID3D11Device* device);
	~Mesh();
	
	ID3D11Buffer* GetVertexBuffer() { return vertexPointer; }
	ID3D11Buffer* GetIndexBuffer() { return indexPointer; }
	int GetIndexCount() { return indexCount; }

	template <typename T>
	void CreatingBuffer(T* vertextArray, unsigned int* intArray, int totalVertices, int totalIndices, ID3D11Device* device);
};

template<typename T>
void Mesh::CreatingBuffer(T* vertextArray, unsigned int* intArray, int totalVertices, int totalIndices, ID3D11Device* device)
{
	indexCount = totalIndices;
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(T) * totalVertices;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA initialVertexData;
	initialVertexData.pSysMem = vertextArray;
	device->CreateBuffer(&vbd, &initialVertexData, &vertexPointer);

	//creating buffer for the indices
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(int) * totalIndices;// I modified this so that I wouldn't need to define 4 separate meshes to create 4 objects         
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA initialIndexData;
	initialIndexData.pSysMem = intArray;
	device->CreateBuffer(&ibd, &initialIndexData, &indexPointer);

}

template<typename T>
Mesh::Mesh(T* vertextArray, unsigned int* intArray, int totalVertices, int totalIndices, ID3D11Device* device)
{
	//creating buffer for the vertices
	CreatingBuffer(vertextArray, intArray, totalVertices, totalIndices, device);
}