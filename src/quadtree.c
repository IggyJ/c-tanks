#include "quadtree.h"


void createNode(Node** parent, int quadrant, int contents) {
    if ( (*parent)->child[quadrant] == NULL) {
        Node * leaf = malloc(sizeof(Node));
        (*parent)->child[quadrant] = leaf;
        if ( (*parent)->hasChildren == 0 ) {
            (*parent)->hasChildren = 1;
        }
        leaf->hasChildren = 0;
        leaf->contents = contents;
        leaf->depth = ((*parent)->depth) + 1;
        leaf->parent = *parent;
        leaf->childOf = quadrant;
        for (int q = 0; q <= 3; q++) {
            leaf->child[q] = NULL;
        }
        //printf("created node at %p with depth %d\n", leaf, leaf->depth);
    }
    return;
}

void printTreeInfo(Node* root) {
    Node * node = root;
    for (int i = 1; i <= (node->depth); i++) {
        printf(" - ");
    }
    if (node->depth == 0) {
        printf("r ");
    }
    printf("node at %p\n", node);
    if (node->hasChildren == 1) {
        for (int q = 0; q <= 3; q++) {
            if (node->child[q] != NULL) {
                printTreeInfo(node->child[q]);
            }
        }
    }
    return;
}

// returns 1 if attempted to destroy root, otherwise returns 0
int destroyLeaves(Node* root) {
    Node * node = root;
    if (node->hasChildren == 1) {
        for (int q = 0; q <= 3; q++) {
            if (node->child[q] != NULL) {
                if (node->child[q]->hasChildren == 0) {
                    free(node->child[q]);
                    node->child[q] = NULL;
                }
                else {
                    destroyLeaves(node->child[q]);
                }
            }
        }
        if (node->child[0] == NULL && node->child[1] == NULL && node->child[2] == NULL && node->child[3] == NULL) {
            node->hasChildren = 0;
        }
    }
    else {
        return 1;
    }
    return 0;
}

void destroyTree(Node* root) {
    int done = 0;
    while(!done) {
        done = destroyLeaves(root);
    }
    free(root);
    return;
}

// returns 0 if all elements are equal, otherwise returns 1
int checkIfEqual(int array[], int n) {
    int result = 0;
    for (int i = 0; i < n && result == 0; i++) {
        if (array[i] != array[0]) {
            result = 1;
        }
    } 
    return result;
}  

void optimiseTree(Node* root) {
    Node * node = root;
    if (node->hasChildren == 1) {
        int leaves = 0;
        for (int q = 0; q <= 3; q++) {
            if (node->child[q] != NULL) {
                if (node->child[q]->hasChildren == 0) {
                    leaves++;
                }
                else {
                    optimiseTree(node->child[q]);
                }
            }
        }
        if (leaves == 4) {
            int array[4] = {node->child[0]->contents, node->child[1]->contents, node->child[2]->contents, node->child[3]->contents};
            if (checkIfEqual(array, 4) == 0) {
                node->contents = node->child[0]->contents;
                for (int q = 0; q <= 3; q++) {
                    free(node->child[q]);
                    node->child[q] = NULL;
                }
                node->hasChildren = 0;
            }
        }
    }
    return;
}

void findNode(Node* root, Node** target, int x, int y, int xRef, int yRef) {
    Node * node = root;
    int size = TREE_SIZE/(pow(2, node->depth));

    if (root->hasChildren == 1) {
        if (x < (xRef + size/2) && y < (yRef + size/2)) {
            if (node->child[NW] != NULL) {
                findNode(node->child[NW], target, x, y, xRef, yRef);
            }
            else {
                (*target) = node;
            }
        }
        else if (x >= (xRef + size/2) && y < (yRef + size/2)) {
            if (node->child[NE] != NULL) {            
                findNode(node->child[NE], target, x, y, (xRef + size/2), yRef);
            }
            else {
                (*target) = node;
            }
        }
        else if (x < (xRef + size/2) && y >= (yRef + size/2)) {
            if (node->child[SW] != NULL) {            
                findNode(node->child[SW], target, x, y, xRef, (yRef + size/2));
            }
            else {
                (*target) = node;
            }
        }
        else if (x >= (xRef + size/2) && y >= (yRef + size/2)) {
            if (node->child[SE] != NULL) {            
                findNode(node->child[SE], target, x, y, (xRef + size/2), (yRef + size/2));
            }
            else {
                (*target) = node;
            }
        }
    }
    else {
        (*target) = node;
    }
}

void createSquare(Node* root, int x, int y, int size, int contents) {
    Node * node = NULL;
    int xStart = x;
    int yStart = y;

    for (x = xStart; x <= xStart + size - MIN_SIZE; x += MIN_SIZE) {
        for (y = yStart; y <= yStart + size - MIN_SIZE; y += MIN_SIZE) {
            findNode(root, &node, x, y, 0, 0);
            while (node->depth != MAX_DEPTH) {
                for (int q = 0; q <= 3; q++) {
                    createNode(&node, q, node->contents);
                }
                findNode(root, &node, x, y, 0, 0);
            }
            node->contents = contents;  
        } 
    }

    for (int i = 0; i <= MAX_DEPTH; i++) {
        optimiseTree(root);
    }

    return;
}

void createCircle(Node* root, int x, int y, int size, int contents) {
    Node * node = NULL;
    int xStart = x;
    int yStart = y;

    for (x = xStart; x <= xStart + size - MIN_SIZE; x += MIN_SIZE) {
        for (y = yStart; y <= yStart + size - MIN_SIZE; y += MIN_SIZE) {
            findNode(root, &node, x, y, 0, 0);
            while (node->depth != MAX_DEPTH) {
                for (int q = 0; q <= 3; q++) {
                    createNode(&node, q, node->contents);
                }
                findNode(root, &node, x, y, 0, 0);
            }
            if (pow((x - xStart - size/2 + MIN_SIZE), 2) + pow((y - yStart - size/2 + MIN_SIZE), 2) <= pow(size/2, 2)) {
                node->contents = contents;  
            }
        } 
    }

    for (int i = 0; i <= MAX_DEPTH; i++) {
        optimiseTree(root);
    }

    return;
}

void generateTerrain(Node* root, int x, int y, int r[]) {
    Node * node = root;
    int size = TREE_SIZE/(pow(2, node->depth));
    if (node->depth <= MAX_DEPTH) {
        for (int q = 0; q <= 3; q++) {
            if (node->depth == 1 && (q == SW || q == SE) && (node->childOf == SW || node->childOf == SE)) {
                createNode(&node, q, 1);
            }
            else if 
            (
            (node->depth == MAX_DEPTH) && 
            (node->parent->parent->parent->parent->parent->parent->parent->childOf == NW || node->parent->parent->parent->parent->parent->parent->parent->childOf == NE) && 
            (node->parent->parent->parent->parent->parent->parent->parent->parent->childOf == SW || node->parent->parent->parent->parent->parent->parent->parent->parent->childOf == SE) && 
            (( r[0]*sin((3*M_PI*x)/TREE_SIZE) + r[1]*sin((6*M_PI*x)/TREE_SIZE) + 320) < y)
            )
            {
                createNode(&node, q, 1);
            }
            else {
                createNode(&node, q, node->contents);
            }
            generateTerrain(node->child[q], x + ( (q == NE || q == SE) ? (size/2):0), y + ( (q == SW || q == SE) ? (size/2):0), r);
        }
    }
    return;
}