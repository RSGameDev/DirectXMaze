//**************************************************************************************************************************
// Title: Effects.cpp (Template for my effects class, based on and built upon with original code. Specified in Task B)
// Author: Frank Luna 
// Date: 2012
// Page: 199 
// Book: Introduction to 3D Game Programming with Directx 11
//**************************************************************************************************************************

#include "Effects.h"

#pragma region Effect

// Initialise the fx file for the project.
Effect::Effect(ID3D11Device* device, const std::wstring& filename)
	: mFX(0)
{
	// Standard C++ file input mechanisms
	std::ifstream fin(filename, std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();

	HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size,
		0, device, &mFX));
}

Effect::~Effect()
{
	ReleaseCOM(mFX);
}
#pragma endregion

#pragma region BasicEffect
BasicEffect::BasicEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	// Access the constant and non constant buffer variables to be used by program.
	Light3Tech = mFX->GetTechniqueByName("Light3");
	Light3TexTech = mFX->GetTechniqueByName("Light3Tex");

	WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	World = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	TexTransform = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	EyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();
	DirLights = mFX->GetVariableByName("gDirLights");
	Mat = mFX->GetVariableByName("gMaterial");
	DiffuseMap = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	CubeMap = mFX->GetVariableByName("gCubeMap")->AsShaderResource();
}

BasicEffect::~BasicEffect()
{
}
#pragma endregion

#pragma region SkyEffect
SkyEffect::SkyEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	SkyTech = mFX->GetTechniqueByName("SkyTech");
	WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	CubeMap = mFX->GetVariableByName("gCubeMap")->AsShaderResource();
}

SkyEffect::~SkyEffect()
{
}
#pragma endregion

#pragma region ParticleEffect
ParticleEffect::ParticleEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	// Access the constant and non constant buffer variables to be used by program.
	StreamOutTech = mFX->GetTechniqueByName("StreamOutTech");
	DrawTech = mFX->GetTechniqueByName("DrawTech");

	ViewProj = mFX->GetVariableByName("gViewProj")->AsMatrix();
	GameTime = mFX->GetVariableByName("gGameTime")->AsScalar();
	TimeStep = mFX->GetVariableByName("gTimeStep")->AsScalar();
	EyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();
	EmitPosW = mFX->GetVariableByName("gEmitPosW")->AsVector();
	EmitDirW = mFX->GetVariableByName("gEmitDirW")->AsVector();
	TexArray = mFX->GetVariableByName("gTexArray")->AsShaderResource();
	RandomTex = mFX->GetVariableByName("gRandomTex")->AsShaderResource();
}

ParticleEffect::~ParticleEffect()
{
}
#pragma endregion

#pragma region Effects

BasicEffect* Effects::BasicFX = 0;									// Initialise the basic effect pointer to null.
SkyEffect*     Effects::SkyFX = 0;
ParticleEffect* Effects::ExplosionFX = 0;							// Initialise the explosion particle effect pointer to null.
ParticleEffect* Effects::SmokeFX = 0;								// Initialise the smoke particle effect pointer to null.
ParticleEffect* Effects::SparkFX = 0;								// Initialise the spark particle effect pointer to null.
ParticleEffect* Effects::FireFX = 0;								// Initialise the fire particle effect pointer to null.

void Effects::InitAll(ID3D11Device* device)
{
	BasicFX = new BasicEffect(device, L"FX/Basic.fxo");				// Create an instance of the effect using the proceeding fxo file. 	
	SkyFX = new SkyEffect(device, L"FX/Sky.fxo");
	ExplosionFX = new ParticleEffect(device, L"FX/Explosion.fxo");  // Create an instance of the effect using the proceeding fxo file.
	SmokeFX = new ParticleEffect(device, L"FX/Smoke.fxo");			// Create an instance of the effect using the proceeding fxo file.
	SparkFX = new ParticleEffect(device, L"FX/Spark.fxo");			// Create an instance of the effect using the proceeding fxo file.
	FireFX = new ParticleEffect(device, L"FX/Fire.fxo");			// Create an instance of the effect using the proceeding fxo file.
}

void Effects::DestroyAll()
{
	SafeDelete(BasicFX);
	SafeDelete(SkyFX);
	SafeDelete(ExplosionFX);
	SafeDelete(SmokeFX);
	SafeDelete(SparkFX);
	SafeDelete(FireFX);
}
#pragma endregion