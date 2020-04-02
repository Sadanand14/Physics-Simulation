#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include "Mesh.h"
#include "Entity.h"
#include "Camera.h"
#include "Lights.h"
#include "Textures.h"
#include "Materials.h";
#include "types.h"
#include "Container.h"
#include "GpuEmitter.h"

class Game
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	//Particle Stuff
	GPUEmitter* emitterGpu = nullptr;
	//Water Stuff
	Materials* material = nullptr;

	//General Stuff
	Camera * camera = nullptr;
	std::vector<Entity*> entityList;
	std::map<std::string, Mesh*> meshMap;
	std::map<std::string, Texture*> texMap;

	Container* m_container;
	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	// Overridden mouse input helper methods
	void OnMouseDown(WPARAM buttonState, int x, int y);
	void OnMouseUp(WPARAM buttonState, int x, int y);
	void OnMouseMove(WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta, int x, int y);

private:
	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders();
	void CreateMatrices();
	void LoadModelDirectory();
	void LoadTextureDirectory();
	void RenderSky();
	void DrawContainer(DirectX::XMFLOAT3* cornerArry);

	// Buffers to hold actual geometry data
	ID3D11Buffer* vertexBuffer = nullptr, * indexBuffer = nullptr;

	// Wrappers for DirectX shaders to provide simplified functionality
	//sky shaders
	SimpleVertexShader* SkyVS = nullptr;
	SimplePixelShader* SkyPS = nullptr;
	//basic entity shaders
	SimpleVertexShader* vertexShader = nullptr;
	SimplePixelShader* pixelShader = nullptr;
	
	//GpuParticleStuff
	SimpleComputeShader * fluidUpdateCS = nullptr,*particleUpdateCS = nullptr, * particleEmitCS = nullptr;
	SimpleComputeShader* collisionUpdateCS = nullptr, * finalUpdateCS = nullptr;;
	SimpleVertexShader* gpuParticleVS = nullptr;
	SimplePixelShader* gpuParticlePS = nullptr;

	// The matrices to go from model space to screen space
	DirectX::XMFLOAT4X4 worldMatrix, viewMatrix, projectionMatrix;

	//RasterStates
	ID3D11RasterizerState* skyRS = nullptr;

	//DepthStates
	ID3D11DepthStencilState* skyDS = nullptr;

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;

	int randomNumber, drawcount = 0;
};


