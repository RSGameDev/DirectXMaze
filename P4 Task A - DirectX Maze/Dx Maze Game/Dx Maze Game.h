//************************************************************************************************************************
// Title: ShapesDemo.h (Template for my main class of which has been expanded upon using my own code. Specified in Task B)
// Author: Frank Luna 
// Date: 2012
// Page: 227 
// Book: Introduction to 3D Game Programming with Directx 11
//************************************************************************************************************************

#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "Effects.h"
#include "Lighting.h"
#include "Vertex.h"
#include "Camera.h"
#include "Graph.h"
#include "Enemy.h"
#include "ParticleSystem.h"
#include "RenderStates.h"
#include "Sky.h"

class MazeGameMain : public D3DApp
{
public:
	MazeGameMain(HINSTANCE hInstance);					// Constructor.
	~MazeGameMain();									// Destructor.
		
	bool Init();										// Initialise.
	void OnResize();									// Resize window.
	void UpdateScene(float dt);							// Update scene with a delta time parameter to keep the
														// rendering succinct.
	void DrawScene();									// Draw the scene.

	void OnMouseDown(WPARAM btnState, int x, int y);	// When the mouse button is pressed down.
	void OnMouseUp(WPARAM btnState, int x, int y);		// When the mouse button is released.
	void OnMouseMove(WPARAM btnState, int x, int y);	// When the mouse device is moved around.

private:
	void BuildGeometryBuffers();						// Creating the primitives with acquired vertices and
	                                                    // indices. Followed by funneling to the vertex and index 
														// buffers.
	void BuildFX();										// Initialise the fx file for the project.
	void BuildVertexLayout();							// Prepare communications from this classes structure to 
														// the colour effect file.
	
	bool LiftCollision(XMFLOAT3 pos);							// A function that returns true or false if the player has collided with the lift.
	bool EndOfGameCollision(XMFLOAT3 pos);						// A function that returns true or false if the player has reached the end of level 2.
	bool PlayerWallCollisionLv1(XMFLOAT3 pos);					// A function that returns true or false if the player has collided with a wall on the first level.
	bool PlayerWallCollisionLv2(XMFLOAT3 pos);					// A function that returns true or false if the player has collided with a wall on the second level.
	bool EnemyCollision(XMFLOAT3 pos, XMFLOAT3 posEnemy);		// A function that returns true or false if the player has collided with the enemy.
	void WallDamageToPlayer(float dt);							// A function that for when the player hits a wall.
	bool FirePlayerCollision(XMFLOAT3 pos, XMFLOAT3 posFire);	// A function that returns true or false if the player collides with the fire particle effect.
	void PlayerRespawn(float dt);								// A function that respawns the player depending on the level they are on.
	void GameRestart();											// A function that returns that restarts the game bringing the player to the start of level 1.

	bool mIsLiftInStartPosition = true;					// Is the lift at its starting position, level 1.
	bool mIsLiftMoving = false;							// Is the lift moving, travelling to level 2.
	bool mIsGameCompleted = false;						// Is the game completed, player reached end of level 2.

private:	

	Sky* mSky;

	ID3D11Buffer* mSkySphereVB;
	ID3D11Buffer* mSkySphereIB;

	ID3D11Buffer* mVertexBuffer;						// A pointer for the buffer that will contain all the vertices.
	ID3D11Buffer* mIndexBuffer;							// A pointer for the buffer that will contain all the indices.

	ID3D11ShaderResourceView* mFloorMapSRV;				// A pointer to store the floor texture.	
	ID3D11ShaderResourceView* mWallMapSRV;				// A pointer to store the wall texture.	
	ID3D11ShaderResourceView* mLiftMapSRV;				// A pointer to store the lift texture.
	ID3D11ShaderResourceView* mPoleMapSRV;				// A pointer to store the pole texture.	
	ID3D11ShaderResourceView* mPlayerMapSRV;			// A pointer to store the player texture.	
	ID3D11ShaderResourceView* mEnemyMapSRV;				// A pointer to store the enemy texture.	

	ID3D11ShaderResourceView* mRandomTexSRV;			// A pointer to store a random texture.	
	ID3D11ShaderResourceView* mExplosionTexSRV;			// A pointer to store the explosion texture.
	ID3D11ShaderResourceView* mSmokeTexSRV;				// A pointer to store the smoke texture.	
	ID3D11ShaderResourceView* mSparkTexSRV;				// A pointer to store the spark texture.	
	ID3D11ShaderResourceView* mFireTexSRV;				// A pointer to store the fire texture.

	ParticleSystem mExplosion;							// A particle system instance for an explosion effect.	
	ParticleSystem mSmoke;								// A particle system instance for a smoke effect.
	ParticleSystem mSpark;								// A particle system instance for a spark effect.
	ParticleSystem mFire1;								// A particle system instance for a fire effect.
	ParticleSystem mFire2;								// A particle system instance for a fire effect.
	ParticleSystem mFire3;								// A particle system instance for a fire effect.
	ParticleSystem mFire4;								// A particle system instance for a fire effect.
	
