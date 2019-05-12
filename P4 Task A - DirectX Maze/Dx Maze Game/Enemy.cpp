//**************************************************************************************************************************
// Title: Enemy.cpp (The enemy class, adapted from. My own code specified in Task B)
// Author: Train2Game course materials
// Date: 2014
// Page: 31
// Book: Train2Game Module 7 Game Programming 101
//**************************************************************************************************************************

#include "Enemy.h"

#define NUM_PATROL_POINTS 3

Enemy::Enemy(Graph *g, int n)
{
	// store the passed parameters
	mGraph = g;
	mNumber = n;

	// Checks if the number passed through the constructors' parameter is 1. This indicates the enemy in the game it is.
	if (mNumber == 1){
		mEnemy1 = true;							// This instance is enemy number 1.
	}

	// Checks if the number passed through the constructors' parameter is 2. This indicates the enemy in the game it is.
	if (mNumber == 2){
		mEnemy2 = true;							// This instance is enemy number 2.
	}

	// Checks if the number passed through the constructors' parameter is 3. This indicates the enemy in the game it is.
	if (mNumber == 3){
		mEnemy3 = true;							// This instance is enemy number 3.
	}
	
	ResetPatrolPoints();
}

void Enemy::ResetPatrolPoints()
{
	// clear out the patrol points vector
	mPatrolPoints.clear();
	
	// Checks the enemy number/indentifier it is.
	if (mEnemy1 == true){

		// Enemy level 1 patrol points.
		IntVec2 patrolPoint1 = IntVec2(3, 7);
		IntVec2 patrolPoint2 = IntVec2(6, 5);
		IntVec2 patrolPoint3 = IntVec2(3, 9); 

		// Add the patrol points to the end of the vector container.
		mPatrolPoints.push_back(patrolPoint1);
		mPatrolPoints.push_back(patrolPoint2);
		mPatrolPoints.push_back(patrolPoint3);
	}

	// Checks the enemy number/indentifier it is.
	if (mEnemy2 == true){

		// Enemy Level 2 patrol points.
		IntVec2 patrolPoint1 = IntVec2(11, 10);
		IntVec2 patrolPoint2 = IntVec2(9, 6);
		IntVec2 patrolPoint3 = IntVec2(7, 4);

		mPatrolPoints.push_back(patrolPoint1);
		mPatrolPoints.push_back(patrolPoint2);
		mPatrolPoints.push_back(patrolPoint3);
	}

	// Checks the enemy number/indentifier it is.
	if (mEnemy3 == true){

		// Enemy Level 3 patrol points.
		IntVec2 patrolPoint1 = IntVec2(3, 11);
		IntVec2 patrolPoint2 = IntVec2(5, 7);
		IntVec2 patrolPoint3 = IntVec2(2, 7);

		mPatrolPoints.push_back(patrolPoint1);
		mPatrolPoints.push_back(patrolPoint2);
		mPatrolPoints.push_back(patrolPoint3);
	}	

	// with our patrol randomly generated, let's start the enemy AI at the first and direct it to the second
	mHeadingToPatrolPoint = 0;
	PlanRouteToNextPatrolPoint();
}

// two helper functions
// this function needs to take a maze coordinate and return the centre of that square in pixels
// Hint: There's the same function in player.cpp - copy and fix that

IntVec2 Enemy::GetPixelPosition(IntVec2 mazePos)
{
	int pps = mGraph->GetPPS();
	int x = mazePos.x*pps + pps / 2;
	int y = mazePos.y*pps + pps / 2;
	return IntVec2(x, y);
}

// this function needs to take a pixel coordinate and return the maze coordinate
// Hint: There's the same function in player.cpp - copy and fix that

IntVec2 Enemy::GetMazeTile(IntVec2 pixelPos)
{
	int pps = mGraph->GetPPS();
	int mx = pixelPos.x / pps;
	int my = pixelPos.y / pps;
	return IntVec2(mx, my);
}

