//**************************************************************************************************************************
// Title: ShapesDemo.cpp (Template for my main class of which has been expanded upon using my own code. Specified in Task B)
// Author: Frank Luna 
// Date: 2012
// Page: 227 
// Book: Introduction to 3D Game Programming with Directx 11
//**************************************************************************************************************************

#include "Dx Maze Game.h"

// The main function for Windows, to initialise it for the project.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	MazeGameMain theApp(hInstance);		

	// Checks if the game has not initialised.
	if (!theApp.Init())					
		return 0;																// Registers as false

	return theApp.Run();														// The game will execute the run function
}

// The constructor for the Game. Initialising the different variables.
MazeGameMain::MazeGameMain(HINSTANCE hInstance)
	: D3DApp(hInstance), mSky(0), mVertexBuffer(0), mIndexBuffer(0),
	mEffect(0), mTechnique(0),
	mFloorMapSRV(0), mWallMapSRV(0), mLiftMapSRV(0), mPoleMapSRV(0), mPlayerMapSRV(0), mEnemyMapSRV(0),
	mRandomTexSRV(0), mExplosionTexSRV(0), mSmokeTexSRV(0), mSparkTexSRV(0), mFireTexSRV(0),
	mEffectWorldViewProj(0), mInputLayout(0), mPlayerPosition(mPlayerSpawnLevel1), mLiftPosition(mLiftLevel1), mPole2Position(mEndOfGame),
	mTheta(1.5f*MathHelper::Pi), mPhi(0.1f*MathHelper::Pi), mRadius(10.0f)
{
	mMainWndCaption = L"DirectX Maze Game";										// The label in the windows title bar.

	mGraphLevel1 = new Graph(11, 11, 1);										// A graph object that will create am 11 by 11 size maze.
	mGraphLevel2 = new Graph(13, 13, 1);

	mEnemyLevel1 = new Enemy(mGraphLevel1, 1);									// An enemy object that uses the graph to navigate around the maze.
	mEnemy1Level2 = new Enemy(mGraphLevel2, 2);
	mEnemy2Level2 = new Enemy(mGraphLevel2, 3);

	mWallPositionLevel1 = mGraphLevel1->StoreWallPosition();					// Level 1 wall positions for the wall world matrice to use.
	mWallPositionLevel2 = mGraphLevel2->StoreWallPosition();					// Level 2 wall positions for the wall world matrice to use.

	// Initialise the mouse position.
	mLastMousePos.x = 0;												
	mLastMousePos.y = 0;
	
	XMMATRIX I = XMMatrixIdentity();											// Create an identity matrix.
	XMStoreFloat4x4(&mFloorWorldLevel1, I);										// Assign the floor matrix to here.
	XMStoreFloat4x4(&mTexTransform, I);											// Assign the texture transform matrix to here.
	XMStoreFloat4x4(&mView, I);													// Assign the view matrix to here.
	XMStoreFloat4x4(&mProj, I);													// Assign the projection matrix to here.
	
	// For every wall position in the level 1 wall position vector, place a wall mesh at the position.
	for (int i = 0; i<mWallPositionLevel1.size(); i++){
		XMFLOAT4X4 temp;
		mWallWorldLevel1.push_back(temp);
		XMStoreFloat4x4(&mWallWorldLevel1[i], XMMatrixTranslation(mWallPositionLevel1[i].x, 0.5f, mWallPositionLevel1[i].z));
	}
	// For every wall position in the level 2 wall position vector, place a wall mesh at the position. Due to the 2nd level positioning the x and z vectors needed offsetting by 6.0 and 4.0.
	for (int i = 0; i<mWallPositionLevel2.size(); i++){
		XMFLOAT4X4 temp;
		mWallWorldLevel2.push_back(temp);
		XMStoreFloat4x4(&mWallWorldLevel2[i], XMMatrixTranslation(mWallPositionLevel2[i].x + 6.0f, 14.5f, mWallPositionLevel2[i].z + 4.0f));
	}

	// Position the wall, lift, pole and enemy matrix to the translations stated below.
	XMStoreFloat4x4(&mFloorWorldLevel1, XMMatrixTranslation(5.0f, 0.0f, 5.0f));
	XMStoreFloat4x4(&mFloorWorldLevel2, XMMatrixTranslation(12.0f, 14.0f, 10.0f));
	XMStoreFloat4x4(&mPoleWorldLevel1, XMMatrixTranslation(9.0f, 0.75f, 9.0f));
	XMStoreFloat4x4(&mPoleWorldLevel2, XMMatrixTranslation(7.0f, 14.75f, 15.0f));

	XMMATRIX FloorTexScale = XMMatrixScaling(3.0f, 3.0f, 0.0f);					 // This allows the floor texture to appear matching to the floor mesh size.
	XMStoreFloat4x4(&mFloorTexTransform, FloorTexScale);						 // Add the texture scale to the floor texture transform matrix.

	// Directional Lighting
	mDirectionalLight[0].Ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);			 //Initialise the ambient values.
	mDirectionalLight[0].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);			 //Initialise the diffuse values.
	mDirectionalLight[0].Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);			 //Initialise the specular values.
	mDirectionalLight[0].Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);	 //Initialise the direction for the light.
	
	mDirectionalLight[1].Ambient = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	mDirectionalLight[1].Diffuse = XMFLOAT4(0.20f, 0.20f, 0.20f, 1.0f);
	mDirectionalLight[1].Specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	mDirectionalLight[1].Direction = XMFLOAT3(-0.37735f, -0.67735f, 0.57735f);

	mDirectionalLight[2].Ambient = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	mDirectionalLight[2].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirectionalLight[2].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirectionalLight[2].Direction = XMFLOAT3(0.0f, -0.707f, -0.707f);

	// Materials
	mFloorMaterial.Ambient = XMFLOAT4(1.0, 1.0, 1.0, 1.0f);						 //Initialise the ambient values.
	mFloorMaterial.Diffuse = XMFLOAT4(0.5f, 0.5, 0.5, 1.0f);					 //Initialise the diffuse values.
	mFloorMaterial.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);				 //Initialise the specular values.

	mWallMaterial.Ambient = XMFLOAT4(0.4f, 0.6f, 0.4f, 1.0f);
	mWallMaterial.Diffuse = XMFLOAT4(0.651f, 0.5f, 0.392f, 1.0f);
	mWallMaterial.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	mLiftMaterial.Ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mLiftMaterial.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mLiftMaterial.Specular = XMFLOAT4(0.9f, 0.9f, 0.9f, 16.0f);

	mPoleMaterial.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mPoleMaterial.Diffuse = XMFLOAT4(0.2f, 0.2f, 0.7f, 1.0f);
	mPoleMaterial.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);

	mPlayerMaterial.Ambient = XMFLOAT4(0.6f, 0.8f, 0.9f, 1.0f);
	mPlayerMaterial.Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mPlayerMaterial.Specular = XMFLOAT4(0.75f, 0.75f, 0.75f, 16.0f);

	mEnemyMaterial.Ambient = XMFLOAT4(1.0f, 0.5f, 0.75f, 1.0f);
	mEnemyMaterial.Diffuse = XMFLOAT4(1.0f, 0.4f, 0.4f, 1.0f);
	mEnemyMaterial.Specular = XMFLOAT4(0.75f, 0.75f, 0.75f, 16.0f);

	// Shadows
	mShadowWallMaterial.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mShadowWallMaterial.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f);
	mShadowWallMaterial.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 16.0f);

	mShadowPlayerMaterial.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mShadowPlayerMaterial.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f);
	mShadowPlayerMaterial.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 16.0f);

	mShadowEnemyMaterial.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mShadowEnemyMaterial.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f);
	mShadowEnemyMaterial.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 16.0f);
}

// The deconstructor.
MazeGameMain::~MazeGameMain()
{
	SafeDelete(mSky);

	// Release the relevant COM interfaces.
	ReleaseCOM(mVertexBuffer);					
	ReleaseCOM(mIndexBuffer);

	ReleaseCOM(mFloorMapSRV);
	ReleaseCOM(mWallMapSRV);
	ReleaseCOM(mLiftMapSRV);
	ReleaseCOM(mPoleMapSRV);
	ReleaseCOM(mPlayerMapSRV);
	ReleaseCOM(mEnemyMapSRV);

	ReleaseCOM(mRandomTexSRV);
	ReleaseCOM(mExplosionTexSRV);
	ReleaseCOM(mSmokeTexSRV);
	ReleaseCOM(mSparkTexSRV);
	ReleaseCOM(mFireTexSRV);

	// The respective classes handle the end of program operations for shutting down safely.
	Effects::DestroyAll();
	InputLayouts::DestroyAll();
	RenderStates::DestroyAll();
}

