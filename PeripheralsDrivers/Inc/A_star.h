/*
 * A_star.h
 *
 *  Created on: Sep 28, 2024
 *      Author: matto
 */

#ifndef A_STAR_H_
#define A_STAR_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define SIDE_COST_FLOAT 1.0F * 10.0F
#define DIAGONAL_COST_FLOAT M_SQRT2 * 10.0F

#define SIDE_COST_INT 10
#define DIAGONAL_COST_INT 14

enum{
	eROW = 0,
	eCOL
};

enum {
	eStartNode = 0,
	eEndNode,
	eCommonNode
};

enum{
	eWalkable = 0,
	eNotWalkable
};
enum{
	eN= 0,
	eS,
	eW,
	eE,
	eNW,
	eNE,
	eSW,
	eSE

};
enum{
	eOPEN = 0,
	eCLOSE,
	eNONE
};

typedef struct {
	uint8_t direction; // direction to reach the node from the last node
	float gridDistance; // distance between grids(nodes)
}naviHandler_t;

typedef struct { // always use 10 as side cost and 14 as diagonal cost
	uint16_t node[2]; // Node coordinates in the matrix (x,y)
	uint16_t G_cost; // Distance from starting node
	uint16_t H_cost; // Distance from end node
	uint16_t F_cost; // G_cost+H_cost
	uint16_t parentNode[2]; // Node from current node was reached
	uint8_t start_endFlag; // To set a node as start/end node
	uint8_t walkableNode; // To set if this node is an obstacle or not
	uint8_t evaluatedNode; // To know if the node was already filled
	naviHandler_t naviHere; // To set how to reach this node from previous node
}nodeHandler_t;

typedef struct{
	nodeHandler_t** Chart; // 2D-array that contains the map
	uint16_t MapRows;
	uint16_t MapCols;
	uint16_t goalNode[2]; // To store the coordinates of goal point
	uint16_t startNode[2]; // To store the coordinates of start point
	float gridSize; // To store the edge size of the grid
}mapHandler_t;



uint8_t getMap(char* mapAsString, mapHandler_t* map, float gridSize); // To convert from map format to nodeHandler format
uint8_t getMapShape(char* mapAsString, uint16_t* outputSize);
uint16_t fillNodeNeighbors(mapHandler_t* map, nodeHandler_t* currentNode, nodeHandler_t* open_list[]); // To fill nodeHandler for surrounding nodes
uint8_t A_star(mapHandler_t* map, nodeHandler_t* currentNode); // To compute path
uint8_t SetStartPoint(mapHandler_t* map, float GridDistance);
uint8_t nodeArraySortByF_cost(nodeHandler_t* arr[], uint16_t arraySize);
uint8_t initializeList(void* list[], int size);
uint8_t getTrajectory(mapHandler_t* map, nodeHandler_t* navigationList[]);
#endif /* A_STAR_H_ */