	ID3DX11Effect* mEffect;								// A pointer to the effect file.
	ID3DX11EffectTechnique* mTechnique;					// A pointer for the technique within the effect file.
	ID3DX11EffectMatrixVariable* mEffectWorldViewProj;	// A pointer to the constand buffer variable in colour.fx.

	ID3D11InputLayout* mInputLayout;					// A pointer to the Input Layout.
		
	DirectionalLight mDirectionalLight[3];				// An array of directional lights.
	Material mFloorMaterial;							// The floor material.
	Material mWallMaterial;								// The wall material.
	Material mLiftMaterial;								// The lift material.
	Material mPoleMaterial;								// The pole material.
	Material mPlayerMaterial;							// The player material.
	Material mEnemyMaterial;							// The enemy material.
	Material mShadowWallMaterial;						// The shadow material the wall.
	Material mShadowPlayerMaterial;						// The shadow material the player.
	Material mShadowEnemyMaterial;						// The shadow material the enemy.

	// Define transformations from local spaces to world space.	
	XMFLOAT4X4 mFloorWorldLevel1;						// The floor world matrix.
	XMFLOAT4X4 mFloorWorldLevel2;						// The floor level 2 world matrix.
	vector<XMFLOAT4X4> mWallWorldLevel1;				// A container for the wall world matrices.
	vector<XMFLOAT4X4> mWallWorldLevel2;				// A container for the wall world matrices for level 2.
	XMFLOAT4X4 mLiftWorld;								// The lift world matrix.
	XMFLOAT4X4 mPoleWorldLevel1;						// The pole world matrix.		
	XMFLOAT4X4 mPoleWorldLevel2;						// The pole world matrix for level 2.
	XMFLOAT4X4 mPlayerWorld;							// The player world matrix.
	XMFLOAT4X4 mEnemyWorldLevel1;						// The enemy world matrix.
	XMFLOAT4X4 mEnemy1WorldLevel2;						// The first enemy for level 2, world matrix.
	XMFLOAT4X4 mEnemy2WorldLevel2;						// The second enemy for level 2, world matrix.
		
	XMFLOAT4X4 mTexTransform;							// The texture transform matrix.
	XMFLOAT4X4 mFloorTexTransform;						// The floor texture transform matrix.

	XMFLOAT4X4 mView;									// The view matrix.
	XMFLOAT4X4 mProj;									// The projection matrix.
	
	int mFloor1VertexOffset;							// The floor vertex offset set in the vertex buffer.
	int mFloor2VertexOffset;							// The floor vertex offset for level 2, set in the vertex buffer.
	int mWallVertexOffset;								// The wall vertex offset set in the vertex buffer.
	int mLiftVertexOffset;								// The lift vertex offset set in the vertex buffer.
	int mPole1VertexOffset;								// The pole vertex offset set in the vertex buffer.
	int mPole2VertexOffset;								// The pole vertex offset for level 2, set in the vertex buffer.
	int mPlayerVertexOffset;							// The player vertex offset set in the vertex buffer.
	int mEnemyVertexOffset;								// The enemy vertex offset set in the vertex buffer.
	
	UINT mFloor1IndexOffset;							// The floor index offset set in the index buffer.
	UINT mFloor2IndexOffset;							// The 2nd floor index offset set in the index buffer.
	UINT mWallIndexOffset;								// The wall index offset set in the index buffer.
	UINT mLiftIndexOffset;								// The lift index offset set in the index buffer.
	UINT mPole1IndexOffset;								// The pole index offset set in the index buffer.
	UINT mPole2IndexOffset;								// The 2nd pole index offset set in the index buffer.
	UINT mPlayerIndexOffset;							// The player index offset set in the index buffer.
	UINT mEnemyIndexOffset;								// The enemy index offset set in the index buffer.
	
	UINT mFloor1IndexCount;								// The floor index count set in the index buffer.
	UINT mFloor2IndexCount;								// The 2nd floor index count set in the index buffer.
	UINT mWallIndexCount;								// The wall index count set in the index buffer.
	UINT mLiftIndexCount;								// The lift index count set in the index buffer.
	UINT mPole1IndexCount;								// The pole index count set in the index buffer.
	UINT mPole2IndexCount;								// The 2nd pole index count set in the index buffer.
	UINT mPlayerIndexCount;								// The player index count set in the index buffer.
	UINT mEnemyIndexCount;								// The enemy index count set in the index buffer.

