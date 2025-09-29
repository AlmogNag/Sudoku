#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#define BOARD_SIZE 9
#define MAX_LEN_NAME 100
#define NOT_FINISH 0        // not finished
#define SUCCESS_FINISH 1    // legal board
#define FINISH_FAILURE 2    // illgagel board
#define FAIL -1              // illgagel/not full board
#define FILLED 3            // llgagel full board

typedef struct _Array
{
    short* arr;
    unsigned short size;
} Array;


typedef struct _Plnode {

    char name[MAX_LEN_NAME];                       // name of player 
    short sudokuBoard[BOARD_SIZE][BOARD_SIZE];     // board of player
    Array*** possibilities;                       // option matrix 
    struct _Plnode* next;

} PLNODE;

typedef struct _Plist {
    PLNODE* head;
    PLNODE* tail;

}PLIST;

typedef struct _Cell // node of cell 
{
    int cell;
    struct _Cell* next;
} CNODE;

typedef struct _Clist {
    CNODE* head;
    CNODE* tail;

}CLIST;


typedef struct treeNode
{
    PLNODE* pData;
    struct treeNode* right;
    struct treeNode* left;
}TreeNode;

typedef struct tree
{
    TreeNode* root;
}TREE;


typedef struct {
    int row;
    int col;
} Position;


// functions declartions //

Array*** PossibleDigits(short sudokuBoard[][BOARD_SIZE]);
void createPossibleDigits(Array**** board);
short* cellPossibleDigits(short sudokuBoard[][BOARD_SIZE], short i, short j, unsigned short* size);
int OneStage(short board[][BOARD_SIZE], Array*** possibilities, int* x, int* y);
void updatePossibleDigits(Array*** possibilities, short board[][BOARD_SIZE], short row, short col, short digit);
int FillBoard(short board[][BOARD_SIZE], Array*** possibilities);
bool checkForEmptyCells(Array*** possibilities, short board[][BOARD_SIZE]);
bool isValidBoard(short board[][BOARD_SIZE]);
void createRandomBoard(short board[BOARD_SIZE][BOARD_SIZE]);
bool isValidValue(short board[BOARD_SIZE][BOARD_SIZE], int row, int col, int value);


void checkAllocation(void* ptr);
void makeEmptyList(PLIST* lst);
void makeEmptyCellList(CLIST* lst);
int isEmptyList(PLIST lst);
int isEmptyCellList(CLIST lst);
void freeList(PLIST* lst);
void freeCellList(CLIST* lst);
void freeTree(TREE* tr);
void freeTreeRec(TreeNode* root);
PLNODE* createNewListNode(char* data, short sudokuBoard[][BOARD_SIZE], PLNODE* next);
CNODE* createNewCellNode(int data, CNODE* next);
void insertNodeToEndList(PLIST* lst, PLNODE* tail);
void insertCellNodeToEndList(CLIST* lst, CNODE* tail);
int countFilledCells(short board[BOARD_SIZE][BOARD_SIZE]);
void bubbleSort(PLNODE** array, int size);
void buildTreeFromArray(TREE** tree, PLNODE** array, int start, int end);
TreeNode* BuildTreeFromArrayHelper(PLNODE** arr, int start, int end);
TreeNode* makeNewTreeNode(PLNODE* data, TreeNode* left, TreeNode* right);
void inOrderTraversal(TREE* tree, PLIST* activePlayers, PLIST* winners);
void inOrderTraversalHelper(TreeNode* root, PLIST* activePlayers, PLIST* winners);
void removeFromLists(PLNODE* player, PLIST* activePlayers, bool shouldFree);


void processPlayer(TreeNode* root, PLIST* activePlayers, PLIST* winners);
bool isExists(int num, short arr[], int size);
void printsudokuBoardToFile(PLNODE* curr, FILE* file);
void sudokoPrintBoard(short sudokuBoard[][BOARD_SIZE]);
bool boardIsFull(short sudokuBoard[][BOARD_SIZE]);
