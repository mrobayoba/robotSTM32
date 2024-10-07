/*
 * A_start.c
 *
 *  Created on: Sep 28, 2024
 *      Author: matto
 */
/*
 * The following functions can be use to structure and write a A-star algorithm!!!
 * THIS IS NOT THE A-STAR ALGORITHM!!!!
 *
 * The user must fill a map with O as walkable grid, # as obstacle grid, % as row end, S as start grid and G as goal grid
 */
#include "A_star.h"

/* * max macro from
 * https://stackoverflow.com/questions/3437404/min-and-max-in-c
 * */
#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

// Private prototypes
int16_t heuristic_octile(int x_current, int y_current, int x_goal, int y_goal);
uint8_t checkGrid(int x_coord, int y_coord, mapHandler_t* map);
uint8_t naviSetTrajectory(nodeHandler_t* node, uint16_t* parentCoordinates, float gridDistance);


/* Returns the pionter to the selected node*/
uint8_t getMapShape(char* mapAsString, uint16_t* outputSize){
	uint8_t done = 0;
	uint16_t row_index = 0;
	uint16_t col_index = 0;
	for (int i = 0; mapAsString[i] != '\0'; i++) {
		if((mapAsString[i] != '%') && !done){

			col_index++;
		}
		else if(mapAsString[i] == '%'){
			row_index++;
			done = 1;
		}
	}
	outputSize[0]= row_index+1; // To store total rows in the array
	outputSize[1]= col_index; // To store total columns in the array

	done = 1;
	return done;
}

uint8_t getMap(char* mapAsString, mapHandler_t* map, float gridSize){
	uint8_t done = 0;
	uint16_t row_index = 0;
	uint16_t col_index = 0;
	// To convert from map format to nodeHandler format
	// Here we have to fill node[2], start_endFlag, walkableNode and initialize evaluatedNode
	for (int i = 0; mapAsString[i] != '\0'; i++) {
		if(mapAsString[i] != '%'){
			if(mapAsString[i] == 'O'){ // when is walkable
				map->Chart[row_index][col_index].node[0] = row_index;
				map->Chart[row_index][col_index].node[1] = col_index;
				map->Chart[row_index][col_index].start_endFlag = eCommonNode;
				map->Chart[row_index][col_index].walkableNode = eWalkable;
				map->Chart[row_index][col_index].evaluatedNode = eNONE;

			}
			else if(mapAsString[i] == '#'){ // when is obstacle
				map->Chart[row_index][col_index].node[0] = row_index;
				map->Chart[row_index][col_index].node[1] = col_index;
				map->Chart[row_index][col_index].start_endFlag = eCommonNode;
				map->Chart[row_index][col_index].walkableNode = eNotWalkable;
				map->Chart[row_index][col_index].evaluatedNode = eNONE;
			}
			else if(mapAsString[i] == 'S'){ // when is start
				map->Chart[row_index][col_index].node[0] = row_index;
				map->Chart[row_index][col_index].node[1] = col_index;
				map->Chart[row_index][col_index].start_endFlag = eStartNode;
				map->Chart[row_index][col_index].walkableNode = eWalkable;
				map->startNode[0] = row_index;
				map->startNode[1] = col_index;
				map->Chart[row_index][col_index].evaluatedNode = eNONE;
			}
			else if(mapAsString[i] == 'G'){ // whes is goal
				map->Chart[row_index][col_index].node[0] = row_index;
				map->Chart[row_index][col_index].node[1] = col_index;
				map->Chart[row_index][col_index].start_endFlag = eEndNode;
				map->Chart[row_index][col_index].walkableNode = eWalkable;
				map->goalNode[0] = row_index;
				map->goalNode[1] = col_index;
				map->Chart[row_index][col_index].evaluatedNode = eNONE;

			}
			col_index++;
		}
		else if(mapAsString[i] == '%'){
			row_index++;
			col_index = 0;
		}
	}
	map->MapRows = row_index+1; // To store total rows in the array
	map->MapCols = col_index; // To store total columns in the array
	map->gridSize = gridSize;

	done = 1;
	return done;
}

