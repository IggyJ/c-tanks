#ifndef QUADTREE_H
#define QUADTREE_H


#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define NW 0
#define NE 1
#define SW 2
#define SE 3

#define TREE_SIZE 512
#define MAX_DEPTH 9
#define MIN_SIZE  1   // = TREE_SIZE/(2^MAX_DEPTH) 


typedef struct Node {
    short hasChildren;
    short contents;
    short depth;
    short childOf;
    struct Node* parent;
    struct Node* child[4]; // NW, NE, SW, SE
} Node;


void createNode(Node** parent, int quadrant, int contents);
void printTreeInfo(Node* root);
int destroyLeaves(Node* root);
void destroyTree(Node* root);
int checkIfEqual(int array[], int n);
void optimiseTree(Node* root);
void findNode(Node* root, Node** target, int x, int y, int xRef, int yRef);
void createSquare(Node* root, int x, int y, int size, int contents);
void createCircle(Node* root, int x, int y, int size, int contents);
void generateTerrain(Node* root, int x, int y, int r[]);


#endif