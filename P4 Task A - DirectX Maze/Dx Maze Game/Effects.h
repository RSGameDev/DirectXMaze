//******************************************************************************************************************
// Title: Effects.h (Template for my effects header, based on and built upon with my own code. Specified in Task B)
// Author: Frank Luna 
// Date: 2012
// Page: 199 
// Book: Introduction to 3D Game Programming with Directx 11
//******************************************************************************************************************

#ifndef EFFECTS_H
#define EFFECTS_H

#include "d3dUtil.h"

#pragma region Effect
class Effect
{
public:
	Effect(ID3D11Device* device, const std::wstring& filename);
	virtual ~Effect();

private:
	Effect(const Effect& rhs);
	Effect& operator=(const Effect& rhs);

protected:
	ID3DX11Effect* mFX;
};
#pragma endregion

#pragma region BasicEffect
class BasicEffect : public Effect
{
public:
	BasicEffect(ID3D11Device* device, const std::wstring& filename);
	~BasicEffect();

	// Functions for building basic effects.
	void SetWorldViewProj(CXMMATRIX M) { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M) { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M) { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M) { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v) { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetDirLights(const DirectionalLight* lights) { DirLights->SetRawValue(lights, 0, 3 * sizeof(DirectionalLight)); }
	void SetMaterial(const Material& mat) { Mat->SetRawValue(&mat, 0, sizeof(Material)); }
	void SetDiffuseMap(ID3D11ShaderResourceView* tex) { DiffuseMap->SetResource(tex); }
	void SetCubeMap(ID3D11ShaderResourceView* tex) { CubeMap->SetResource(tex); }
		
	ID3DX11EffectTechnique* Light3Tech;					// Pointer to the lighting technique.
	ID3DX11EffectTechnique* Light3TexTech;				// Pointer to the lighting and texture technique.

	ID3DX11EffectMatrixVariable* WorldViewProj;			// Pointer to a world view projection effect matrix variable.
	ID3DX11EffectMatrixVariable* World;					// Pointer to a world effect matrix variable.
	ID3DX11EffectMatrixVariable* WorldInvTranspose;		// Pointer to a world inverse transpose effect matrix variable.
	ID3DX11EffectMatrixVariable* TexTransform;			// Pointer to a texture transform effect matrix variable.
	ID3DX11EffectVectorVariable* EyePosW;				// Pointer to an eye position world effect vector variable.
	ID3DX11EffectVariable* DirLights;					// Pointer to a directional lights effect variable.
	ID3DX11EffectVariable* Mat;							// Pointer to a materials effect variable.

	ID3DX11EffectShaderResourceVariable* DiffuseMap;
	ID3DX11EffectShaderResourceVariable* CubeMap;
};
#pragma endregion

#pragma region SkyEffect
class SkyEffect : public Effect
{
public:
	SkyEffect(ID3D11Device* device, const std::wstring& filename);
	~SkyEffect();

	void SetWorldViewProj(CXMMATRIX M) { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetCubeMap(ID3D11ShaderResourceView* cubemap) { CubeMap->SetResource(cubemap); }

	ID3DX11EffectTechnique* SkyTech;

	ID3DX11EffectMatrixVariable* WorldViewProj;

	ID3DX11EffectShaderResourceVariable* CubeMap;
};
#pragma endregion

#pragma region ParticleEffect
class ParticleEffect : public Effect
{
public:
	ParticleEffect(ID3D11Device* device, const std::wstring& filename);
	~ParticleEffect();

	// Functions for building particle systems.
	void SetViewProj(CXMMATRIX M) { ViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetGameTime(float f) { GameTime->SetFloat(f); }
	void SetTimeStep(float f) { TimeStep->SetFloat(f); }
	void SetEyePosW(const XMFLOAT3& v) { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetEmitPosW(const XMFLOAT3& v) { EmitPosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetEmitDirW(const XMFLOAT3& v) { EmitDirW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetTexArray(ID3D11ShaderResourceView* tex) { TexArray->SetResource(tex); }
	void SetRandomTex(ID3D11ShaderResourceView* tex) { RandomTex->SetResource(tex); }

	ID3DX11EffectTechnique* StreamOutTech;			// Pointer to the stream out technique.
	ID3DX11EffectTechnique* DrawTech;				// Pointer to the draw technique.

	ID3DX11EffectMatrixVariable* ViewProj;			// Pointer to a view projection effect matrix variable.
	ID3DX11EffectScalarVariable* GameTime;			// Pointer to a game time effect scalar variable.
	ID3DX11EffectScalarVariable* TimeStep;			// Pointer to a time step effect scalar variable.
	ID3DX11EffectVectorVariable* EyePosW;			// Pointer to an eye position world effect vector variable.
	ID3DX11EffectVectorVariable* EmitPosW;			// Pointer to an emit position world effect vector variable.
	ID3DX11EffectVectorVariable* EmitDirW;			// Pointer to an emit direction world effect vector variable.
	ID3DX11EffectShaderResourceVariable* TexArray;	// Pointer to a texture array shader resource variable.
	ID3DX11EffectShaderResourceVariable* RandomTex; // Pointer to a random texture shader resource variable.
};
#pragma endregion

#pragma region Effects
class Effects
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static BasicEffect* BasicFX;					// Pointer to a BasicFX 
	static SkyEffect* SkyFX;
	static ParticleEffect* ExplosionFX;				// Pointer to an ExplosionFX 
	static ParticleEffect* SmokeFX;					// Pointer to a SmokeFX 
	static ParticleEffect* SparkFX;					// Pointer to a SparkFX 
	static ParticleEffect* FireFX;					// Pointer to a FireFX 
};
#pragma endregion

#endif // EFFECTS_H