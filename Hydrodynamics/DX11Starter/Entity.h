#pragma once
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")
#include "Mesh.h"
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include "Game.h"
#include "Materials.h"
#include "Lights.h"
#include "types.h"

using namespace DirectX;
//creating an enitity class

class Entity 
{


	XMFLOAT4X4 m_translation, m_rotation, m_scaling; // Guess this is pretty obvious
	std::string m_modelTitle;
	Materials* m_material;



public:
	static std::vector<Mesh> m_meshList;

	Entity(XMMATRIX a, XMMATRIX b, XMMATRIX c, std::string, Materials* e);
	~Entity();
	
	void SetPos(XMMATRIX X) { XMStoreFloat4x4(&m_translation,X); }
	void SetRot(XMMATRIX Y) { XMStoreFloat4x4(&m_rotation,Y); }
	void SetScale(XMMATRIX Z) { XMStoreFloat4x4(&m_scaling,Z); }
	XMFLOAT4X4 GetPos(){ return m_translation; }
	XMFLOAT4X4 GetRot(){ return m_rotation; }
	XMFLOAT4X4 GetScale(){ return m_scaling; }
	std::string GetTitle() { return m_modelTitle; }

	XMMATRIX GetWM(); // returns a world matrix for storing in the worldMatrix variable
};