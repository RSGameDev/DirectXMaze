//**************************************************************************************************************************
// Title: Enemy.h (The enemy header, adapted from. My own code specified in Task B)
// Author: Train2Game course materials
// Date: 2014
// Page: 31
// Book: Train2Game Module 7 Game Programming 101
//**************************************************************************************************************************

#pragma once

#include "Graph.h"

class Enemy
{
public:
	Enemy(Graph *g, int n);				// The enemy constructor that takes in a graph and integer parameter. The integer indicates the enemy number it is (the game has 3 enemies in total). This is key for setting up the patrol points and further.
	XMFLOAT3 Tick(float dt);			// This is where all the enemy control logic lives, return true if we spot the player - game lose condition.

	void ResetPatrolPoints();			// Reset the patrol points for the enemy.

	XMFLOAT3 enemy3DPosition;			// The position of the enemy using a 3D expression.

private:
	Enemy();							// Enenmy constructor.
	Graph *mGraph = nullptr;			// A pointer to a graph instance and set to null.

	vector<IntVec2> mPatrolPoints;		 // A vector of points on our patrol, we start at the first, then go to the second and so on. This is for the first level enemy.
	vector<IntVec2> mPatrolPointsEnemy1; // A vector of points on our patrol, we start at the first, then go to the second and so on. This is for the second level enemy.
	vector<IntVec2> mPatrolPointsEnemy2; // A vector of points on our patrol, we start at the first, then go to the second and so on. This is for the third level enemy.

	int mHeadingToPatrolPoint;			// The next point on our patrol.

	vector<IntVec2> mRoute;				// Our current route - a route takes us from one patrol point to another and is calculated using Drijkstra.
	unsigned int mRouteIndex;			// Our current index in the route, this would start and 0 and keep incrementing till we reach the last point in the route meaning we arrived.

	IntVec2 mPosition;					// The actual location of the enemy in pixels on screen - not maze coordinates.
	IntVec2 mMoveVec;					// Direction enemy is currently moving.
	IntVec2 mMoveTarget;				// Their target in pixels - not maze coordinates.

	// These two functions convert to and from maze coordinates/pixel coordinates.
	IntVec2 GetMazeTile(IntVec2 pixelPos);
	IntVec2 GetPixelPosition(IntVec2 mazePos);

	//Bump headingToPatrolPoint and generate a route to the new destination.
	void PlanRouteToNextPatrolPoint();
	
	int mNumber;						// The number storing the integer in the constructors parameter. This indicates the enemy number it is, this could be from 1-3.

	bool mEnemy1 = false;				// This makes clear which enemy is in question, the enemy that will be returning a 3D vector position to the main class.
	bool mEnemy2 = false;				// This makes clear which enemy is in question, the enemy that will be returning a 3D vector position to the main class.
	bool mEnemy3 = false;				// This makes clear which enemy is in question, the enemy that will be returning a 3D vector position to the main class.
};