uint16_t fillNodeNeighbors(mapHandler_t* map, nodeHandler_t* currentNode, nodeHandler_t* open_list[]){

	// To fill nodeHandler for surrounding nodes
	// Here we have to fill G_cost, H_cost, F_cost, parentNode[2]
	int x = currentNode->node[0];
	int y = currentNode->node[1];
	int x_goal = map->goalNode[0];
	int y_goal = map->goalNode[1];
	uint16_t listIndex = 0;

	for (int i = 0; open_list[i]!= NULL;i++){
		listIndex = i;
	}
//	listIndex++; // Next index to fill in open


	for (int i = -1; i <= 1; i++) {         // Rows: -1, 0, +1 relative to the center
		for (int j = -1; j <= 1; j++) {     // Cols: -1, 0, +1 relative to the center
			if (i == 0 && j == 0) {
				continue; // Skip the center itself
			}
			else if((i == j) || (i+j == 0)){ // Diagonals
				if(checkGrid(x+i,y+j , map)){ //Check for the coordinate to exist
					// Fill data in the node
					if (map->Chart[x+i][y+j].evaluatedNode == eNONE
							|| ((map->Chart[x+i][y+j].evaluatedNode == eOPEN)
									&&(map->Chart[x+i][y+j].F_cost > ((currentNode->G_cost
									+DIAGONAL_COST_INT)+heuristic_octile(x+i, y+j, x_goal, y_goal))))) {
						map->Chart[x+i][y+j].G_cost = currentNode->G_cost +DIAGONAL_COST_INT;
						map->Chart[x+i][y+j].H_cost = heuristic_octile(x+i, y+j, x_goal, y_goal);
						map->Chart[x+i][y+j].F_cost = map->Chart[x+i][y+j].G_cost + map->Chart[x+i][y+j].H_cost;
						map->Chart[x+i][y+j].parentNode[eROW]= currentNode->node[eROW];
						map->Chart[x+i][y+j].parentNode[eCOL]= currentNode->node[eCOL];
						map->Chart[x+i][y+j].evaluatedNode =eOPEN;
						// Add node pointer to the openList to be manage
						open_list[listIndex] = &map->Chart[x+i][y+j];
						listIndex++;
					}
				}
				else{
//					open_list[listIndex] = NULL; //save not available nodes as NULL
//					listIndex++;
				}
			}
			else{ // Verticals and Horizontals
				if(checkGrid(x+i,y+j , map)){ //Check for the coordinate to exist
					// Fill data in the node
					if (map->Chart[x+i][y+j].evaluatedNode == eNONE
							|| (map->Chart[x+i][y+j].F_cost > ((currentNode->G_cost
									+SIDE_COST_INT)+heuristic_octile(x+i, y+j, x_goal, y_goal)))) {
						map->Chart[x+i][y+j].G_cost = currentNode->G_cost +SIDE_COST_INT;
						map->Chart[x+i][y+j].H_cost = heuristic_octile(x+i, y+j, x_goal, y_goal);
						map->Chart[x+i][y+j].F_cost = map->Chart[x+i][y+j].G_cost + map->Chart[x+i][y+j].H_cost;
						map->Chart[x+i][y+j].parentNode[eROW]= currentNode->node[eROW];
						map->Chart[x+i][y+j].parentNode[eCOL]= currentNode->node[eCOL];
						map->Chart[x+i][y+j].evaluatedNode =eOPEN;
						// Add node pointer to the openList to be manage
						open_list[listIndex] = &map->Chart[x+i][y+j];
						listIndex++;
					}
				}
				else{
//					open_list[listIndex] = NULL; //save not available nodes as NULL
//					listIndex++;
				}
			}
		}
	}

	return listIndex; //returns the pointer to the next available index
}
uint8_t SetStartPoint(mapHandler_t* map, float GridDistance){
	uint8_t done = 0;

	map->Chart[map->startNode[0]][map->startNode[1]].G_cost = 0;
	map->Chart[map->startNode[0]][map->startNode[1]].H_cost = heuristic_octile((int)map->startNode[0],
																(int)map->startNode[1],
																(int)map->goalNode[0],
																(int)map->goalNode[1]);
	map->Chart[map->startNode[0]][map->startNode[1]].F_cost = map->Chart[map->startNode[0]][map->startNode[1]].G_cost +
															map->Chart[map->startNode[0]][map->startNode[1]].H_cost;

	done = 1;
	return done;
}


// Private functions
uint8_t checkGrid(int x_coord, int y_coord, mapHandler_t* map){
	return ((x_coord>=0) && (y_coord>=0))
			&& ((x_coord<map->MapRows) && (y_coord<map->MapCols))
			&& (!map->Chart[x_coord][y_coord].walkableNode)
			&& (map->Chart[x_coord][y_coord].evaluatedNode != eCLOSE)
			&& (map->Chart[x_coord][y_coord].start_endFlag != eStartNode);
}

int16_t heuristic_octile(int x_current, int y_current, int x_goal, int y_goal){
	// This function computes the H_cost for a 8-path model (verticals, horizontals, diagonals)
	int absX = abs(x_current-x_goal),absY = abs(y_current-y_goal);
	int aux = (14-10)*min(absX,absY) + 10*max(absX,absY);

	return (int16_t) aux;

}