	XMFLOAT3 mEyePosW;											// A variable for the eye position for the game.
	XMFLOAT3 mPlayerPosition;									// Coordinates of the player.	
	XMFLOAT3 mLastPlayerPosition;								// Coordinate of the players' last position before the last keyboard input.
	XMFLOAT3 mDeathPlayerPosition;								// Coordinates for where the player died.
	XMFLOAT3 mPlayerSpawnLevel1 = XMFLOAT3(1.0f, 0.5f, 1.0f);	// The first level spawn point for the player.
	XMFLOAT3 mPlayerSpawnLevel2 = XMFLOAT3(9.0f, 14.5f, 9.0f);  // The second level spawn point for the player.
	vector<XMFLOAT3> mWallPositionLevel1;						// A container for the wall positions.
	vector<XMFLOAT3> mWallPositionLevel2;						// A container for the wall positions for level 2.
	XMFLOAT3 mEnemy1NewPositionLevel1;							// A variable that holds enemy one's new position for level 1.
	XMFLOAT3 mEnemy1NewPositionLevel2;							// A variable that holds enemy two's new position for level 2.
	XMFLOAT3 mEnemy2NewPositionLevel2;							// A variable that holds enemy three's new position for level 2.
	XMFLOAT3 mLiftLevel1 = XMFLOAT3(9.0f, 0.1f, 9.0f);			// The 3D vector for where the first level lift is positioned.
	XMFLOAT3 mEndOfGame = XMFLOAT3(7.0f, 14.75f, 15.0f);		// The 3D vector for where the end of game position is.
	XMFLOAT3 mLiftPosition;										// Coordinates for the lift.	
	XMFLOAT3 mPole2Position;									// Coordinates for the 2nd pole on level 2.	
	

	float mTheta;										// Mathematic equations to allow for mouse device functioning.
	float mPhi;											// Mathematic equations to allow for mouse device functioning.
	float mRadius;										// Mathematic equations to allow for mouse device functioning.

	POINT mLastMousePos;								// The last mouse position relative to x and y on the screen.

	Camera mCam;											// The camera instance for the player.
	Graph *mGraphLevel1;										// A pointer to the first level graph instance.
	Graph *mGraphLevel2;										// A pointer to the second level graph instance.
	Enemy *mEnemyLevel1;										// A pointer to the first enemy instance.
	Enemy *mEnemy1Level2;									// A pointer to the second enemy instance.
	Enemy *mEnemy2Level2;									// A pointer to the third enemy instance.

	int mPlayerHealth = 100;								// The players health amount.
	float mPlayerRedValue = 0.6f;							// The red value from the RGB on the players material.	
	float mPlayerBlueValue = 0.8f;							// The blue value from the RGB on the players material.
	float mPlayerGreenValue = 0.9f;							// The green value from the RGB on the players material.

	// Calculation: RGB value above, difference from 1.0 or 0.0. Divide by 100 to get 1%.
	// This is then used when the player collides with a wall.
	float mRedDamage = 0.004;							    // This is the current red value from above, calculating the difference from 1.0 (as this number refers to a red colour). This number is then divided by 100 to get 1% of this number.
	float mBlueDamage = 0.008;								// This is the current blue value from above, calculating the difference from 0.0 (as I want a strong red colour). This number is then divided by 100 to get 1% of this number.
	float mGreenDamage = 0.009;								// This is the current green value from above, calculating the difference from 0.0 (as I want a strong red colour). This number is then divided by 100 to get 1% of this number.

	bool mIsEnemyCollidedLevel1 = false;					// Has the enemy collided with the player on level 1.
	bool mIsEnemy1CollidedLevel2 = false;					// Has the enemy collided with the player on level 2.
	bool mIsEnemy2CollidedLevel2 = false;					// Has the other enemy collided with the player on level 2.

	bool mIsFire1Collided = false;							// has the fire particle effect had the player collided with it.
	bool mIsFire2Collided = false;							// has the fire particle effect had the player collided with it.
	bool mIsFire3Collided = false;							// has the fire particle effect had the player collided with it.
	bool mIsFire4Collided = false;							// has the fire particle effect had the player collided with it.

	XMFLOAT3 mFire1Pos = XMFLOAT3(11.0f, 14.3f, 9.0f);		// Positions for the individual fire particle effects.
	XMFLOAT3 mFire2Pos = XMFLOAT3(17.0f, 14.3f, 5.0f);		// Positions for the individual fire particle effects.
	XMFLOAT3 mFire3Pos = XMFLOAT3(12.0f, 14.3f, 11.0f);		// Positions for the individual fire particle effects.
	XMFLOAT3 mFire4Pos = XMFLOAT3(15.0f, 14.3f, 14.0f);		// Positions for the individual fire particle effects.
	
	bool mExplode = false;									// true or false variables for every particle effect in the game. Which switch to true when functioning.
	bool mSmokeOn = false;									// true or false variables for every particle effect in the game. Which switch to true when functioning.
	bool mSparksOn = false;									// true or false variables for every particle effect in the game. Which switch to true when functioning.
	bool mFire1On = false;									// true or false variables for every particle effect in the game. Which switch to true when functioning.
	bool mFire2On = false;									// true or false variables for every particle effect in the game. Which switch to true when functioning.
	bool mFire3On = false;									// true or false variables for every particle effect in the game. Which switch to true when functioning. 
	bool mFire4On = false;									// true or false variables for every particle effect in the game. Which switch to true when functioning. 
	
	float mTemp1 = 0.0f;									// Temporary variable for reseting the fire particle effects to a timer.	
	float mTemp2 = 0.0f;									// Temporary variable for reseting the fire particle effects to a timer.
	float mTemp3 = 0.0f;									// Temporary variable for reseting the fire particle effects to a timer.
	float mTemp4 = 0.0f;									// Temporary variable for reseting the fire particle effects to a timer.
};

