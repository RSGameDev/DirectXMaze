//**************************************************************************************************************************
// Title: Graph.cpp (The graph class, adapted from. My code insert begins at line 89 to 107.)
// Author: Train2Game course materials
// Date: 2014
// Page: 31
// Book: Train2Game Module 7 Game Programming 101
//**************************************************************************************************************************

#include "Graph.h"

#include <algorithm>

IntVec2 operator+ (const IntVec2& V1, const IntVec2& V2) { IntVec2 ret(V1.x, V1.y); return ret += V2; }
IntVec2 operator- (const IntVec2& V1, const IntVec2& V2) { IntVec2 ret(V1.x, V1.y); return ret -= V2; }
IntVec2 operator* (const IntVec2& V, int S) { IntVec2 ret(V.x, V.y); return ret *= S; }
IntVec2 operator* (int S, const IntVec2& V) { IntVec2 ret(V.x, V.y); return ret *= S; }

Graph::Graph(int wMaze, int hMaze, int pps)
{
	mPixelsPerSquare = pps;
	mW = wMaze;
	mH = hMaze;

	mMaze = new Tile[mW*mH];

	RegenerateMaze();
}

void Graph::RegenerateMaze()
{
	// we're using the maze generator algorithm
	// start by filling the maze with solid walls and reseetting the visited flags
	for (int i = 0; i < mW*mH; i++)
	{
		mMaze[i].isSolid = true;
		mMaze[i].visited = false;
	}
	// pick a random starting position - needs to be odd as the walls are on evens and needs to avoid being on the wall
	int x = (rand() % (mW - 2)) | 1;
	int y = (rand() % (mH - 2)) | 1;
	// call the maze generator
	IntVec2 v(x, y);
	RecurseMazeGenerate(v);
	ResetVisited();
}

// tile is valid if in bounds and not visited

bool Graph::CheckTileValid(IntVec2 &v)
{
	if ((v.x < 0) || (v.x >= mW) || (v.y < 0) || (v.y >= mH))
		return false;
	return !IsVisited(v);
}

void Graph::RecurseMazeGenerate(IntVec2 &v)
{
	// locate a random neighbouring cell that hasn't been visited
	vector<IntVec2> possibles; // 4 possible directions
	possibles.push_back(IntVec2(0, -1));
	possibles.push_back(IntVec2(1, 0));
	possibles.push_back(IntVec2(0, 1));
	possibles.push_back(IntVec2(-1, 0));

	// randomise the list using a neat std function
	random_shuffle(possibles.begin(), possibles.end());

	// iterate the list going in that direction
	for (unsigned int i = 0; i < possibles.size(); i++)
	{
		// check the direction is valid
		if (CheckTileValid(v + 2 * possibles[i]))
		{
			// mark squares in this direction
			IntVec2 newPos = v + possibles[i];
			mMaze[newPos.y*mW + newPos.x].isSolid = false;
			mMaze[newPos.y*mW + newPos.x].visited = true;
			newPos += possibles[i];
			mMaze[newPos.y*mW + newPos.x].isSolid = false;
			mMaze[newPos.y*mW + newPos.x].visited = true;
			// recurse
			RecurseMazeGenerate(newPos);
		}
	}
}

// ****** Code I added to this class in order to retrieve the wall positions *************************

vector<XMFLOAT3> Graph::StoreWallPosition()
{
	vector<XMFLOAT3> ret;
	// start by rendering the walls
	for (int x = 0; x < mH; x++)
	{		
		for (int z = 0; z < mW; z++)
		{
			IntVec2 v(x, z);
			// as a slight optimisation we look for runs of walls and merge them into single blocks
			if (IsSolid(v))
			{
				XMFLOAT3 position = XMFLOAT3((float)x, 0.5f, (float)z);
				ret.push_back(position);
			}
		}
	}
	return ret;
}
// ****************************************************************************************************

#define WEIGHT_INFINITY 0x7fffffff // largest possible value for a signed 32 bit integer

void Graph::InitDijkstra(IntVec2 p1, IntVec2 p2)
{
	// set the weight of the start square to 0, the rest to infinity and mark all squares as unvisited
	for (int i = 0; i < mW*mH; i++)
	{
		mMaze[i].weight = WEIGHT_INFINITY;
		mMaze[i].visited = false;
		mMaze[i].path = false;
	}
	GetTile(p1).weight = 0;
	GetTile(p1).backTrack = p1;
	// store the start and end points
	mSearchStart = p1;
	mSearchEnd = p2;
	//DijkstraNodesVisited = 0;
}