// Initialise the Game.
bool MazeGameMain::Init()
{
	// Checks if the game has not initialised.
	if (!D3DApp::Init())
		return false;					// Registers as false

	// Must init Effects first since InputLayouts depend on shader signatures.
	Effects::InitAll(md3dDevice);		// Initialise the fx file for the project.
	InputLayouts::InitAll(md3dDevice);	// Prepare communications from this classes structure to the colour effect file.
	RenderStates::InitAll(md3dDevice);  // Initialise the render state class for the project.

	BuildGeometryBuffers();				// Creating the primitives with acquired vertices and indices. Followed by
										// funneling to the vertex and index buffers.
	
	mSky = new Sky(md3dDevice, L"Textures/Skybox.dds", 5000.0f);

	// Create a shader resource view to the texture. Stores the image to a pointer that was made.
	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice,
		L"Textures/grasstextures.dds", 0, 0, &mWallMapSRV, 0));

	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice,
		L"Textures/ParkPavement.dds", 0, 0, &mFloorMapSRV, 0));

	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice,
		L"Textures/Lift.dds", 0, 0, &mLiftMapSRV, 0));

	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice,
		L"Textures/Pole.dds", 0, 0, &mPoleMapSRV, 0));

	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice,
		L"Textures/marble.dds", 0, 0, &mPlayerMapSRV, 0));

	HR(D3DX11CreateShaderResourceViewFromFile(md3dDevice,
		L"Textures/EnemyTex.dds", 0, 0, &mEnemyMapSRV, 0));
	
	// Create a random texture to be used by the particle system.
	mRandomTexSRV = d3dHelper::CreateRandomTexture1DSRV(md3dDevice);
		
	// Create the particle effects adding the texture dds file into a vector then initialising it, how many particles to use as well.
	std::vector<std::wstring> explosion;
	explosion.push_back(L"Textures\\Explosion.dds");
	mExplosionTexSRV = d3dHelper::CreateTexture2DArraySRV(md3dDevice, md3dImmediateContext, explosion);
	mExplosion.Init(md3dDevice, Effects::ExplosionFX, mExplosionTexSRV, mRandomTexSRV, 300);
	
	std::vector<std::wstring> smokecloud;
	smokecloud.push_back(L"Textures\\Smoke.dds");
	mSmokeTexSRV = d3dHelper::CreateTexture2DArraySRV(md3dDevice, md3dImmediateContext, smokecloud);
	mSmoke.Init(md3dDevice, Effects::SmokeFX, mSmokeTexSRV, mRandomTexSRV, 800);

	std::vector<std::wstring> sparks;
	sparks.push_back(L"Textures\\Spark.dds");
	mSparkTexSRV = d3dHelper::CreateTexture2DArraySRV(md3dDevice, md3dImmediateContext, sparks);
	mSpark.Init(md3dDevice, Effects::SparkFX, mSparkTexSRV, mRandomTexSRV, 200);

	std::vector<std::wstring> flares;
	flares.push_back(L"Textures\\Flare.dds");
	mFireTexSRV = d3dHelper::CreateTexture2DArraySRV(md3dDevice, md3dImmediateContext, flares);
	mFire1.Init(md3dDevice, Effects::FireFX, mFireTexSRV, mRandomTexSRV, 500);
	mFire1.SetEmitPos(XMFLOAT3(mFire1Pos));	// The emit position for the fire particle effect.

	mFire2.Init(md3dDevice, Effects::FireFX, mFireTexSRV, mRandomTexSRV, 500);
	mFire2.SetEmitPos(XMFLOAT3(mFire2Pos)); // The emit position for the fire particle effect.

	mFire3.Init(md3dDevice, Effects::FireFX, mFireTexSRV, mRandomTexSRV, 500);
	mFire3.SetEmitPos(XMFLOAT3(mFire3Pos)); // The emit position for the fire particle effect.

	mFire4.Init(md3dDevice, Effects::FireFX, mFireTexSRV, mRandomTexSRV, 500);
	mFire4.SetEmitPos(XMFLOAT3(mFire4Pos)); // The emit position for the fire particle effect.

	return true;
}

// Function for resizing the window.
void MazeGameMain::OnResize()
{
	D3DApp::OnResize();													// Calls the OnResize function from the D3DApp class.

	mCam.SetLens(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);	// Set the frustum for the camera.
}

