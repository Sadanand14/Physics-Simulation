#pragma once
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")
#include "Mesh.h"
#include <d3dcompiler.h>
//#include "Game.h"
#include"types.h"

	using namespace DirectX;

class Camera 
{
	XMFLOAT4X4 viewMatrix, projectionMatrix;
	XMVECTOR upUnit;
	XMFLOAT3 currentPos;
	XMFLOAT4 rotation;
	float xRot, yRot,delta;
public:
	//Camera() {};
	Camera(float width, float height);
	~Camera();

	void Update(float deltaTime);
	inline XMFLOAT4X4 GetView() { return viewMatrix; }
	inline XMFLOAT4X4 GetProjection() { return projectionMatrix; }
	inline XMFLOAT3 GetPosition() { return currentPos; };

	void UpdateViewMatrix();
	void Rotate(float x , float y);
	void MoveRelative(float x, float y, float z);
	void MoveAbsolute(float x, float y, float z);
};