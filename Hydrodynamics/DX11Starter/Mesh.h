#pragma once
#include "Vertex.h"
#include "types.h"

//creating mesh class
class Mesh
{
	ID3D11Buffer *vertexPointer = nullptr, *indexPointer = nullptr;
	int indexCount=NULL;
	unsigned int vertexCount;
	float width;
	//Vertex*VertexArr=nullptr;
	//unsigned int* indexarr=nullptr;
public: 
	
	Mesh(Vertex* vertextArray, unsigned int * intArray, int totalVertices, int totalIndices, ID3D11Device* device);
	
	Mesh(const char* objFile, ID3D11Device* device);
	~Mesh();
	
	inline float GetWidth() { return width; }
	inline unsigned int GetVertexCount()
	{
		return vertexCount; 
	}
	ID3D11Buffer* GetVertexBuffer() { return vertexPointer; }
	ID3D11Buffer* GetIndexBuffer() { return indexPointer; }
	int GetIndexCount() { return indexCount; }

	
	void CreatingBuffer(Vertex* vertextArray, unsigned int* intArray, int totalVertices, int totalIndices, ID3D11Device* device);
};

