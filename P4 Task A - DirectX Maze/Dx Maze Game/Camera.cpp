//**************************************************************************************************************************
// Title: Camera.cpp (Original code reduced by approximately 50% to contain only relevant code for this project.) 
// Author: Frank Luna 
// Date: 2012
// Page: 430 
// Book: Introduction to 3D Game Programming with Directx 11
//**************************************************************************************************************************

#include "Camera.h"

// The constructor for the Camera. Initialising the different variables.
Camera::Camera()
	: mPosition(0.0f, 0.0f, 0.0f), 
	  mRight(1.0f, 0.0f, 0.0f),
	  mUp(0.0f, 1.0f, 0.0f),
	  mLook(0.0f, 0.0f, 1.0f)
{
	SetLens(0.25f*MathHelper::Pi, 1.0f, 1.0f, 1000.0f);
}

// The deconstructor.
Camera::~Camera()
{
}

// This function retrieves the position for the camera.
XMFLOAT3 Camera::GetPosition()const
{
	return mPosition;
}

// This function takes the parameters and builds a projection matrix from them.
void Camera::SetLens(float fovY, float aspect, float zn, float zf)
{
	// cache properties
	mFovY = fovY;
	mAspect = aspect;
	mNearZ = zn;
	mFarZ = zf;

	mNearWindowHeight = 2.0f * mNearZ * tanf( 0.5f*mFovY );
	mFarWindowHeight  = 2.0f * mFarZ * tanf( 0.5f*mFovY );

	XMMATRIX P = XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
	XMStoreFloat4x4(&mProj, P);
}

// This function adjusts the camera so it is focused on a particular target.
// Taking in vector values via the parameters.
void Camera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
{
	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
	XMVECTOR U = XMVector3Cross(L, R);

	XMStoreFloat3(&mPosition, pos);
	XMStoreFloat3(&mLook, L);
	XMStoreFloat3(&mRight, R);
	XMStoreFloat3(&mUp, U);
}

// This function adjusts the camera so it is focused on a particular target. 
// Taking in xmfloat3 values via the parameters.
void Camera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
	XMVECTOR P = XMLoadFloat3(&pos);
	XMVECTOR T = XMLoadFloat3(&target);
	XMVECTOR U = XMLoadFloat3(&up);

	LookAt(P, T, U);
}

// This function retrieves the view matrix for the camera.
XMMATRIX Camera::View()const
{
	return XMLoadFloat4x4(&mView);
}

// This function retrieves the projection matrix for the camera.
XMMATRIX Camera::Proj()const
{
	return XMLoadFloat4x4(&mProj);
}

// This function retrieves the view projection matrix for the camera, through use of a multiplication function.
XMMATRIX Camera::ViewProj()const
{
	return XMMatrixMultiply(View(), Proj());
}

// This function adjusts the camera to be positioned correctly and to update the view transformation matrix.
void Camera::UpdateViewMatrix()
{
	XMVECTOR R = XMLoadFloat3(&mRight);
	XMVECTOR U = XMLoadFloat3(&mUp);
	XMVECTOR L = XMLoadFloat3(&mLook);
	XMVECTOR P = XMLoadFloat3(&mPosition);

	// Keep camera's axes orthogonal to each other and of unit length.
	L = XMVector3Normalize(L);
	U = XMVector3Normalize(XMVector3Cross(L, R));

	// U, L already ortho-normal, so no need to normalize cross product.
	R = XMVector3Cross(U, L); 

	// Fill in the view matrix entries.
	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));

	XMStoreFloat3(&mRight, R);
	XMStoreFloat3(&mUp, U);
	XMStoreFloat3(&mLook, L);

	mView(0,0) = mRight.x; 
	mView(1,0) = mRight.y; 
	mView(2,0) = mRight.z; 
	mView(3,0) = x;   

	mView(0,1) = mUp.x;
	mView(1,1) = mUp.y;
	mView(2,1) = mUp.z;
	mView(3,1) = y;  

	mView(0,2) = mLook.x; 
	mView(1,2) = mLook.y; 
	mView(2,2) = mLook.z; 
	mView(3,2) = z;   

	mView(0,3) = 0.0f;
	mView(1,3) = 0.0f;
	mView(2,3) = 0.0f;
	mView(3,3) = 1.0f;
}