bool Graph::StepDijkstra()
{
	IntVec2 candidate(-1, -1);
	int minWeight = WEIGHT_INFINITY;
	// the first step in each step is to find the lowest unvisited node and go from there
	for (int y = 0; y < mH; y++)
	{
		for (int x = 0; x < mW; x++)
		{
			IntVec2 v(x, y);
			if ((!IsSolid(v)) && (!IsVisited(v)))
			{ // not solid and not visited
				int weight = GetTile(v).weight;
				if (weight < minWeight)
				{ // replace minWeight
					candidate = v;
					minWeight = weight;
				}
			}
		}
	}
	// did we find a candidate - if not there's no path to the destination
	if (minWeight == WEIGHT_INFINITY)
		return false;
	// mark the candidate as visited
	GetTile(candidate).visited = true;
	DijkstraNodesVisited++; // maintain a metric
	Tile &t = GetTile(candidate);

	// get it's weight + 1 - which would be the weight of the next square
	int weight = GetTile(candidate).weight + 1;

	// look at all the directions from this tile and if they're valid and unvisited check if the current path is shorter than the lowest found so far
	IntVec2 newPos = candidate + IntVec2(0, -1); // go north
	if ((!IsSolid(newPos)) && (!IsVisited(newPos)))
	{
		// if the weight of this tile is greater than our weight+1 set it to our weight+1 and set the backtrack direction
		if (GetTile(newPos).weight > weight)
		{
			GetTile(newPos).weight = weight;
			GetTile(newPos).backTrack = candidate;
		}
	}

	newPos = candidate + IntVec2(1, 0); // go east
	if ((!IsSolid(newPos)) && (!IsVisited(newPos)))
	{
		// if the weight of this tile is greater than our weight+1 set it to our weight+1 and set the backtrack direction
		if (GetTile(newPos).weight > weight)
		{
			GetTile(newPos).weight = weight;
			GetTile(newPos).backTrack = candidate;
		}
	}

	newPos = candidate + IntVec2(0, 1); // go south
	if ((!IsSolid(newPos)) && (!IsVisited(newPos)))
	{
		// if the weight of this tile is greater than our weight+1 set it to our weight+1 and set the backtrack direction
		if (GetTile(newPos).weight > weight)
		{
			GetTile(newPos).weight = weight;
			GetTile(newPos).backTrack = candidate;
		}
	}

	newPos = candidate + IntVec2(-1, 0); // go west
	if ((!IsSolid(newPos)) && (!IsVisited(newPos)))
	{
		// if the weight of this tile is greater than our weight+1 set it to our weight+1 and set the backtrack direction
		if (GetTile(newPos).weight > weight)
		{
			GetTile(newPos).weight = weight;
			GetTile(newPos).backTrack = candidate;
		}
	}

	return true;
}

bool Graph::IsDijkstraFinished(vector<IntVec2> &ret)
{
	ret.clear();

	// if the searchEnd tile is marked as visited we're done and can retrieve the path
	if (!IsVisited(mSearchEnd))
		return false;

	// the shortest path has been found
	// clear the visited tiles so we can re-mark just the path - marking the path is cosmetic
	ResetVisited();
	GetTile(mSearchEnd).visited = true; // remember we finished
	// backtrack till we reach the start storing each point into the ret vector
	IntVec2 pos = mSearchEnd;
	ret.push_back(pos); // always insert the end point
	GetTile(pos).path = true;
	while (pos != mSearchStart)
	{
		pos = GetTile(pos).backTrack; // step back
		ret.push_back(pos); // add this tile
		GetTile(pos).path = true;
	}
	// the list we created is backwards, so we reverse it
	reverse(ret.begin(), ret.end());
	return true;
}

void Graph::ResetVisited()
{
	for (int i = 0; i < mW*mH; i++)
	{
		mMaze[i].visited = false;
	}
}

IntVec2 Graph::GetForwardDirection(Direction dir)
{
	switch (dir)
	{
	case NORTH:
		return IntVec2(0, -1);
	case EAST:
		return IntVec2(1, 0);
	case SOUTH:
		return IntVec2(0, 1);
	case WEST:
		return IntVec2(-1, 0);
	}
	return IntVec2(0, 0);
}


void Graph::RunDijkstra(IntVec2 p1, IntVec2 p2, vector<IntVec2> &ret)
{
	InitDijkstra(p1, p2);
	while (!IsDijkstraFinished(ret))
		StepDijkstra();
}