// Update the scene, using delta time as a parametre to keep the rendering consistent. 
void MazeGameMain::UpdateScene(float dt)
{
	// Convert Spherical to Cartesian coordinates.
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi);

	mEyePosW = XMFLOAT3(x, y, z);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, V);

	mLastPlayerPosition = mPlayerPosition;								// The last position before player input occurs. Used when collisions occur.

	// Checks if the player has not exploded or has sparks occuring.
	if (!mExplode || !mSparksOn){

		// Allow user to move box.
		if (GetAsyncKeyState('A') & 0x8000)
			mPlayerPosition.x -= 1.0f*dt;

		if (GetAsyncKeyState('D') & 0x8000)
			mPlayerPosition.x += 1.0f*dt;

		if (GetAsyncKeyState('W') & 0x8000)
			mPlayerPosition.z += 1.0f*dt;

		if (GetAsyncKeyState('S') & 0x8000)
			mPlayerPosition.z -= 1.0f*dt;
	}

	// **** LIFT ****
	// Update the new world matrix.
	XMMATRIX liftRotate = XMMatrixRotationY(0.5f*MathHelper::Pi);
	XMMATRIX liftScale = XMMatrixScaling(1.0, 1.0, 1.0);

	// A boolean obtaining data for if the end of game location has been collided by the player.
	mIsGameCompleted = EndOfGameCollision(mPlayerPosition);		

	// Checks to see if the game has been completed or not.
	if (mIsGameCompleted){
		// Restart the game.
		GameRestart();		
	}

	// A boolean obtaining data for if the lift has been collided by the player.
	bool isLiftCollided = LiftCollision(mPlayerPosition);

	// Checks if the life has been collided with by the player and the lift is on the first level.
	if (isLiftCollided && mIsLiftInStartPosition){

		mIsLiftMoving = true;												// Lift is now moving to the 2nd level.
		// The players position is set to the lifts position so the player is firmly on the lift object.
		mPlayerPosition = XMFLOAT3(mLiftPosition.x, mLiftPosition.y + 0.4f, mLiftPosition.z);
		mPlayerPosition.y += 2.0f*dt;										// The players position travels upwards with a speed of 2.0f.	
		mLiftPosition.y += 2.0f*dt;											// The lifts position travels upwards with a speed of 2.0f. 
		mLiftPosition.y = MathHelper::Min(mLiftPosition.y, 14.0f);			// The lift will be clamped at y position 14.0. It cannot exceed this position.	
		// If the lifts position is 14.0f on the y axis.
		if (mLiftPosition.y == 14.0f){										
			mIsLiftMoving = false;											// The lift is not moving anymore.
			mIsLiftInStartPosition = false;									// The lift is not on the first level anymore.
			mPlayerHealth = 100;											// Players health is 100. After reaching a new level the players health should be reset. To satisfy the player.
			mPlayerRedValue = 0.6f;											// The starting red value for the player for its RGB settings.
			mPlayerBlueValue = 0.8f;										// The starting blue value for the player for its RGB settings.
			mPlayerGreenValue = 0.9f;										// The starting green value for the player for its RGB settings.
			mPlayerMaterial.Ambient = XMFLOAT4(0.6f, 0.8f, 0.9f, 1.0f);		// The normal ambient material values for the player. So the player appears with the healthy colour again in case it was damaged before.
			mLiftPosition = mLiftLevel1;									// The lift position is set back to the first level position ready for use if the player requires to return to level 2 that way.
		}
	}

	// Update the new world matrix.
	XMMATRIX liftOffset = XMMatrixTranslation(mLiftPosition.x, mLiftPosition.y, mLiftPosition.z);
	XMStoreFloat4x4(&mLiftWorld, liftRotate*liftScale*liftOffset);

	// **** PLAYER ****
	// Update the player world matrix.
	XMMATRIX playerRotate = XMMatrixRotationY(0.5f*MathHelper::Pi);
	XMMATRIX playerScale = XMMatrixScaling(1.0, 1.0, 1.0);

	// Checks if the lift is still on the first level
	if (mIsLiftInStartPosition){
		// A boolean obtaining data for if a wall on the first level has collided with the player.
		bool isCollided = PlayerWallCollisionLv1(mPlayerPosition);

		// Is a collision detected
		if (isCollided){
			// Perform the wall damage to player function.
			WallDamageToPlayer(dt);
		}	
	}
	// Else if the lift is on the 2nd level.
	else if (!mIsLiftInStartPosition){
		// A boolean obtaining data for if a wall on the second level has collided with the player.
		bool isCollided = PlayerWallCollisionLv2(mPlayerPosition);					

		// Is a collision detected
		if (isCollided){
			// Perform the wall damage to player function.
			WallDamageToPlayer(dt);
		}

		// Is the first fire particle effect active. The same occurs for fire2 to fire4.
		if (mFire1On){
			// A boolean obtaining data for if a fire object the second level has collided with the player. The same occurs for fire2 to fire4.
			mIsFire1Collided = FirePlayerCollision(mPlayerPosition, mFire1Pos);			
		}
		if (mFire2On){
			mIsFire2Collided = FirePlayerCollision(mPlayerPosition, mFire2Pos);
		}
		if (mFire3On){
			mIsFire3Collided = FirePlayerCollision(mPlayerPosition, mFire3Pos);
		}
		if (mFire4On){
			mIsFire4Collided = FirePlayerCollision(mPlayerPosition, mFire4Pos);
		}		

		// Checks if any of the fire objects have been collided with by the player.
		if (mIsFire1Collided || mIsFire2Collided || mIsFire3Collided || mIsFire4Collided){
			mPlayerHealth -= 1;					// Subtract the health of the player by 1.
			// Change the RGB values of the player by 1% to see the loss of health on the player. The player will change from its natural colout to a red when health is low.
			mPlayerMaterial.Ambient = XMFLOAT4(mPlayerRedValue += mRedDamage, mPlayerBlueValue -= mBlueDamage, mPlayerGreenValue -= mGreenDamage, 1.0f);
			// If the players health is zero or less.
			if (mPlayerHealth <= 0){
				mExplode = true;				// The explode boolean is given a true value.
			}
			// Sets the threshold for the relative RGB values.
			if (mPlayerRedValue >= 1.0f){
				mPlayerRedValue = 1.0f;
			}
			if (mPlayerBlueValue <= 0.0f){
				mPlayerBlueValue = 0.0f;
			}
			if (mPlayerGreenValue >= 0.0f){
				mPlayerGreenValue = 0.0f;
			}
		}	
	}
	// Update the player world matrix.
	XMMATRIX playerOffset = XMMatrixTranslation(mPlayerPosition.x, mPlayerPosition.y, mPlayerPosition.z);
	XMStoreFloat4x4(&mPlayerWorld, playerRotate*playerScale*playerOffset);

	// Is the explode boolean true.
	if (mExplode){
		// The player respawn function is called.
		PlayerRespawn(dt);
	}

	// **** ENEMY ****
	// Update the new world matrix.
	XMMATRIX enemyRotate = XMMatrixRotationY(0.5f*MathHelper::Pi);
	XMMATRIX enemyScale = XMMatrixScaling(1.0, 1.0, 1.0);	

	// Checks if the lift is on the first level.
	if (mIsLiftInStartPosition){
		// A boolean obtaining data for if the enemy on the first level has collided with the player.
		mIsEnemyCollidedLevel1 = EnemyCollision(mPlayerPosition, mEnemy1NewPositionLevel1);
	}
	else{
		// A boolean obtaining data for if either of the enemies on the second level has collided with the player.
		mIsEnemy1CollidedLevel2 = EnemyCollision(mPlayerPosition, mEnemy1NewPositionLevel2);
		mIsEnemy2CollidedLevel2 = EnemyCollision(mPlayerPosition, mEnemy2NewPositionLevel2);
	}

	// Checks if any of the enemy objects have been collided with by the player.
	if (mIsEnemyCollidedLevel1 || mIsEnemy1CollidedLevel2 || mIsEnemy2CollidedLevel2){
		mExplode = true;						// The explode boolean is given a true value.
		// The player respawn function is called.
		PlayerRespawn(dt);
	}

	// Store the enemies new position performed by the enemies tick function.
	mEnemy1NewPositionLevel1 = mEnemyLevel1->Tick(dt);					
	mEnemy1NewPositionLevel2 = mEnemy1Level2->Tick(dt);
	mEnemy2NewPositionLevel2 = mEnemy2Level2->Tick(dt);

	// Update the new world matrix.
	XMMATRIX enemyOffset1 = XMMatrixTranslation(mEnemy1NewPositionLevel1.x, mEnemy1NewPositionLevel1.y, mEnemy1NewPositionLevel1.z);
	XMStoreFloat4x4(&mEnemyWorldLevel1, enemyRotate*enemyScale*enemyOffset1);

	XMMATRIX enemyOffset2 = XMMatrixTranslation(mEnemy1NewPositionLevel2.x, mEnemy1NewPositionLevel2.y, mEnemy1NewPositionLevel2.z);
	XMStoreFloat4x4(&mEnemy1WorldLevel2, enemyRotate*enemyScale*enemyOffset2);

	XMMATRIX enemyOffset3 = XMMatrixTranslation(mEnemy2NewPositionLevel2.x, mEnemy2NewPositionLevel2.y, mEnemy2NewPositionLevel2.z);
	XMStoreFloat4x4(&mEnemy2WorldLevel2, enemyRotate*enemyScale*enemyOffset3);
		
	// Checks if the smoke particle effect is active.
	if (mSmokeOn){
		mSmoke.SetEmitPos(mDeathPlayerPosition);		// The emit position for the fire particle effect.
		mSmoke.Update(dt, mTimer.TotalTime());			// The update function for the particle effect is performed.
		// Checks if the age of the smoke is greater or equal to 2.0f.
		if (mSmoke.GetAge() >= 2.0f){
			mSmoke.Reset();								// Reset the particle effect.
			mSmokeOn = false;							// The smoke is set to being deactivated.
		}
	}

	// **** CAMERA ****
	// Update the camera so it moves with the player.
	XMFLOAT3 offset = XMFLOAT3(x, y + 4.0f, z);
	XMFLOAT3 newCameraPos = XMFLOAT3(mPlayerPosition.x + offset.x, mPlayerPosition.y + offset.y, mPlayerPosition.z + offset.z);	
	mCam.LookAt(newCameraPos, mPlayerPosition, XMFLOAT3(0.0f, 0.4f, 0.0f));	

	// Checks if the fire is not on.
	if (!mFire1On){
		mTemp1 += dt;								// Increase using delta time.
		// If the time is greater than 2.0.
		if ((mTemp1 > 2.0f)){	
			mTemp1 = 0.0f;							// Reset the time.
			mFire1On = true;						// The fire is activated.
		}
	} // Else if the fire is greater than 2.0.
	else if (mFire1.GetAge() > 2.0f)
	{
		mFire1.Reset();								// Reset the particle effect.
		mFire1On = false;							// The fire is deactivated.
	}

	// Checks if the fire is on.
	if (mFire1On){							
		mFire1.Update(dt, mTimer.TotalTime());		// The update function for the fire particle effect is performed.
	}

	// Checks if the fire2 is not on.
	if (!mFire2On){
		mTemp2 += dt;
		if ((mTemp2 > 6.0f)){
			mTemp2 = 0.0f;
			mFire2On = true;
		}
	} // Fire system reset
	else if (mFire2.GetAge() > 2.0f)
	{
		mFire2.Reset();
		mFire2On = false;
	}

	if (mFire2On){
		mFire2.Update(dt, mTimer.TotalTime());
	}

	// Checks if the fire3 is not on.
	if (!mFire3On){
		mTemp3 += dt;
		if ((mTemp3 > 4.0f)){
			mTemp3 = 0.0f;
			mFire3On = true;
		}
	} // Fire system reset
	else if (mFire3.GetAge() > 2.0f)
	{
		mFire3.Reset();
		mFire3On = false;
	}

	if (mFire3On){
		mFire3.Update(dt, mTimer.TotalTime());
	}

	// Checks if the fire4 is not on.
	if (!mFire4On){
		mTemp4 += dt;
		if ((mTemp4 > 4.0f)){
			mTemp4 = 0.0f;
			mFire4On = true;
		}
	} // Fire system reset
	else if (mFire4.GetAge() > 2.0f)
	{
		mFire4.Reset();
		mFire4On = false;
	}

	if (mFire4On){
		mFire4.Update(dt, mTimer.TotalTime());
	}	
}

// A function that returns true or false if the player has collided with a wall.
bool MazeGameMain::PlayerWallCollisionLv1(XMFLOAT3 pos)
{
	XMFLOAT3 playerPosition = pos;			// The player position.
	float  playerBoundary = 0.30f;   		// Boundary for the player from its centre.
	float  wallBoundary = 0.5f;				// Boundary for the wall from its centre.

	// Using the wall position container, determine the x,z difference and check if a wall collision has occured.
	for (int i = 0; i < mWallPositionLevel1.size(); i++){

		float diffX = abs(mWallPositionLevel1[i].x - playerPosition.x);	// The difference between the player's x position and the wall's x position. 
		float diffZ = abs(mWallPositionLevel1[i].z - playerPosition.z);	// The difference between the player's z position and the wall's z position.

		// Checks if the diffX and diffZ are less than the proximity both player and wall can have. Before a collision would occur. 
		if (diffX <= playerBoundary + wallBoundary && diffZ <= playerBoundary + wallBoundary)
		{
			return true;
		}
	}
	return false;
}

// A function that returns true or false if the player has collided with a wall.
bool MazeGameMain::PlayerWallCollisionLv2(XMFLOAT3 pos)
{
	XMFLOAT3 playerPosition = pos;								// The player position.
	float  playerBoundary = 0.30f;   							// Boundary for the player from its centre.
	float  wallBoundary = 0.5f;									// Boundary for the wall from its centre.

	// Using the wall position container, determine the x,z difference and check if a wall collision has occured.
	for (int i = 0; i < mWallPositionLevel2.size(); i++){

		float updateWallX = mWallPositionLevel2[i].x + 6.0f;	// Due to the maze offset. This required amending also by the 6.0 offset.
		float updatedWallZ = mWallPositionLevel2[i].z + 4.0f;	// Due to the maze offset. This required amending also by the 4.0 offset.

		float diffX = abs(updateWallX - playerPosition.x);		// The difference between the player's x position and the wall's x position. 
		float diffZ = abs(updatedWallZ - playerPosition.z);		// The difference between the player's z position and the wall's z position.

		// Checks if the diffX and diffZ are less than the proximity both player and wall can have. Before a collision would occur. 
		if (diffX <= playerBoundary + wallBoundary && diffZ <= playerBoundary + wallBoundary)
		{
			return true;
		}
	}
	return false;
}

