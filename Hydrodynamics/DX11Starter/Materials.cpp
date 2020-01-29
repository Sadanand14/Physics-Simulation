#include "Game.h"
#include "Vertex.h"
#include "d3d11.h"
#include "Mesh.h"
#include <vector>
#include "Materials.h"

Materials::Materials(SimpleVertexShader* a, SimplePixelShader* b)
{
	vShader = a;
	pShader = b;
}

Materials::~Materials() 
{

}