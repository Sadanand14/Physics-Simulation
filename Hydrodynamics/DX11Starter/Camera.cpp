#include "Game.h"
#include "Vertex.h"
#include "d3d11.h"
#include "Mesh.h"
#include <vector>
#include "Camera.h"
#include <iostream>

using namespace DirectX;

Camera::Camera(float width,float height) 
{
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,		// Field of View Angle
		width / height,				// Aspect ratio
		0.1f,						// Near clip plane distance
		500.0f);					// Far clip plane distance

	
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P));
	upUnit = XMVectorSet(0, 1, 0, 0);
	XMStoreFloat3(&currentPos, XMVectorSet(0, 0, -5, 0));
	xRot = 0;
	yRot = 0;
	XMStoreFloat4(&rotation, XMQuaternionIdentity());
}

Camera::~Camera() {	}

void Camera::Update(float dt)
{
	float speed = dt * 3;

	if (GetAsyncKeyState(VK_SHIFT)) { speed *= 5; }
	if (GetAsyncKeyState(VK_CONTROL)) { speed *= 0.1f; }

	// Movement
	if (GetAsyncKeyState('W') & 0x8000) { MoveRelative(0, 0, speed); }
	if (GetAsyncKeyState('S') & 0x8000) { MoveRelative(0, 0, -speed); }
	if (GetAsyncKeyState('A') & 0x8000) { MoveRelative(-speed, 0, 0); }
	if (GetAsyncKeyState('D') & 0x8000) { MoveRelative(speed, 0, 0); }
	if (GetAsyncKeyState('X') & 0x8000) { MoveAbsolute(0, -speed, 0); }
	if (GetAsyncKeyState(' ') & 0x8000) { MoveAbsolute(0, speed, 0); }

	// Check for reset
	if (GetAsyncKeyState('R') & 0x8000)
	{
		XMStoreFloat3(&currentPos, XMVectorSet(0, 0, -5, 0));
		xRot = 0;
		yRot = 0;
		XMStoreFloat4(&rotation, XMQuaternionIdentity());
	}

	// Update the view every frame - could be optimized
	UpdateViewMatrix();
}


void Camera::UpdateViewMatrix() 
{
	XMVECTOR direction = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), XMLoadFloat4(&rotation));

	XMMATRIX view = XMMatrixLookToLH(XMLoadFloat3(&currentPos), direction,upUnit);

	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(view));
}

void Camera::Rotate(float x, float y) 
{
	xRot += x;
	yRot += y;

	xRot = max(min(xRot, XM_PIDIV2), -XM_PIDIV2);

	XMStoreFloat4(&rotation, XMQuaternionRotationRollPitchYaw(xRot, yRot, 0.0f));
}

void Camera::MoveRelative(float x, float y, float z)
{
	XMVECTOR direction = XMVector3Rotate(XMVectorSet(x, y, z, 0), XMLoadFloat4(&rotation));

	XMStoreFloat3(&currentPos, XMLoadFloat3(&currentPos) + direction);
}

void Camera::MoveAbsolute(float x, float y, float z)
{
	currentPos.x += x;
	currentPos.y += y;
	currentPos.z += z;

}