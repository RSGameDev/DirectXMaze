//**************************************************************************************************************************
// Title: color.fx
// Author: Frank Luna 
// Date: 2012
// Page: Chapter 6, Shape demo
// Book: Introduction to 3D Game Programming with Directx 11
//**************************************************************************************************************************

// Constant buffer
cbuffer cbPerObject
{
	float4x4 gWorldViewProj;	// Word view and projection variable.
};

// Vertex fed into the colour fx file.
struct VertexIn
{
	float3 Pos   : POSITION;	// Position of the vertices
	float4 Colour : COLOUR;		// Colour of the vertices
};

// Vertex fed out of the colour fx file.
struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float4 Colour : COLOUR;
};

// Vertex coming out of the Vertex Shader.
VertexOut VS(VertexIn vin)
{
	VertexOut vout;				//  Variable for the VertexOut structure
	
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.Pos, 1.0f), gWorldViewProj);
	
	// Just pass vertex color into the pixel shader.
    vout.Colour = vin.Colour;
    
    return vout;				// Return the vout structure.
}

// Particle Shader for colouring the vertices.
float4 PS(VertexOut pin) : SV_Target
{
    return pin.Colour;			// Returns the colour for the vertercies.
}

// The technique that will be called from the main class to execute this effect.
technique11 ColourTech
{
	// Number of passes.
    pass P0
    {
		// Set the Vertex Shader using the following parametres, the VS function inside of this fx file.
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		// A Geometry Shader will not be applicable here.
		SetGeometryShader( NULL );
		// Set the Vertex Shader using the following parametres, the PS function inside of this fx file..
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}

