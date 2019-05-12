//**************************************************************************************************************************
// Title: TextureMgr.h 
// Author: Frank Luna 
// Date: 2012
// Page: 542 
// Book: Introduction to 3D Game Programming with Directx 11
//**************************************************************************************************************************

#ifndef TEXTUREMGR_H
#define TEXTUREMGR_H

#include "d3dUtil.h"
#include <map>

///<summary>
/// Simple texture manager to avoid loading duplicate textures from file.  That can
/// happen, for example, if multiple meshes reference the same texture filename. 
///</summary>
class TextureMgr
{
public:
	TextureMgr();
	~TextureMgr();

	void Init(ID3D11Device* device);

	ID3D11ShaderResourceView* CreateTexture(std::wstring filename);

private:
	TextureMgr(const TextureMgr& rhs);
	TextureMgr& operator=(const TextureMgr& rhs);
	
private:
	ID3D11Device* md3dDevice;
	std::map<std::wstring, ID3D11ShaderResourceView*> mTextureSRV;
};

#endif // TEXTUREMGR_H