uint8_t nodeArraySortByF_cost(nodeHandler_t* arr[], uint16_t arraySize) {

    for (uint16_t i = 0; i < arraySize - 1; i++) { //compare F_cost, if they are equal compare H_cost
        uint16_t min_idx = i;
        for (uint16_t j = i + 1; j < arraySize; j++) {
        	if(arr[min_idx] == NULL){ // if NULL is found put it at bottom
        		min_idx = j;
        	}
        	else if (arr[j]->F_cost < arr[min_idx]->F_cost) {
        		//if next val is lowest to current val, put next val at top
                min_idx = j;
            }
        	else if(arr[j]->F_cost == arr[min_idx]->F_cost){
        		//if next val equal to current val evaluate second criteria
        		if(arr[j]->H_cost < arr[min_idx]->H_cost){
        			// if next val is lowest to current val, put next val at top
        			min_idx = j;
        		}
        	}
        }
        // Swap the found minimum element with the first element
        nodeHandler_t* temp = arr[min_idx];
        arr[min_idx] = arr[i];
        arr[i] = temp;
    }

    return 1;
}
//This looks useful to iterate until NULL!
uint8_t initializeList(void* list[], int size){
	for(uint16_t i = 0; i < size; i++){
		list[i] = NULL; // To initialize openList
	}
	return 1;
}

uint8_t getTrajectory(mapHandler_t* map, nodeHandler_t* navigationList[]){
	//Take goalNode and track back the path by the parents
	nodeHandler_t* ptrCurrentNode = NULL;
	uint16_t navListIndex = 0;
	// Start tracking from goalNode
	ptrCurrentNode = &map->Chart[map->goalNode[eROW]][map->goalNode[eCOL]];
	while(1){
		//if parent of current node is equal to start node
		if((ptrCurrentNode->parentNode[0] == map->startNode[0])
				&&(ptrCurrentNode->parentNode[1] == map->startNode[1])){
			// config naviHere for thisNode and save it to navigationList and break loop
			naviSetTrajectory(ptrCurrentNode, ptrCurrentNode->parentNode,
					map->gridSize);
			navigationList[navListIndex] = ptrCurrentNode;
			break;
		}
		// calculate distance and direction from parent to current node
		// So next_grid.naviHere must be check when in current_grid in order to drive oppy to next_grid!!!
		naviSetTrajectory(ptrCurrentNode, ptrCurrentNode->parentNode,
				map->gridSize);
		// add ptrCurrentNode to navigation list
		navigationList[navListIndex] = ptrCurrentNode;
		navListIndex++;
		// after config naviHere in current node, set ptrCurrentNode to the parent node
		ptrCurrentNode = &map->Chart[ptrCurrentNode->parentNode[0]][ptrCurrentNode->parentNode[1]];
	}

	return 1;
}

uint8_t naviSetTrajectory(nodeHandler_t* node, uint16_t* parentCoordinates, float gridDistance){
	int x_current,y_current;
	int x_parent,y_parent;

	x_current = node->node[0];
	y_current = node->node[1];

	x_parent = parentCoordinates[0];
	y_parent = parentCoordinates[1];

	float aux;

	if ((x_parent == x_current-1) && (y_parent == y_current-1)){ // Top left grid
		aux = M_SQRT2*gridDistance;
		//add this val to distance in naviHere component
		node->naviHere.gridDistance = aux;
		node->naviHere.direction = eSE;

	}
	else if ((x_parent == x_current-1) && (y_parent == y_current+1)){ // Top right grid
		aux = M_SQRT2*gridDistance;
		//add this val to distance in naviHere component
		node->naviHere.gridDistance = aux;
		node->naviHere.direction = eSW;

	}
	else if ((x_parent == x_current+1) && (y_parent == y_current-1)){ // Bottom left grid
		aux = M_SQRT2*gridDistance;
		//add this val to distance in naviHere component
		node->naviHere.gridDistance = aux;
		node->naviHere.direction = eNE;

	}
	else if ((x_parent == x_current+1) && (y_parent == y_current+1)){ // Bottom right grid
		aux = M_SQRT2*gridDistance;
		//add this val to distance in naviHere component
		node->naviHere.gridDistance = aux;
		node->naviHere.direction = eNW;

	}
	else if ((x_parent == x_current-1) && (y_parent == y_current)){ // Top grid
		aux = gridDistance;
		//add this val to distance in naviHere component
		node->naviHere.gridDistance = aux;
		node->naviHere.direction = eS;

	}
	else if ((x_parent == x_current+1) && (y_parent == y_current)){ // Bottom grid
		aux = gridDistance;
		//add this val to distance in naviHere component
		node->naviHere.gridDistance = aux;
		node->naviHere.direction = eN;

	}
	else if ((x_parent == x_current) && (y_parent == y_current-1)){ // Left grid
		aux = gridDistance;
		//add this val to distance in naviHere component
		node->naviHere.gridDistance = aux;
		node->naviHere.direction = eE;

	}
	else if ((x_parent == x_current) && (y_parent == y_current+1)){ // Right
		aux = gridDistance;
		//add this val to distance in naviHere component
		node->naviHere.gridDistance = aux;
		node->naviHere.direction = eW;

	}

	else{
		return 0;
	}

	return 1;
}