// A function that returns true or false if the enemy has collided with the player wall.
bool MazeGameMain::EnemyCollision(XMFLOAT3 pos, XMFLOAT3 posEnemy)
{
	XMFLOAT3 playerPos = pos;				// The player position.
	XMFLOAT3 enemyPos = posEnemy;			// The enemy position.
	float  playerBoundary = 0.30f;			// Boundary for the player from its centre
	float  enemyBoundary = 0.35f;			// Boundary for the enemy from its centre.

	float diffX = abs(playerPos.x - posEnemy.x);	// The difference between the player's x position and the enemy's x position. 
	float diffZ = abs(playerPos.z - posEnemy.z);	// The difference between the player's z position and the enemy's z position.

	// Checks if the diffX and diffZ are less than the proximity both player and enemy can have. Before a collision would occur. 
	if (diffX <= playerBoundary + enemyBoundary && diffZ <= playerBoundary + enemyBoundary)
	{
		return true;
	}

	return false;
}

// A function for when the wall damages the player.
void MazeGameMain::WallDamageToPlayer(float dt)
{
	mSparksOn = true;							// The spark particle effect is activated.
	mPlayerPosition = mLastPlayerPosition;		// The last position before player input occurs. 
	mPlayerHealth -= 1;							// Subtract the health of the player by 1.
	// Change the RGB values of the player by 1% to see the loss of health on the player. The player will change from its natural colout to a red when health is low.
	mPlayerMaterial.Ambient = XMFLOAT4(mPlayerRedValue += mRedDamage, mPlayerBlueValue -= mBlueDamage, mPlayerGreenValue -= mGreenDamage, 1.0f);
	// If the players health is zero or less.
	if (mPlayerHealth <= 0){
		mExplode = true;						// The explode boolean is given a true value.
	}
	// Sets the threshold for the relative RGB values.
	if (mPlayerRedValue >= 1.0f){
		mPlayerRedValue = 1.0f;
	}
	if (mPlayerBlueValue <= 0.0f){
		mPlayerBlueValue = 0.0f;
	}
	if (mPlayerGreenValue >= 0.0f){
		mPlayerGreenValue = 0.0f;
	}
	// If the spark particle effect is activated.
	if (mSparksOn){
		mSpark.SetEmitPos(mPlayerPosition);		// The emit position for the spark particle effect. The player's position.
		mSpark.Update(dt, mTimer.TotalTime());	// The update function for the spark particle effect is performed.
	}
}

// A function that returns true or false if the player has collided with a fire object.
bool MazeGameMain::FirePlayerCollision(XMFLOAT3 pos, XMFLOAT3 posFire)
{
	XMFLOAT3 playerPos = pos;					// The player position.
	XMFLOAT3 firePos = posFire;					// The fire position.
	float  playerBoundary = 0.35f; 				// Boundary for the player from its centre
	float  fireBoundary = 0.5f;					// Boundary for the enemy from its centre.
	float diffX;								// Difference of the x vector.
	float diffZ;								// Difference of the y vector.

	diffX = abs(playerPos.x - posFire.x);		// The difference between the player's x position and the fire's x position. 
	diffZ = abs(playerPos.z - posFire.z);		// The difference between the player's z position and the fire's z position.

	// Checks if the diffX and diffZ are less than the proximity both player and fire can have. Before a collision would occur. 
	if (diffX <= playerBoundary + fireBoundary && diffZ <= playerBoundary + fireBoundary)
	{
		return true;
	}

	return false;
}

// A function for when the player respawns.
void MazeGameMain::PlayerRespawn(float dt)
{
	mSparksOn = false;													// Deactivate the spark particle effect.
	mPlayerPosition = mLastPlayerPosition;								// The last position before player input occurs. 
	mDeathPlayerPosition = mPlayerPosition;								// The death position aquires the current player position.
	mExplosion.SetEmitPos(XMFLOAT3(mPlayerPosition.x, mPlayerPosition.y - 0.5f, mPlayerPosition.z));	// The emit position for the explosion particle effect. The player's position.
	mExplosion.Update(dt, mTimer.TotalTime());							// The update function for the explosion particle effect is performed.
	// Checks the age of the explosion particle effect is over or equal to 2.0.
	if (mExplosion.GetAge() >= 2.0f){
		mExplosion.Reset();												// Resets the particle effect.
		// Is the left on the first level.
		if (mIsLiftInStartPosition){									
			mPlayerPosition = XMFLOAT3(mPlayerSpawnLevel1);				// The player will respawn at the first level spawn point.
		}																
		else{															
			mPlayerPosition = XMFLOAT3(mPlayerSpawnLevel2);				// The player will respawn at the second level spawn point.
		}																
		mExplode = false;												// Set explode as being deactivated.
		mPlayerHealth = 100;											// The players health is reset.
		mPlayerRedValue = 0.6f;											// The RGB values are reset.
		mPlayerBlueValue = 0.8f;										// 
		mPlayerGreenValue = 0.9f;										//
		mPlayerMaterial.Ambient = XMFLOAT4(0.6f, 0.8f, 0.9f, 1.0f);		// The player ambience material is reset.
		mSmokeOn = true;												// The smoke is activated as a result of an explosion having occured.
																		
		mIsEnemyCollidedLevel1 = false;									// Set the enemies as having been collided with to false.
		mIsEnemy1CollidedLevel2 = false;								//
		mIsEnemy2CollidedLevel2 = false;								//
																		
		mIsFire1Collided = false;										// Set the fire as having been collided with to false. 
		mIsFire2Collided = false;										//
		mIsFire3Collided = false;										//
		mIsFire4Collided = false;										//
	}
}

// A function that returns true or false if the player has collided with the lift object.
bool MazeGameMain::LiftCollision(XMFLOAT3 pos)
{
	XMFLOAT3 playerPosition = pos;										// The player position.
	XMFLOAT3 liftPosition = XMFLOAT3(mLiftPosition.x, mLiftPosition.y, mLiftPosition.z); // The lift position.
	float  playerBoundary = 0.2f;									    // Boundary for the player from its centre
	float  liftBoundary = 0.2f;											// Boundary for the lift from its centre

	float diffX = abs(liftPosition.x - playerPosition.x);				// The difference between the lift's x position and the player's x position. 
	float diffZ = abs(liftPosition.z - playerPosition.z);				// The difference between the lift's x position and the player's x position. 

	// Checks if the diffX and diffZ are less than the proximity both player and lift can have. Before a collision would occur. 
	if (diffX <= playerBoundary + liftBoundary && diffZ <= playerBoundary + liftBoundary)
	{
		return true;
	}

	return false;
}

// A function that returns true or false if the player has collided with the end of game location.
bool MazeGameMain::EndOfGameCollision(XMFLOAT3 pos)
{
	XMFLOAT3 playerPosition = pos;										// The player position.
	XMFLOAT3 pole2Position = XMFLOAT3(mEndOfGame);						// The pole2 position. 
	float  playerBoundary = 0.2f;										// Boundary for the player from its centre
	float  poleBoundary = 0.2f;											// Boundary for the pole from its centre
																		
	float diffX = abs(pole2Position.x - playerPosition.x);				// The difference between the poles' x position and the player's x position. 
	float diffZ = abs(pole2Position.z - playerPosition.z);				// The difference between the poles' x position and the player's x position. 

	// Checks if the diffX and diffZ are less than the proximity both player and pole can have. Before a collision would occur. 
	if (diffX <= playerBoundary + poleBoundary && diffZ <= playerBoundary + poleBoundary)
	{
		return true;
	}

	return false;
}

// A function that restarts the game. Once the player has reached the end of game location.
void MazeGameMain::GameRestart()
{
	mIsGameCompleted = false;											// The players health is reset.
	mPlayerPosition = mPlayerSpawnLevel1;								// The player will respawn at the first level spawn point.
	mIsLiftInStartPosition = true;										// The lift is reset to the first level.
	mPlayerHealth = 100;												// Reset the player's health to 100.
	mPlayerRedValue = 0.6f;												// The RGB values are reset.
	mPlayerBlueValue = 0.8f;											// 
	mPlayerGreenValue = 0.9f;											//
	mPlayerMaterial.Ambient = XMFLOAT4(0.6f, 0.8f, 0.9f, 1.0f);			// The player ambience material is reset.
}

