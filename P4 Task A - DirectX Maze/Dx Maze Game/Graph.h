//**************************************************************************************************************************
// Title: Graph.h (The graph header, adapted from. My own code specified in Task B)
// Author: Train2Game course materials
// Date: 2014
// Page: 31
// Book: Train2Game Module 7 Game Programming 101
//**************************************************************************************************************************

#pragma once

#include <string>
#include <vector>
#include "MathHelper.h"
using namespace std;


class RenderInterface;

enum Direction { NORTH = 0, EAST, WEST, SOUTH };

// simple integer 2d vector class
class IntVec2
{
public:
	int x, y;
	IntVec2() { x = 0; y = 0; }
	IntVec2(int xVal, int yVal) { x = xVal; y = yVal; }
	bool operator == (const IntVec2& V) const { return (V.x == x) && (V.y == y); }
	bool operator != (const IntVec2& V) const { return (V.x != x) || (V.y != y); }
	IntVec2& operator= (const IntVec2& V) { x = V.x; y = V.y; return *this; }
	IntVec2& operator+= (const IntVec2& V) { x += V.x; y += V.y; return *this; }
	IntVec2& operator-= (const IntVec2& V) { x -= V.x; y -= V.y; return *this; }
	IntVec2& operator*= (int S) { x *= S; y *= S; return *this; }
	IntVec2& operator/= (int S) { x /= S; y /= S; return *this; }
};
// Binary operators
IntVec2 operator+ (const IntVec2& V1, const IntVec2& V2);
IntVec2 operator- (const IntVec2& V1, const IntVec2& V2);
IntVec2 operator* (const IntVec2& V, int S);
IntVec2 operator* (int S, const IntVec2& V);

// the tile structure we use to represent the maze

struct Tile
{
	IntVec2 backTrack; // used to store the path 
	IntVec2 forwardTrack; // used by Wall Follower to store the forward path
	int weight;
	bool isSolid : 1; // this tile is a solid wall
	bool visited : 1;
	bool path : 1; // tiles is marked as path - used for rendering only
	bool wallFollowPath : 1;
};

class Graph
{
public:
	Graph(int w, int h, int pps);
	~Graph() { delete mMaze; }

	void RegenerateMaze();
	
	vector<XMFLOAT3> StoreWallPosition();

	// initialise a Dijkstra search from p1 to p2
	void InitDijkstra(IntVec2 p1, IntVec2 p2);
	bool StepDijkstra(); // returns false if no path
	bool IsDijkstraFinished(vector<IntVec2> &ret); // returns true if we reached the destination
	void RunDijkstra(IntVec2 p1, IntVec2 p2, vector<IntVec2> &ret);
	int DijkstraNodesVisited = 0;

	int GetWidth() { return mW; }
	int GetHeight() { return mH; }
	int GetPPS() { return mPixelsPerSquare; }

	IntVec2 GetForwardDirection(Direction dir);

	bool IsSolid(IntVec2 pos) { return mMaze[pos.y*mW + pos.x].isSolid; }
	Tile &GetTile(IntVec2 pos) { return mMaze[pos.y*mW + pos.x]; }

private:
	Graph(); // hide the default constructor

	int mW, mH;
	Tile *mMaze = nullptr;
	int mPixelsPerSquare = 0;

	IntVec2 mSearchStart, mSearchEnd; // current search start and end points

	bool CheckTileValid(IntVec2 &v);
	void RecurseMazeGenerate(IntVec2 &v);
	bool IsVisited(IntVec2 pos) { return mMaze[pos.y*mW + pos.x].visited; }

	void ResetVisited();
};