// this function needs to increase headingToPatrolPoint making sure it doesn't wrap
// we then need to plan a route from the last patrol point to this one and position the enemy at the first point
void Enemy::PlanRouteToNextPatrolPoint()
{
	// store the current patrol start point
	IntVec2 startPatrol = mPatrolPoints[mHeadingToPatrolPoint];
	// increase headingToPatrolPoint
	mHeadingToPatrolPoint++;
	// check it didn't go out of bounds using NUM_PATROL_POINTS
	if (mHeadingToPatrolPoint >= NUM_PATROL_POINTS)
		mHeadingToPatrolPoint = 0;
	IntVec2 endPatrol = mPatrolPoints[mHeadingToPatrolPoint];
	// calculate a route between the two points
	mGraph->RunDijkstra(startPatrol, endPatrol, mRoute);
	// reset the enemy to the first point on the route - remembering the difference between maze positions and on screen pixel positions
	mPosition = GetPixelPosition(mRoute[0]);
	// Store the 2D vector as a 3D vector.
	enemy3DPosition = XMFLOAT3(mPosition.x, 0.0f, mPosition.y);
	// reset routeIndex to the first point in the list
	mRouteIndex = 0;
	// clear the moveVec and moveTarget
	mMoveVec = IntVec2(0, 0);
	mMoveTarget = mPosition; // this will force an update in Tick()
}

// The tick function is where a lot of the enemy AI work is done

XMFLOAT3 Enemy::Tick(float dt)
{
	// start by checking if we reached our next route position
	if (mPosition == mMoveTarget)
	{
		// we reached our next route position
		// increment routeIndex and if we wrap call PlanRouteToNextPatrolPoint()
		// Note: to get the number of squares in a route we use route.size()
		mRouteIndex++;
		if (mRouteIndex >= mRoute.size())
		{
			PlanRouteToNextPatrolPoint();
			mRouteIndex = 1;
		}
		// now we need to figure out where we're going and what movement vector we need to get there
		IntVec2 newSquare = mRoute[mRouteIndex];
		mMoveVec = newSquare - GetMazeTile(mPosition);
		// set moveTarget to the pixel position we're headingfor
		mMoveTarget = GetPixelPosition(newSquare);
	}
	// okay now we need to move in moveVec direction till we hit moveTarget
		
	// Move into moveVec direction using delta time to keep in sync with the system.
	enemy3DPosition.x += (float)mMoveVec.x*dt;
	enemy3DPosition.z += (float)mMoveVec.y*dt;

	// Subtract from the target destination thhe enemies x and z position.
	float distanceX = abs((float)mPosition.x - enemy3DPosition.x);
	float distanceZ = abs((float)mPosition.y - enemy3DPosition.z);

	// Check if the enemy has reached it's destination.
	if (distanceX >= 1.0f || distanceZ >= 1.0f){
		mPosition = mMoveTarget;
	}
	
	// If this instance is enemy1. Return the XMFLOAT3 value for the main class to use, specific to the enemy1.
	if (mEnemy1 == true){
		XMFLOAT3 ret = XMFLOAT3(enemy3DPosition.x, 0.5f, enemy3DPosition.z);
		return ret;
	}
	
	// If this instance is enemy2. Return the XMFLOAT3 value for the main class to use, specific to the enemy2.
	if (mEnemy2 = true){
		XMFLOAT3 ret = XMFLOAT3(enemy3DPosition.x + 6.0f, 14.5f, enemy3DPosition.z + 4.0f);
		return ret;
	}

	// If this instance is enemy3. Return the XMFLOAT3 value for the main class to use, specific to the enemy3.
	if (mEnemy3 = true){
		XMFLOAT3 ret = XMFLOAT3(enemy3DPosition.x + 6.0f, 14.5f, enemy3DPosition.z + 4.0f);
		return ret;
	}	
}