// Draws/renders the scene.
void MazeGameMain::DrawScene()
{
	// Clear the screen.
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	// Clear the stencil buffer.
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Bind the input layout to the Input Assembler.
	md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	// Default blend factor value.
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	UINT stride = sizeof(Vertex::Basic32);										// Size of the vertex buffer.
	UINT offset = 0;															// The value for the offset from which the input assembler should start at.
		
	// Rebuild the view matrix once per frame.
	mCam.UpdateViewMatrix();

	// Set constants
	XMMATRIX view = mCam.View();
	XMMATRIX proj = mCam.Proj();
	XMMATRIX viewProj = mCam.ViewProj();

	// Set per frame constants.
	Effects::BasicFX->SetDirLights(mDirectionalLight);							// Setup the directional lights for the scene.
	Effects::BasicFX->SetEyePosW(mCam.GetPosition());							// Setup eye position for the lighting calculations in Basic.fx.
	Effects::BasicFX->SetCubeMap(mSky->CubeMapSRV());

	//<<<md3dImmediateContext->RSSetState(RenderStates::CullBackRS);

	// The technique to use from the Basic.fx.
	ID3DX11EffectTechnique* activeTech = Effects::BasicFX->Light3TexTech;

	D3DX11_TECHNIQUE_DESC techDesc;
	activeTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)									// Loop over each pass in the technique.
	{
		// Bind the vertex buffer to an input slot here denoted by the 1 value. Reference the vertex buffer and other variables.
		md3dImmediateContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
		// Bind the index buffer to the Input Assembler, with the index buffer pointer and the format for the indices.
		md3dImmediateContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// Draw the floor, level 1.		
		XMMATRIX world = XMLoadFloat4x4(&mFloorWorldLevel1);					// Load the floor world to be used for this draw function.
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);		// Perform the calculation to inverse the world matrix.
		XMMATRIX worldViewProj = world*view*proj;								// Calculate the world view projection.

		Effects::BasicFX->SetWorld(world);										// Setup the world matrix for drawing the floor.
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);				// Setup the world inverse transpose matrix.
		Effects::BasicFX->SetWorldViewProj(worldViewProj);						// Setup the world view projection matrix.
		Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&mFloorTexTransform));	// Setup the texture transform matrix.
		Effects::BasicFX->SetMaterial(mFloorMaterial);							// Setup the material to be used for the floor.
		Effects::BasicFX->SetDiffuseMap(mFloorMapSRV);							// Setup the texture to be used for the floor.

		// Draw shaders, render states. Update constant buffer.
		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		// Draw the floor using the index count and offsets to retrieve the relevant data from the buffer.
		md3dImmediateContext->DrawIndexed(mFloor1IndexCount, mFloor1IndexOffset, mFloor1VertexOffset);
		
		// Draw the floor, level 2..		
		world = XMLoadFloat4x4(&mFloorWorldLevel2);									 // Load the floor world to be used for this draw function.
		worldInvTranspose = MathHelper::InverseTranspose(world);					 // Perform the calculation to inverse the world matrix.
		worldViewProj = world*view*proj;											 // Calculate the world view projection.
																					 
		Effects::BasicFX->SetWorld(world);											 // Setup the world matrix for drawing the floor.
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);					 // Setup the world inverse transpose matrix.
		Effects::BasicFX->SetWorldViewProj(worldViewProj);							 // Setup the world view projection matrix.
		Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&mFloorTexTransform));		 // Setup the texture transform matrix.
		Effects::BasicFX->SetMaterial(mFloorMaterial);								 // Setup the material to be used for the floor.
		Effects::BasicFX->SetDiffuseMap(mFloorMapSRV);								 // Setup the texture to be used for the floor.

		// Draw shaders, render states. Update constant buffer.
		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		// Draw the floor using the index count and offsets to retrieve the relevant data from the buffer.
		md3dImmediateContext->DrawIndexed(mFloor2IndexCount, mFloor2IndexOffset, mFloor2VertexOffset);


		// Draw the wall, using a container of wall world matrices
		for (int i = 0; i < mWallWorldLevel1.size(); ++i)
		{
			world = XMLoadFloat4x4(&mWallWorldLevel1[i]);							 // Load the wall world to be used for this draw function.
			worldInvTranspose = MathHelper::InverseTranspose(world);				 // Perform the calculation to inverse the world matrix.
			worldViewProj = world*view*proj;										 // Calculate the world view projection.

			Effects::BasicFX->SetWorld(world);										 // Setup the world matrix for drawing the wall.
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);				 // Setup the world inverse transpose matrix.
			Effects::BasicFX->SetWorldViewProj(worldViewProj);						 // Setup the world view projection matrix.
			Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&mTexTransform));		 // Setup the texture transform matrix.
			Effects::BasicFX->SetMaterial(mWallMaterial);							 // Setup the material to be used for the wall.
			Effects::BasicFX->SetDiffuseMap(mWallMapSRV);							 // Setup the texture to be used for the wall.

			// Draw shaders, render states. Update constant buffer.
			activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			// Draw the wall using the index count and offsets to retrieve the relevant data from the buffer.
			md3dImmediateContext->DrawIndexed(mWallIndexCount, mWallIndexOffset, mWallVertexOffset);
		}

		// Draw the wall, using a container of wall level 2 world matrices
		for (int i = 0; i < mWallWorldLevel2.size(); ++i)
		{
			world = XMLoadFloat4x4(&mWallWorldLevel2[i]);							 // Load the wall world level 2 to be used for this draw function.
			worldInvTranspose = MathHelper::InverseTranspose(world);				 // Perform the calculation to inverse the world matrix.
			worldViewProj = world*view*proj;										 // Calculate the world view projection.

			Effects::BasicFX->SetWorld(world);										 // Setup the world matrix for drawing the wall for level 2.
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);				 // Setup the world inverse transpose matrix.
			Effects::BasicFX->SetWorldViewProj(worldViewProj);						 // Setup the world view projection matrix.
			Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&mTexTransform));		 // Setup the texture transform matrix.
			Effects::BasicFX->SetMaterial(mWallMaterial);							 // Setup the material to be used for the wall.
			Effects::BasicFX->SetDiffuseMap(mWallMapSRV);							 // Setup the texture to be used for the wall.

			// Draw shaders, render states. Update constant buffer.
			activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			// Draw the wall using the index count and offsets to retrieve the relevant data from the buffer.
			md3dImmediateContext->DrawIndexed(mWallIndexCount, mWallIndexOffset, mWallVertexOffset);
		}

		// Draw the lift.				
		world = XMLoadFloat4x4(&mLiftWorld);										 // Load the lift world to be used for this draw function.
		worldInvTranspose = MathHelper::InverseTranspose(world);					 // Perform the calculation to inverse the world matrix.
		worldViewProj = world*view*proj;											 // Calculate the world view projection.
																					 
		Effects::BasicFX->SetWorld(world);											 // Setup the world matrix for drawing the lift.
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);					 // Setup the world inverse transpose matrix.
		Effects::BasicFX->SetWorldViewProj(worldViewProj);							 // Setup the world view projection matrix.
		Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&mTexTransform));			 // Setup the texture transform matrix.
		Effects::BasicFX->SetMaterial(mLiftMaterial);								 // Setup the material to be used for the lift.
		Effects::BasicFX->SetDiffuseMap(mLiftMapSRV);							     // Setup the texture to be used for the lift.

		// Draw shaders, render states. Update constant buffer.
		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		// Draw the lift using the index count and offsets to retrieve the relevant data from the buffer.
		md3dImmediateContext->DrawIndexed(mLiftIndexCount, mLiftIndexOffset, mLiftVertexOffset);

		// Draw the pole for level 1.
		world = XMLoadFloat4x4(&mPoleWorldLevel1);									// Load the pole world to be used for this draw function.
		worldInvTranspose = MathHelper::InverseTranspose(world);					// Perform the calculation to inverse the world matrix.
		worldViewProj = world*view*proj;											// Calculate the world view projection.

		Effects::BasicFX->SetWorld(world);											// Setup the world matrix for drawing the pole.
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);					// Setup the world inverse transpose matrix.
		Effects::BasicFX->SetWorldViewProj(worldViewProj);							// Setup the world view projection matrix.
		Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&mTexTransform));			// Setup the texture transform matrix.
		Effects::BasicFX->SetMaterial(mPoleMaterial);								// Setup the material to be used for the pole.
		Effects::BasicFX->SetDiffuseMap(mPoleMapSRV);								// Setup the texture to be used for the pole.

		// Draw shaders, render states. Update constant buffer.
		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		// Draw the pole using the index count and offsets to retrieve the relevant data from the buffer.
		md3dImmediateContext->DrawIndexed(mPole1IndexCount, mPole1IndexOffset, mPole1VertexOffset);

		// Draw the pole for level 2.
		world = XMLoadFloat4x4(&mPoleWorldLevel2);									// Load the pole world to be used for this draw function.
		worldInvTranspose = MathHelper::InverseTranspose(world);					// Perform the calculation to inverse the world matrix.
		worldViewProj = world*view*proj;											// Calculate the world view projection.

		Effects::BasicFX->SetWorld(world);											// Setup the world matrix for drawing the pole.
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);					// Setup the world inverse transpose matrix.
		Effects::BasicFX->SetWorldViewProj(worldViewProj);							// Setup the world view projection matrix.
		Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&mTexTransform));			// Setup the texture transform matrix.
		Effects::BasicFX->SetMaterial(mPoleMaterial);								// Setup the material to be used for the pole.
		Effects::BasicFX->SetDiffuseMap(mPoleMapSRV);								// Setup the texture to be used for the pole.

		// Draw shaders, render states. Update constant buffer.
		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		// Draw the pole using the index count and offsets to retrieve the relevant data from the buffer.
		md3dImmediateContext->DrawIndexed(mPole2IndexCount, mPole2IndexOffset, mPole2VertexOffset);


		// Draw the player. 
		world = XMLoadFloat4x4(&mPlayerWorld);										// Load the player world to be used for this draw function.		
		worldInvTranspose = MathHelper::InverseTranspose(world);					// Perform the calculation to inverse the world matrix.
		worldViewProj = world*view*proj;											// Calculate the world view projection.

		Effects::BasicFX->SetWorld(world);											// Setup the world matrix for drawing the player.
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);					// Setup the world inverse transpose matrix.
		Effects::BasicFX->SetWorldViewProj(worldViewProj);							// Setup the world view projection matrix.
		Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&mTexTransform));			// Setup the texture transform matrix.
		Effects::BasicFX->SetMaterial(mPlayerMaterial);								// Setup the material to be used for the player.
		Effects::BasicFX->SetDiffuseMap(mPlayerMapSRV);								// Setup the texture to be used for the player.

		// Draw shaders, render states. Update constant buffers.
		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		// If the explode boolean is false. Draw the player using the index count and offsets to retrieve the relevant data from the buffers.
		if (!mExplode){
			md3dImmediateContext->DrawIndexed(mPlayerIndexCount, mPlayerIndexOffset, mPlayerVertexOffset);
		}

		// Draw the enemy. 
		world = XMLoadFloat4x4(&mEnemyWorldLevel1);									// Load the enemy world to be used for this draw function.
		worldInvTranspose = MathHelper::InverseTranspose(world);					// Perform the calculation to inverse the world matrix.
		worldViewProj = world*view*proj;											// Calculate the world view projection.

		Effects::BasicFX->SetWorld(world);											// Setup the world matrix for drawing the enemy.
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);					// Setup the world inverse transpose matrix.
		Effects::BasicFX->SetWorldViewProj(worldViewProj);							// Setup the world view projection matrix.
		Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&mTexTransform));			// Setup the texture transform matrix.
		Effects::BasicFX->SetMaterial(mEnemyMaterial);								// Setup the material to be used for the enemy.
		Effects::BasicFX->SetDiffuseMap(mEnemyMapSRV);								// Setup the texture to be used for the enemy.

		// Draw shaders, render states. Update constant buffers.
		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);		
		// Draw the enemy using the index count and offsets to retrieve the relevant data from the buffers.
		md3dImmediateContext->DrawIndexed(mEnemyIndexCount, mEnemyIndexOffset, mEnemyVertexOffset);

		// Draw the enemy. 
		world = XMLoadFloat4x4(&mEnemy1WorldLevel2);								// Load the enemy world to be used for this draw function.
		worldInvTranspose = MathHelper::InverseTranspose(world);					// Perform the calculation to inverse the world matrix.
		worldViewProj = world*view*proj;											// Calculate the world view projection.

		Effects::BasicFX->SetWorld(world);											// Setup the world matrix for drawing the enemy.
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);					// Setup the world inverse transpose matrix.
		Effects::BasicFX->SetWorldViewProj(worldViewProj);							// Setup the world view projection matrix.
		Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&mTexTransform));			// Setup the texture transform matrix.
		Effects::BasicFX->SetMaterial(mEnemyMaterial);								// Setup the material to be used for the enemy.
		Effects::BasicFX->SetDiffuseMap(mEnemyMapSRV);								// Setup the texture to be used for the enemy.

		// Draw shaders, render states. Update constant buffers.
		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		// Draw the enemy using the index count and offsets to retrieve the relevant data from the buffers.
		md3dImmediateContext->DrawIndexed(mEnemyIndexCount, mEnemyIndexOffset, mEnemyVertexOffset);

		// Draw the enemy. 
		world = XMLoadFloat4x4(&mEnemy2WorldLevel2);								 // Load the enemy world to be used for this draw function.
		worldInvTranspose = MathHelper::InverseTranspose(world);					 // Perform the calculation to inverse the world matrix.
		worldViewProj = world*view*proj;											 // Calculate the world view projection.

		Effects::BasicFX->SetWorld(world);											 // Setup the world matrix for drawing the enemy.
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);					 // Setup the world inverse transpose matrix.
		Effects::BasicFX->SetWorldViewProj(worldViewProj);							 // Setup the world view projection matrix.
		Effects::BasicFX->SetTexTransform(XMLoadFloat4x4(&mTexTransform));			 // Setup the texture transform matrix.
		Effects::BasicFX->SetMaterial(mEnemyMaterial);								 // Setup the material to be used for the enemy.
		Effects::BasicFX->SetDiffuseMap(mEnemyMapSRV);								 // Setup the texture to be used for the enemy.

		// Draw shaders, render states. Update constant buffers.
		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		// Draw the enemy using the index count and offsets to retrieve the relevant data from the buffers.
		md3dImmediateContext->DrawIndexed(mEnemyIndexCount, mEnemyIndexOffset, mEnemyVertexOffset);


		// *** SHADOWS ***
		// Set per object constants.
		XMVECTOR shadowPlane = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);				
		XMVECTOR toMainLight = -XMLoadFloat3(&mDirectionalLight[1].Direction);
		XMMATRIX S = XMMatrixShadow(shadowPlane, toMainLight);
		XMMATRIX shadowOffsetYLevel1 = XMMatrixTranslation(0.0f, 0.001f, 0.0f);
		XMMATRIX shadowOffsetYLevel2 = XMMatrixTranslation(7.75f, 14.001f, -12.0f);

		// Draw the wall level 1 shadows.
		// Loop through the container of wall world matrices for level 1.
		for (int i = 0; i < mWallWorldLevel1.size(); ++i){
			// Set per object constants.
			XMMATRIX world = XMLoadFloat4x4(&mWallWorldLevel1[i])*S*shadowOffsetYLevel1;
			XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
			XMMATRIX worldViewProj = world*view*proj;

			Effects::BasicFX->SetWorld(world);									// Setup the world matrix for drawing the wall.
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);			// Setup the world inverse transpose matrix.
			Effects::BasicFX->SetWorldViewProj(worldViewProj);					// Setup the world view projection matrix.
			Effects::BasicFX->SetMaterial(mShadowWallMaterial);					// Setup the shadow material to be used by the wall.

			// Bind the transparent blend state object to the output merger stage of the pipeline.
			md3dImmediateContext->OMSetBlendState(RenderStates::TransparentBS, blendFactor, 0xffffffff);
			// Bind the no double blend state object to the output merger stage of the pipeline.
			md3dImmediateContext->OMSetDepthStencilState(RenderStates::NoDoubleBlendDSS, 0);
			// Draw shaders, render states. Update constant buffers.
			activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			// Draw the wall using the index count and offsets to retrieve the relevant data from the buffer.
			md3dImmediateContext->DrawIndexed(mWallIndexCount, mWallIndexOffset, mWallVertexOffset);
		}

		// Set the object constants for the wall shadow offset. 
		XMMATRIX shadowWallOffsetYLevel2 = XMMatrixTranslation(7.8f, 14.01f, -11.9f);

		// Draw the wall level 2 shadows.
		// Loop through the container of wall world matrices for level 2.
		for (int i = 0; i < mWallWorldLevel2.size(); ++i){
			// Set per object constants.
			world = XMLoadFloat4x4(&mWallWorldLevel2[i])*S*shadowWallOffsetYLevel2;
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world*view*proj;

			Effects::BasicFX->SetWorld(world);									// Setup the world matrix for drawing the wall.
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);			// Setup the world inverse transpose matrix.
			Effects::BasicFX->SetWorldViewProj(worldViewProj);					// Setup the world view projection matrix.
			Effects::BasicFX->SetMaterial(mShadowWallMaterial);					// Setup the shadow material to be used by the wall.

			// Bind the transparent blend state object to the output merger stage of the pipeline.
			md3dImmediateContext->OMSetBlendState(RenderStates::TransparentBS, blendFactor, 0xffffffff);
			// Bind the no double blend state object to the output merger stage of the pipeline.
			md3dImmediateContext->OMSetDepthStencilState(RenderStates::NoDoubleBlendDSS, 0);
			// Draw shaders, render states. Update constant buffers.
			activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			// Draw the wall using the index count and offsets to retrieve the relevant data from the buffer.
			md3dImmediateContext->DrawIndexed(mWallIndexCount, mWallIndexOffset, mWallVertexOffset);
		}

		// Draw the player shadow.
		// Is the lift on the second floor.
		if (mIsLiftMoving){
			// Remove shadow whilst on the lift. 
			mShadowPlayerMaterial.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
			mShadowPlayerMaterial.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
			mShadowPlayerMaterial.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		}
		else{
			// Shadow reappears. 
			mShadowPlayerMaterial.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
			mShadowPlayerMaterial.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f);
			mShadowPlayerMaterial.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 16.0f);
		}

		// Is the lift on the first level.
		if (mIsLiftInStartPosition){
			// Set per object constant. Shadow level 1 values.
			world = XMLoadFloat4x4(&mPlayerWorld)*S*shadowOffsetYLevel1;
		}
		else{
			// Set per object constant. Shadow level 2 values.
			world = XMLoadFloat4x4(&mPlayerWorld)*S*shadowOffsetYLevel2;
		}		

		// Set per object constants.
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		Effects::BasicFX->SetWorld(world);										// Setup the world matrix for drawing the player.
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);				// Setup the world inverse transpose matrix.
		Effects::BasicFX->SetWorldViewProj(worldViewProj);						// Setup the world view projection matrix.
		Effects::BasicFX->SetMaterial(mShadowPlayerMaterial);					// Setup the shadow material to be used by the player.

		// Bind the transparent blend state object to the output merger stage of the pipeline.
		md3dImmediateContext->OMSetBlendState(RenderStates::TransparentBS, blendFactor, 0xffffffff);
		// Bind the no double blend state object to the output merger stage of the pipeline.
		md3dImmediateContext->OMSetDepthStencilState(RenderStates::NoDoubleBlendDSS, 0);
		// Draw shaders, render states. Update constant buffers.
		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		// Draw the player using the index count and offsets to retrieve the relevant data from the buffer.
		md3dImmediateContext->DrawIndexed(mPlayerIndexCount, mPlayerIndexOffset, mPlayerVertexOffset);
		
		// Draw the Enemy shadow.
		// Set per object constants.
		world = XMLoadFloat4x4(&mEnemyWorldLevel1)*S*shadowOffsetYLevel1;
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;
			
		Effects::BasicFX->SetWorld(world);										// Setup the world matrix for drawing the enemy.
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);				// Setup the world inverse transpose matrix.
		Effects::BasicFX->SetWorldViewProj(worldViewProj);						// Setup the world view projection matrix.
		Effects::BasicFX->SetMaterial(mShadowEnemyMaterial);					// Setup the shadow material to be used by the enemy.

		// Bind the transparent blend state object to the output merger stage of the pipeline.
		md3dImmediateContext->OMSetBlendState(RenderStates::TransparentBS, blendFactor, 0xffffffff);
		// Bind the no double blend state object to the output merger stage of the pipeline.
		md3dImmediateContext->OMSetDepthStencilState(RenderStates::NoDoubleBlendDSS, 0);
		// Draw shaders, render states. Update constant buffers.
		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		// Draw the enemy using the index count and offsets to retrieve the relevant data from the buffer.
		md3dImmediateContext->DrawIndexed(mEnemyIndexCount, mEnemyIndexOffset, mEnemyVertexOffset);

		// Set per object constants.
		world = XMLoadFloat4x4(&mEnemy1WorldLevel2)*S*shadowOffsetYLevel2;
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		Effects::BasicFX->SetWorld(world);										// Setup the world matrix for drawing the enemy.
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);				// Setup the world inverse transpose matrix.
		Effects::BasicFX->SetWorldViewProj(worldViewProj);						// Setup the world view projection matrix.
		Effects::BasicFX->SetMaterial(mShadowEnemyMaterial);					// Setup the shadow material to be used by the enemy.

		// Bind the transparent blend state object to the output merger stage of the pipeline.
		md3dImmediateContext->OMSetBlendState(RenderStates::TransparentBS, blendFactor, 0xffffffff);
		// Bind the no double blend state object to the output merger stage of the pipeline.
		md3dImmediateContext->OMSetDepthStencilState(RenderStates::NoDoubleBlendDSS, 0);
		// Draw shaders, render states. Update constant buffers.
		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		// Draw the enemy using the index count and offsets to retrieve the relevant data from the buffer.
		md3dImmediateContext->DrawIndexed(mEnemyIndexCount, mEnemyIndexOffset, mEnemyVertexOffset);

		// Set per object constants.
		world = XMLoadFloat4x4(&mEnemy2WorldLevel2)*S*shadowOffsetYLevel2;
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		Effects::BasicFX->SetWorld(world);										// Setup the world matrix for drawing the enemy.
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);				// Setup the world inverse transpose matrix.
		Effects::BasicFX->SetWorldViewProj(worldViewProj);						// Setup the world view projection matrix.
		Effects::BasicFX->SetMaterial(mShadowEnemyMaterial);					// Setup the shadow material to be used by the enemy.

		// Bind the transparent blend state object to the output merger stage of the pipeline.
		md3dImmediateContext->OMSetBlendState(RenderStates::TransparentBS, blendFactor, 0xffffffff);
		// Bind the no double blend state object to the output merger stage of the pipeline.
		md3dImmediateContext->OMSetDepthStencilState(RenderStates::NoDoubleBlendDSS, 0);
		// Draw shaders, render states. Update constant buffers.
		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		// Draw the enemy using the index count and offsets to retrieve the relevant data from the buffer.
		md3dImmediateContext->DrawIndexed(mEnemyIndexCount, mEnemyIndexOffset, mEnemyVertexOffset);
	}

	// *** PARTICLE EFFECTS ***
	// Explosion
	mExplosion.SetEyePos(mCam.GetPosition());
	// Is explode activated.
	if (mExplode){
		// Draw the explosion particle effect.
		mExplosion.Draw(md3dImmediateContext, mCam);
	}
	
	// Smoke
	mSmoke.SetEyePos(mCam.GetPosition());
	// Is smoke activated.
	if (mSmokeOn){
		// Draw the smoke particle effect.
		mSmoke.Draw(md3dImmediateContext, mCam);
	}
	
	// Spark
	mSpark.SetEyePos(mCam.GetPosition());
	// Is spark activated.
	if (mSparksOn){
		// Draw the spark particle effect.
		mSpark.Draw(md3dImmediateContext, mCam);
	}
	
	// Fire for four objects. Repeated for the other three fire objects.
	mFire1.SetEyePos(mCam.GetPosition());
	// Is fire activated.
	if (mFire1On){
		// Draw the fire particle effect.
		mFire1.Draw(md3dImmediateContext, mCam);
	}
	// Bind the transparent blend state object to the output merger stage of the pipeline.
	md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);		

	mFire2.SetEyePos(mCam.GetPosition());
	if (mFire2On){
		mFire2.Draw(md3dImmediateContext, mCam);
	}
	md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);

	mFire3.SetEyePos(mCam.GetPosition());
	if (mFire3On){
		mFire3.Draw(md3dImmediateContext, mCam);
	}
	md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);

	mFire4.SetEyePos(mCam.GetPosition());
	if (mFire4On){
		mFire4.Draw(md3dImmediateContext, mCam);
	}
	md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);

	mSky->Draw(md3dImmediateContext, mCam);

	//// Restore default states.
	md3dImmediateContext->RSSetState(0);
	md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);
	md3dImmediateContext->OMSetDepthStencilState(0, 0);

	// Present the back buffer to the screen.
	HR(mSwapChain->Present(0, 0));		
}

