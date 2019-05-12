//**************************************************************************************************************************
// Title: Vertex.cpp (Template for my vertex class, based on and built upon with additional code. Specified in Task B)
// Author: Frank Luna 
// Date: 2012
// Page: 199 
// Book: Introduction to 3D Game Programming with Directx 11
//**************************************************************************************************************************

#include "Vertex.h"
#include "Effects.h"

#pragma region InputLayoutDesc

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::Pos[1] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

// Create the vertex input layouts.
const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::Basic32[3] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
};
const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::Particle[5] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "AGE", 0, DXGI_FORMAT_R32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TYPE", 0, DXGI_FORMAT_R32_UINT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

#pragma endregion

#pragma region InputLayouts

ID3D11InputLayout* InputLayouts::Pos = 0;
ID3D11InputLayout* InputLayouts::Basic32 = 0;
ID3D11InputLayout* InputLayouts::Particle = 0;

void InputLayouts::InitAll(ID3D11Device* device)
{
	
	// Create the input layout
	D3DX11_PASS_DESC passDesc;

	//
	// Pos
	//
	Effects::SkyFX->SkyTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(InputLayoutDesc::Pos, 1, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &Pos));

	// Basic32
	Effects::BasicFX->Light3Tech->GetPassByIndex(0)->GetDesc(&passDesc);
	// Create the Input Layput. Using the above defined structure, number of elements it contains, the input signature
	// of the vertex shader, the byte size for this signature, the pointer for which the input layout will use.
	HR(device->CreateInputLayout(InputLayoutDesc::Basic32, 3, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &Basic32));

	// Particle
	// Create the input layout
	Effects::ExplosionFX->StreamOutTech->GetPassByIndex(0)->GetDesc(&passDesc);
	// Create the Input Layout. Using the above defined structure, number of elements it contains, the input signature
	// of the vertex shader, the byte size for this signature, the pointer for which the input layout will use.
	HR(device->CreateInputLayout(InputLayoutDesc::Particle, 5, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &Particle));
}

void InputLayouts::DestroyAll()
{
	ReleaseCOM(Pos);
	ReleaseCOM(Basic32);
	ReleaseCOM(Particle);
}

#pragma endregion
