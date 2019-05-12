//*******************************************************************************************************************
// Title: Vertex.h (Template for my vertex header, based on and built upon with additional code. Specified in Task B)
// Author: Frank Luna 
// Date: 2012
// Page: 199 
// Book: Introduction to 3D Game Programming with Directx 11
//*******************************************************************************************************************

#ifndef VERTEX_H
#define VERTEX_H

#include "d3dUtil.h"

namespace Vertex
{
	// Structures to be used in the input layout
	struct Basic32
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;
	};
	struct Particle
	{
		XMFLOAT3 InitialPos;
		XMFLOAT3 InitialVel;
		XMFLOAT2 Size;
		float Age;
		unsigned int Type;
	};
}

class InputLayoutDesc
{
public:
	// The input layout arrays to be used in the project.
	static const D3D11_INPUT_ELEMENT_DESC Pos[1];
	static const D3D11_INPUT_ELEMENT_DESC Basic32[3];
	static const D3D11_INPUT_ELEMENT_DESC Particle[5];
};

class InputLayouts
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static ID3D11InputLayout* Pos;
	static ID3D11InputLayout* Basic32;
	static ID3D11InputLayout* Particle;
};

#endif // VERTEX_H