// When the mouse button is pressed down.
void MazeGameMain::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

// When the mouse button is released.
void MazeGameMain::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

// When the mouse device is moved around.
void MazeGameMain::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		// Update angles based on input to orbit camera around box.
		mTheta += dx;
		mPhi += dy;

		// Restrict the angle mPhi to approximately 90 degrees so the camera stays above the floor.
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi/2 - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		// Make each pixel correspond to 0.01 unit in the scene.
		float dx = 0.01f*static_cast<float>(x - mLastMousePos.x);
		float dy = 0.01f*static_cast<float>(y - mLastMousePos.y);

		// Update the camera radius based on input.
		mRadius += dx - dy;

		// Restrict the radius. Set the minimum distance at 3.0f and the maximum distance at 10.0f.
		mRadius = MathHelper::Clamp(mRadius, 3.0f, 10.0f);
	}

	// The last mouse position relative to x and y on the screen.
	mLastMousePos.x = x;		
	mLastMousePos.y = y;
}

// Creating the primitives with acquired vertices and indices. Followed by funneling to the vertex and index buffers.
void MazeGameMain::BuildGeometryBuffers()
{
	GeometryGenerator::MeshData floor1;							// Create a floor1 variable of the MeshData structure from the GeometryGenerator class.
	GeometryGenerator::MeshData floor2;							// Create a floor2 variable of the MeshData structure from the GeometryGenerator class.
	GeometryGenerator::MeshData wall;							// Create a wall variable of the MeshData structure from the GeometryGenerator class.
	GeometryGenerator::MeshData lift;							// Create a lift variable of the MeshData structure from the GeometryGenerator class.
	GeometryGenerator::MeshData pole1;							// Create a pole variable of the MeshData structure from the GeometryGenerator class.
	GeometryGenerator::MeshData pole2;							// Create a pole2 variable of the MeshData structure from the GeometryGenerator class.
	GeometryGenerator::MeshData player;							// Create a player variable of the MeshData structure from the GeometryGenerator class.
	GeometryGenerator::MeshData enemy;							// Create a enemy variable of the MeshData structure from the GeometryGenerator class.

	GeometryGenerator geoGen;									// Create an instance of the GeometryGenerator class.

	geoGen.CreateGrid(11.0f, 11.0f, 60, 40, floor1);			// Create a floor1 primitive using the stated values.
	geoGen.CreateGrid(13.0f, 13.0f, 60, 40, floor2);			// Create a floor2 primitive using the stated values.
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, wall);					// Create a wall primitive using the stated values.
	geoGen.CreateGrid(1.0f, 1.0f, 60, 40, lift);				// Create a lift primitive using the stated values.
	geoGen.CreateCylinder(0.075f, 0.075f, 1.5f, 20, 20, pole1);	// Create a pole1 primitive using the stated values.
	geoGen.CreateCylinder(0.075f, 0.075f, 1.5f, 20, 20, pole2); // Create a pole2 primitive using the stated values.
	geoGen.CreateSphere(0.30f, 20, 20, player);					// Create a player primitive using the stated values.
	geoGen.CreateSphere(0.35f, 20, 20, enemy);					// Create a enemy primitive using the stated values.

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	mFloor1VertexOffset = 0;
	mFloor2VertexOffset = floor1.Vertices.size();
	mWallVertexOffset = mFloor2VertexOffset + floor2.Vertices.size();
	mLiftVertexOffset = mWallVertexOffset + wall.Vertices.size();
	mPole1VertexOffset = mLiftVertexOffset + lift.Vertices.size();
	mPole2VertexOffset = mPole1VertexOffset + pole1.Vertices.size();
	mPlayerVertexOffset = mPole2VertexOffset + pole2.Vertices.size();
	mEnemyVertexOffset = mPlayerVertexOffset + player.Vertices.size();

	// Cache the index count of each object.
	mFloor1IndexCount = floor1.Indices.size();
	mFloor2IndexCount = floor2.Indices.size();
	mWallIndexCount = wall.Indices.size();
	mLiftIndexCount = lift.Indices.size();
	mPole1IndexCount = pole1.Indices.size();
	mPole2IndexCount = pole2.Indices.size();
	mPlayerIndexCount = player.Indices.size();
	mEnemyIndexCount = enemy.Indices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	mFloor1IndexOffset = 0; 
	mFloor2IndexOffset = mFloor1IndexOffset + mFloor1IndexCount;
	mWallIndexOffset = mFloor2IndexOffset + mFloor2IndexCount;
	mLiftIndexOffset = mWallIndexOffset + mWallIndexCount;
	mPole1IndexOffset = mLiftIndexOffset + mLiftIndexCount;
	mPole2IndexOffset = mPole1IndexOffset + mPole1IndexCount;
	mPlayerIndexOffset = mPole2IndexOffset + mPole2IndexCount;
	mEnemyIndexOffset = mPlayerIndexOffset + mPlayerIndexCount;

	// The total vertex count acquired through accumulating all the vertice sizes for each meshdata variable. 
	UINT totalVertexCount =
		floor1.Vertices.size() +
		floor2.Vertices.size() +
		wall.Vertices.size() +
		lift.Vertices.size() +
		pole1.Vertices.size() +
		pole2.Vertices.size() +
		player.Vertices.size() +
		enemy.Vertices.size();

	// The total index count acquired through accumulating all the index count values for each meshdata variable.
	UINT totalIndexCount =
		mFloor1IndexCount +
		mFloor2IndexCount +
		mWallIndexCount +
		mLiftIndexCount +
		mPole1IndexCount +
		mPole2IndexCount +
		mPlayerIndexCount +
		mEnemyIndexCount;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//
	std::vector<Vertex::Basic32> vertices(totalVertexCount);

	UINT k = 0;
	// loop through the floor1 vertice size.
	for (size_t i = 0; i < floor1.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = floor1.Vertices[i].Position;			// For each vertex coordinates, store the floor's vertex coordinates.
		vertices[k].Normal = floor1.Vertices[i].Normal;			// For each vertex normal coordinates, store the floor's vertex normal coordinates.
		vertices[k].Tex = floor1.Vertices[i].TexC;				// For each vertex tex coordinates, store the floor's vertex tex coordinates.
	}

	// loop through the floor2 vertice size.
	for (size_t i = 0; i < floor2.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = floor2.Vertices[i].Position;			// For each vertex coordinates, store the floor's vertex coordinates.
		vertices[k].Normal = floor2.Vertices[i].Normal;			// For each vertex normal coordinates, store the floor's vertex normal coordinates.
		vertices[k].Tex = floor2.Vertices[i].TexC;				// For each vertex tex coordinates, store the floor's vertex tex coordinates.
	}

	// loop through the wall vertice size.
	for (size_t i = 0; i < wall.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = wall.Vertices[i].Position;			// For each vertex coordinates, store the floor's vertex coordinates.
		vertices[k].Normal = wall.Vertices[i].Normal;			// For each vertex normal coordinates, store the floor's vertex normal coordinates.
		vertices[k].Tex = wall.Vertices[i].TexC;				// For each vertex tex coordinates, store the floor's vertex tex coordinates.
	}

	// loop through the lift vertice size.
	for (size_t i = 0; i < lift.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = lift.Vertices[i].Position;			// For each vertex coordinates, store the lift's vertex coordinates.
		vertices[k].Normal = lift.Vertices[i].Normal;			// For each vertex normal coordinates, store the lift's vertex normal coordinates.
		vertices[k].Tex = lift.Vertices[i].TexC;				// For each vertex tex coordinates, store the lift's vertex tex coordinates.
	}

	// loop through the pole1 vertice size.
	for (size_t i = 0; i < pole1.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = pole1.Vertices[i].Position;			// For each vertex coordinates, store the pole's vertex coordinates.
		vertices[k].Normal = pole1.Vertices[i].Normal;			// For each vertex normal coordinates, store the pole's vertex normal coordinates.
		vertices[k].Tex = pole1.Vertices[i].TexC;				// For each vertex tex coordinates, store the pole's vertex tex coordinates.
	}

	// loop through the pole2 vertice size.
	for (size_t i = 0; i < pole2.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = pole2.Vertices[i].Position;			// For each vertex coordinates, store the pole's vertex coordinates.
		vertices[k].Normal = pole2.Vertices[i].Normal;			// For each vertex normal coordinates, store the pole's vertex normal coordinates.
		vertices[k].Tex = pole2.Vertices[i].TexC;				// For each vertex tex coordinates, store the pole's vertex tex coordinates.
	}

	// loop through the player vertice size.
	for (size_t i = 0; i < player.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = player.Vertices[i].Position;			// For each vertex coordinates, store the floor's vertex coordinates.
		vertices[k].Normal = player.Vertices[i].Normal;			// For each vertex normal coordinates, store the floor's vertex normal coordinates.
		vertices[k].Tex = player.Vertices[i].TexC;				// For each vertex tex coordinates, store the floor's vertex tex coordinates.
	}

	// loop through the enemy vertice size.
	for (size_t i = 0; i < enemy.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = enemy.Vertices[i].Position;			// For each vertex coordinates, store the floor's vertex coordinates.
		vertices[k].Normal = enemy.Vertices[i].Normal;			// For each vertex normal coordinates, store the floor's vertex normal coordinates.
		vertices[k].Tex = enemy.Vertices[i].TexC;				// For each vertex tex coordinates, store the floor's vertex tex coordinates.
	}
		
	// Define the vertex buffer
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;							// The contents will not change after creation.
	vbd.ByteWidth = sizeof(Vertex::Basic32) * totalVertexCount;	// The size in bytes of the vertex buffer to be created.
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;					// For a vertex buffer.
	vbd.CPUAccessFlags = 0;										// No read or write access from the CPU after creation.
	vbd.MiscFlags = 0;											// Will not be required.
	D3D11_SUBRESOURCE_DATA vinitData;							// A system memory array to contain the data for intialising
																// the vertex buffer.
	vinitData.pSysMem = &vertices[0];
	// Create the vertex buffer.
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVertexBuffer));

	// Pack the indices of all the meshes into one index buffer.	
	std::vector<UINT> indices;
	indices.insert(indices.end(), floor1.Indices.begin(), floor1.Indices.end());
	indices.insert(indices.end(), floor2.Indices.begin(), floor2.Indices.end());
	indices.insert(indices.end(), wall.Indices.begin(), wall.Indices.end());
	indices.insert(indices.end(), lift.Indices.begin(), lift.Indices.end());
	indices.insert(indices.end(), pole1.Indices.begin(), pole1.Indices.end());
	indices.insert(indices.end(), pole2.Indices.begin(), pole2.Indices.end());
	indices.insert(indices.end(), player.Indices.begin(), player.Indices.end());
	indices.insert(indices.end(), enemy.Indices.begin(), enemy.Indices.end());

	// Define the index buffer
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;							 // The contents will not change after creation.
	ibd.ByteWidth = sizeof(UINT) * totalIndexCount;				 // The size in bytes of the index buffer to be created.
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;					 // For an index buffer.
	ibd.CPUAccessFlags = 0;										 // No read or write access from the CPU after creation.
	ibd.MiscFlags = 0;											 // Will not be required.
	D3D11_SUBRESOURCE_DATA iinitData;							 // A system memory array to contain the data for intialising
																 // the index buffer.
	iinitData.pSysMem = &indices[0];
	// Create the index buffer.
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIndexBuffer));
}
