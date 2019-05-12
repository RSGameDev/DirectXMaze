//**************************************************************************************************************************
// Title: RenderStates.h (Original code reduced by approximately 80% to contain only relevant code for this project.) 
// Author: Frank Luna 
// Date: 2012
// Page: 332 
// Book: Introduction to 3D Game Programming with Directx 11
//**************************************************************************************************************************

#ifndef RENDERSTATES_H
#define RENDERSTATES_H

#include "d3dUtil.h"

class RenderStates
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();
	
	// Blend states
	static ID3D11BlendState* TransparentBS;

	// Depth/stencil states
	static ID3D11DepthStencilState* NoDoubleBlendDSS;
};

#endif // RENDERSTATES_H