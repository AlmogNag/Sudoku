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


void main() {

    srand(12345); 

    int i;
    int x; // numPlayers - מספר שחקני סודוקו
    char name[MAX_LEN_NAME];
    short board[BOARD_SIZE][BOARD_SIZE];
    PLNODE* newNode; // משמש כנוד חדש

    PLIST activePlayers; // רשימה של שחקנים שעדין במשחק
    PLIST winners; // רשימה של מנצחים
    TREE* playerTree = NULL; // עץ של שחקנים
    PLNODE** playerArray; // מערך של מצביעים לשחקנים שעדין במשחק
    FILE* file = fopen("winner_file", "wt"); // winner file
    if (file == NULL) {
        printf("File does not exist\n");
        exit(1);
    }


    // Initialize lists
    makeEmptyList(&activePlayers);
    makeEmptyList(&winners);

    // Input number of players
    printf("enter a number ");
    scanf("%d", &x);

    // Allocate array for players
    playerArray = (PLNODE**)malloc(sizeof(PLNODE*) * x);
    checkAllocation(playerArray);


    // Input player names and create boards
    for (int i = 0; i < x; i++) {

        printf("enter %d name ", i);
        scanf("%s", name);
        createRandomBoard(board);
        newNode = createNewListNode(name, board, NULL);
        insertNodeToEndList(&activePlayers, newNode);
    }

    // Populate playerArray with sorted nodes
    PLNODE* curr = activePlayers.head;
    for (i = 0; i < x; i++) {
        playerArray[i] = curr;
        curr = curr->next;
    }



    // Sort playerArray by number of filled cells and name
    bubbleSort(playerArray, x);

    // Build the tree
    int n = (int)pow(2, ceil(log2(x + 1))) - 1;     // 𝑛 = 2^⌈log2(𝑥 + 1)⌉ − 1
    playerArray = (PLNODE**)realloc(playerArray, sizeof(PLNODE*) * n); // increase the playerArray to size n
    checkAllocation(playerArray);

    for (int i = x; i < n; i++) { // null to the new cells

        playerArray[i] = NULL;

    }
    

    buildTreeFromArray(&playerTree, playerArray, 0, x - 1);

    
    // Process players
    while (isEmptyList(activePlayers) == false) { // while list isnt empty cheak in tree - game loop

        

        inOrderTraversal(playerTree, &activePlayers, &winners);
        
    }

    curr = winners.head;

    while (curr != NULL) {

        fprintf(file, "%s\n", curr->name);
        printsudokuBoardToFile(curr, file); // הלוח של השחקן  
        curr = curr->next;
    }

    fclose(file);

    // Free allocated memory
    free(playerArray);
    freeList(&activePlayers); 
    freeList(&winners); 
    freeTree(playerTree); 
}


//סעיף א/
Array*** PossibleDigits(short sudokuBoard[][BOARD_SIZE]) {

    Array*** res;
    short i, j;

    createPossibleDigits(&res);

    for (i = 0; i < BOARD_SIZE; i++) {

        for (j = 0; j < BOARD_SIZE; j++) {

            if ((sudokuBoard[i][j] >= 1) && (sudokuBoard[i][j] <= 9)) { // there is number in cell
                free(res[i][j]);
                res[i][j] = NULL;

            }
            else {

                res[i][j]->arr = cellPossibleDigits(sudokuBoard, i, j, &res[i][j]->size);
            }
        }
    }

    return res;
}

void createPossibleDigits(Array**** res) {

    int i, j;

    *res = (Array***)malloc(BOARD_SIZE * sizeof(Array**));
    checkAllocation(*res);

    for (i = 0; i < BOARD_SIZE; i++) {
        (*res)[i] = (Array**)malloc(BOARD_SIZE * sizeof(Array*)); // Allocate memory for res[i]
        checkAllocation(*res);

        for (j = 0; j < BOARD_SIZE; j++) {
            (*res)[i][j] = (Array*)malloc(sizeof(Array)); // Allocate memory for res[i][j]
            checkAllocation(*res);

        }
    }
 
}

short* cellPossibleDigits(short sudokuBoard[][BOARD_SIZE], short row, short col, unsigned short* size) {
    int i, j;
    short arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 }; // Initialize with all possible digits
    short* res;
    short start_row = row / 3 * 3;
    short end_row = start_row + 3;
    short start_col = col / 3 * 3;
    short end_col = start_col + 3;
    *size = 9; // Start with all 9 possible digits

    // Check the row and eliminate digits
    for (i = 0; i < BOARD_SIZE; i++) {
        if (sudokuBoard[row][i] != -1 && arr[sudokuBoard[row][i] - 1] != -1) {
            arr[sudokuBoard[row][i] - 1] = -1;
            (*size)--; // Only decrement once per unique number
        }
    }

    // Check the column and eliminate digits
    for (i = 0; i < BOARD_SIZE; i++) {
        if (sudokuBoard[i][col] != -1 && arr[sudokuBoard[i][col] - 1] != -1) {
            arr[sudokuBoard[i][col] - 1] = -1;
            (*size)--; // Only decrement once per unique number
        }
    }

    // Check the 3x3 box and eliminate digits
    for (i = start_row; i < end_row; i++) {
        for (j = start_col; j < end_col; j++) {
            if (sudokuBoard[i][j] != -1 && arr[sudokuBoard[i][j] - 1] != -1) {
                arr[sudokuBoard[i][j] - 1] = -1;
                (*size)--; // Only decrement once per unique number
            }
        }
    }

    // Allocate memory for the result array based on the final size
    res = (short*)malloc(sizeof(short) * (*size));
    checkAllocation(res); // Check allocation

    // Copy valid digits from arr[] to res[]
    j = 0;
    for (i = 0; i < 9; i++) {
        if (arr[i] != -1) {
            res[j++] = arr[i];
        }
    }

    return res;
}



//סעיף ב//
int OneStage(short board[][9], Array*** possibilities, int* x, int* y) {

    int i, j;
    short digit;
    bool first = true; 
    bool changesMade = false;
    bool cellUpdated = false;
    int minOptions = BOARD_SIZE + 1;
    int minX = -1, minY = -1;

    // Process cells with exactly one possible digit
    while (changesMade == true || first == true) {
        
        changesMade = false;

        for (i = 0; i < BOARD_SIZE; i++) {
            
            if (first == true) {
                first = false; 
                
            }
            for (j = 0; j < BOARD_SIZE; j++) {
                

                if (board[i][j] == -1) { // Only process empty cells

                     // Now we access cellArray only after it's initialized:
                    if ((possibilities)[i][j]->size == 1) { // Single possible digit
                        digit = (possibilities)[i][j]->arr[0];
                        board[i][j] = digit;
                        updatePossibleDigits(possibilities, board, i, j, digit);
                        cellUpdated = true;
                        changesMade = true;
                    }
                }
            }
        }
    }

    if (changesMade == false) {
        // No single-digit cells were updated, find cell with minimum options
        for (i = 0; i < BOARD_SIZE; i++) {

            for (j = 0; j < BOARD_SIZE; j++) {

                if (board[i][j] == -1) {
                    if ((possibilities)[i][j]->size == 0) {
                        return FINISH_FAILURE;
                    }
                    if ((possibilities)[i][j]->size > 1 && (possibilities)[i][j]->size < minOptions) {
                        minOptions = (possibilities)[i][j]->size;
                        minX = i;
                        minY = j;
                    }
                }
            }
        }

        if (minX != -1 && minY != -1) { // update minimu options cell cordinate
            *x = minX;
            *y = minY;
            sudokoPrintBoard(board);
            return NOT_FINISH;
        }
    }
    

    // Check if the board is fully and correctly filled
    if (isValidBoard(board) == true) {
        return SUCCESS_FINISH;
    }
    else {
        return FINISH_FAILURE;
    }
}

void updatePossibleDigits(Array*** possibilities, short board[][BOARD_SIZE], short row, short col, short digit) {


    int startRow = (row / 3) * 3;
    int startCol = (col / 3) * 3;

    for (int i = 0; i < BOARD_SIZE; i++) {

        // Remove digit from row possibilities
        if ((possibilities)[row][i] != NULL && (possibilities)[row][i]->arr != NULL) {
            Array* arr = (possibilities)[row][i];
            for (int k = 0; k < arr->size; k++) {
                if (arr->arr[k] == digit) {
                    for (int l = k; l < arr->size - 1; l++) {
                        arr->arr[l] = arr->arr[l + 1];
                    }
                    (arr->size)--;
                    break;
                }
            }
        }
        // Remove digit from col possibilities       
        if ((possibilities)[i][col] != NULL && (possibilities)[i][col]->arr != NULL) {
            Array* arr = (possibilities)[i][col];
            for (int k = 0; k < arr->size; k++) {
                if (arr->arr[k] == digit) {
                    for (int l = k; l < arr->size - 1; l++) {
                        arr->arr[l] = arr->arr[l + 1];
                    }
                    (arr->size)--;
                    break;
                }
            }
        }
    }
    // Remove digit from 3X3 possibilities
    for (int i = startRow; i < startRow + 3; i++) {
        for (int j = startCol; j < startCol + 3; j++) {
            if ((possibilities)[i][j] != NULL && (possibilities)[i][j]->arr != NULL) {
                Array* arr = (possibilities)[i][j];
                for (int k = 0; k < arr->size; k++) {
                    if (arr->arr[k] == digit) {
                        for (int l = k; l < arr->size - 1; l++) {
                            arr->arr[l] = arr->arr[l + 1];
                        }
                        (arr->size)--;
                        break;
                    }
                }
            }
        }
    }



    // Clean up the memory for the updated cell
    free((possibilities)[row][col]->arr);
    free((possibilities)[row][col]);
    (possibilities)[row][col] = NULL;
}



// סעיף ג //
int FillBoard(short board[][BOARD_SIZE], Array*** possibilities) {
    int i;
    Position cell;
    int result;

    while (true) {
        result = OneStage(board, possibilities, &cell.row, &cell.col);

        if (result == SUCCESS_FINISH) {
            return FILLED;
        }
        else if (result == FINISH_FAILURE) {
            return FAIL;
        }
        else if (result == NOT_FINISH) {
            if ((possibilities)[cell.row][cell.col]->size <= 1) {
                printf("Error: Cell (%d, %d) does not have multiple options for user input.\n", cell.row, cell.col);
                return FAIL;
            }

            printf("\nCell number [%d,%d] currently holds the minimum number of possible values, select one of the below:\n", cell.row, cell.col);
            for (i = 0; i < (possibilities)[cell.row][cell.col]->size; i++) {
                printf("%d. %d\n", i + 1, (possibilities)[cell.row][cell.col]->arr[i]);
            }

            int userChoice;
            scanf("%d", &userChoice);
            printf("Updating cell %d with value %d\n", ((cell.row * BOARD_SIZE) + cell.col), userChoice);

            bool validChoice = false;
            for (i = 0; i < (possibilities)[cell.row][cell.col]->size; i++) {
                if ((possibilities)[cell.row][cell.col]->arr[i] == userChoice) {
                    validChoice = true;
                    break;
                }
            }
            if (!validChoice) {
                printf("Invalid choice.\n");
                return FAIL;
            }

            board[cell.row][cell.col] = userChoice;
            updatePossibleDigits(possibilities, board, cell.row, cell.col, userChoice);

            // Check for duplications immediately after updating
            if (!isValidBoard(board)) {
                printf("User's selections led to duplications\n");
                return FAIL;
            }
            else {
                if (!checkForEmptyCells(possibilities, board)) {
                    printf("No valid moves left after updating cell [%d,%d]\n", cell.row, cell.col);
                    return FAIL;
                }
            }
            // If valid, update the possible digits for the rest of the board
        }
        else {
            printf("Unexpected result from OneStage: %d\n", result);
            return FAIL;
        }
    }
}

bool checkForEmptyCells(Array*** possibilities, short board[][BOARD_SIZE]) {

    int i, j;

    for (i = 0; i < BOARD_SIZE; i++) {
        for (j = 0; j < BOARD_SIZE; j++) {
            // אם התא ריק
            if (board[i][j] == -1 && possibilities[i][j]->size == 0) {
                return false;
            }
        }
    }
    return true;
}

bool isValidBoard(short board[][BOARD_SIZE]) {
    int i, j, k, l;
    int startRow, startCol, value;

    for (i = 0; i < BOARD_SIZE; i++) {

        bool row[BOARD_SIZE + 1] = { false };
        bool col[BOARD_SIZE + 1] = { false };

        // Check rows and columns
        for (j = 0; j < BOARD_SIZE; j++) {
            // Check for duplicate in row
            if (board[i][j] != -1) {
                if (row[board[i][j]] == true) {
                    return false; // Duplicate found in row
                }
                row[board[i][j]] = true;
            }

            // Check for duplicate in column
            if (board[j][i] != -1) {
                if (col[board[j][i]] == true) {
                    return false; // Duplicate found in column
                }
                col[board[j][i]] = true;
            }
        }

        // Check 3x3 boxes
        bool box[BOARD_SIZE + 1] = { false };
        startRow = (i / 3) * 3;
        startCol = (i % 3) * 3;

        for (k = 0; k < 3; k++) {
            for (l = 0; l < 3; l++) {
                value = board[startRow + k][startCol + l];
                if (value != -1) {
                    if (box[value] == true) {
                        return false; // Duplicate found in box
                    }
                    box[value] = true;
                }
            }
        }
    }

    return true; // No duplicates found
}



// exstra //

void checkAllocation(void* ptr)
{
    if (ptr == NULL)
    {
        printf("Allocation error\n");
        exit(-1);
    }
}

void makeEmptyList(PLIST* lst)
{
    lst->head = lst->tail = NULL;
}

void makeEmptyCellList(CLIST* lst)
{
    lst->head = lst->tail = NULL;
}

int isEmptyList(PLIST lst)
{
    return (lst.head == NULL);
}

int isEmptyCellList(CLIST lst)
{
    return (lst.head == NULL);
}

PLNODE* createNewListNode(char* data, short sudokuBoard[][BOARD_SIZE], PLNODE* next)
{
    PLNODE* res;
    int i, j;
    res = (PLNODE*)malloc(sizeof(PLNODE));
    checkAllocation(res);

    strcpy(res->name, data);

    res->possibilities = NULL;
    res->next = next;
    for (i = 0; i < BOARD_SIZE; i++) {

        for (j = 0; j < BOARD_SIZE; j++) {
            res->sudokuBoard[i][j] = sudokuBoard[i][j];
        }
    }
    return res;
}

CNODE* createNewCellNode(int data, CNODE* next)
{
    CNODE* res;
    res = (CNODE*)malloc(sizeof(CNODE));
    res->cell = data;
    res->next = next;
    return res;
}

void insertNodeToEndList(PLIST* lst, PLNODE* tail)
{
    if (isEmptyList(*lst)) {
        lst->head = lst->tail = tail;
    }
    else {
        lst->tail->next = tail;
        lst->tail = tail;
    }
    tail->next = NULL;
}

void insertCellNodeToEndList(CLIST* lst, CNODE* tail)
{
    if (isEmptyCellList(*lst)) {
        lst->head = lst->tail = tail;
    }
    else {
        lst->tail->next = tail;
        lst->tail = tail;
    }
    tail->next = NULL;
}

void freeList(PLIST* lst)
{
    PLNODE* curr = lst->head;
    PLNODE* next;

    while (curr != NULL)
    {
        next = curr->next;
        free(curr);
        curr = next;
    }
}

void freeCellList(CLIST* lst) {

    CNODE* curr = lst->head;
    CNODE* next;

    while (curr != NULL)
    {
        next = curr->next;
        free(curr);
        curr = next;
    }
}

int countFilledCells(short board[BOARD_SIZE][BOARD_SIZE]) { // count number of filled cells

    int count = 0;
    int i, j;

    for (i = 0; i < BOARD_SIZE; i++) {

        for (j = 0; j < BOARD_SIZE; j++) {

            if (board[i][j] != -1) {
                count++;
            }
        }
    }
    return count;
}

void bubbleSort(PLNODE** array, int size) { // sort playerArray from min to max by size else by name

    int i, j;
    PLNODE* temp;
    int filledCells1;
    int filledCells2;

    for (i = 0; i < size - 1; i++) {

        for (j = 0; j < size - i - 1; j++) {

            // Compare number of filled cells first
            filledCells1 = countFilledCells(array[j]->sudokuBoard);
            filledCells2 = countFilledCells(array[j + 1]->sudokuBoard);


            if (filledCells1 == filledCells2) {
                // If the number of filled cells is the same, compare names 
                if (strcmp(array[j]->name, array[j + 1]->name) > 0) {
                    temp = array[j];
                    array[j] = array[j + 1];
                    array[j + 1] = temp;
                }


                // If the number of filled cells is different
                else if (filledCells1 > filledCells2) {
                    temp = array[j];
                    array[j] = array[j + 1];
                    array[j + 1] = temp;
                }
            }
        }
    }
}

void createRandomBoard(short board[BOARD_SIZE][BOARD_SIZE]) {

    int i, j;
    int k;      // מספר בין 1 לגודל הרשימה 
    int index;
    int value; // value on cell
    int N;    //  מספר המשבצות המלאות בתחילת המשחק 
    int row;
    int col;
    int size = BOARD_SIZE * BOARD_SIZE;

    CLIST cellList;
    CNODE* curr;
    CNODE* prev;

    makeEmptyCellList(&cellList);

    // Initialize the list with all cell indices (1 to 81)
    for (i = 1; i <= BOARD_SIZE * BOARD_SIZE; i++) {
        curr = createNewCellNode(i, NULL);
        checkAllocation(curr);
        insertCellNodeToEndList(&cellList, curr);
    }

    // random values to vary between program runs
    srand((unsigned int)time(NULL));

    // Determine the number of initially filled cells
    N = 1 + rand() % 20;

    // Initialize the board with -1
    for (i = 0; i < BOARD_SIZE; i++) {
        for (j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = -1;
        }
    }

    // Randomly select N cells and assign values
    for (i = 0; i < N; i++) {
        // Pick a random cell from the list
        k = 1 + rand() % size;

        curr = cellList.head;
        prev = NULL;

        // Traverse the list to find the k-th cell
        for (index = 1; index < k; index++) {
            prev = curr;
            curr = curr->next;
        }

        // Get row and column from cell index
        row = (curr->cell - 1) / BOARD_SIZE;
        col = (curr->cell - 1) % BOARD_SIZE;

        // Generate a random valid value for this cell
        value = 1 + rand() % BOARD_SIZE;

        while (isValidValue(board, row, col, value) == false) {    // while there is the same value in row/ column/ box

            value = 1 + rand() % BOARD_SIZE;
        }

        // Assign the value to the board and remove the cell from the list
        board[row][col] = value;
        if (prev == NULL) {
            cellList.head = curr->next;
        }
        else {
            prev->next = curr->next;
        }
        if (curr == cellList.tail) {
            cellList.tail = prev;
        }
        free(curr);
        size -= 1;
    }

}

bool isValidValue(short board[BOARD_SIZE][BOARD_SIZE], int row, int col, int value) {

    int i, j;
    int startRow, startCol;

    // Check if value is already in the same row, column, or 3x3 sub-grid
    for (i = 0; i < BOARD_SIZE; i++) {
        if (board[row][i] == value || board[i][col] == value) {
            return false;
        }
    }

    // Check if value is already in the 3x3 sub-grid
    startRow = (row / 3) * 3;
    startCol = (col / 3) * 3;

    for (i = startRow; i < startRow + 3; i++) {

        for (j = startCol; j < startCol + 3; j++) {

            if (board[i][j] == value) {
                return false;
            }
        }
    }

    return true;
}

void buildTreeFromArray(TREE** playerTree, PLNODE** playerArray, int start, int end)
{

    if (start > end) {
        *playerTree = NULL;
        return;
    }

    // Allocate memory for the tree
    *playerTree = (TREE*)malloc(sizeof(TREE));
    checkAllocation(*playerTree);

    // Build the tree
    (*playerTree)->root = BuildTreeFromArrayHelper(playerArray, start, end);
}

TreeNode* BuildTreeFromArrayHelper(PLNODE** arr, int start, int end)
{
    if (start > end) {
        return NULL;
    }

    int mid = (start + end) / 2;

    // Create the root node
    TreeNode* root = makeNewTreeNode(arr[mid], NULL, NULL);

    // Recursively build the left and right subtrees
    root->left = BuildTreeFromArrayHelper(arr, start, mid - 1);
    root->right = BuildTreeFromArrayHelper(arr, mid + 1, end);

    return root;
}

TreeNode* makeNewTreeNode(PLNODE* data, TreeNode* left, TreeNode* right)
{
    TreeNode* res;

    res = (TreeNode*)malloc(sizeof(TreeNode));
    checkAllocation(res);

    res->pData = data;
    res->left = left;
    res->right = right;

    return res;
}

void inOrderTraversal(TREE* tree, PLIST* activePlayers, PLIST* winners) { // Function to perform in-order traversal of the tree

    if (tree == NULL)
        return;

    inOrderTraversalHelper(tree->root, activePlayers, winners);
}

void inOrderTraversalHelper(TreeNode* root, PLIST* activePlayers, PLIST* winners) {

    // Helper function for in-order traversal
    if (root == NULL)
        return;

    inOrderTraversalHelper(root->left, activePlayers, winners);

    processPlayer(root, activePlayers, winners);

    inOrderTraversalHelper(root->right, activePlayers, winners);
}

void removeFromLists(PLNODE* player, PLIST* activePlayers, bool shouldFree) {

    PLNODE* prev = NULL;
    PLNODE* curr = activePlayers->head;

    while (curr != NULL) {
        if (curr == player) {
            if (prev == NULL) {
                activePlayers->head = curr->next;
            }
            else {
                prev->next = curr->next;
            }
            if (curr == activePlayers->tail) {
                activePlayers->tail = prev;
            }
            if (shouldFree) {
                // Free allocated memory
                free(curr->possibilities);
                free(curr);
            }
            break;
        }
        prev = curr;
        curr = curr->next;
    }
}

void processPlayer(TreeNode* root, PLIST* activePlayers, PLIST* winners) {
    
    int i;
    Position pos;
    int result;
    int numOfOptions;
    int input;

    if (root == NULL || root->pData == NULL)
        return;
    
    root->pData->possibilities = PossibleDigits(root->pData->sudokuBoard);
    result = OneStage(root->pData->sudokuBoard, root->pData->possibilities, &pos.row, &pos.col);

    if (result == FINISH_FAILURE) {
        // Remove player from activePlayers and set NULL in the tree
        removeFromLists(root->pData, activePlayers, true);
        root->pData = NULL;
    }
    else if (result == SUCCESS_FINISH) {
        // Move player to winners and remove from activePlayers
        insertNodeToEndList(winners, root->pData);
        removeFromLists(root->pData, activePlayers, false);
        root->pData = NULL;
    }
    else { // NOT_FINISH
        while (result == NOT_FINISH) {
            numOfOptions = root->pData->possibilities[pos.row][pos.col]->size;

            // Display options to the player
            printf("Choose a value for cell [%d, %d]:\n", pos.row, pos.col);
            for (i = 0; i < numOfOptions; i++) {
                printf("%d. %d\n", i + 1, root->pData->possibilities[pos.row][pos.col]->arr[i]);
            }


            scanf("%d", &input);

            // Validate input
            if (!isExists(input, root->pData->possibilities[pos.row][pos.col]->arr, numOfOptions)) {
                // Invalid move, remove player from activePlayers
                removeFromLists(root->pData, activePlayers, true);
                root->pData = NULL;
                return;
            }

            // Update the player's board and possibilities
            root->pData->sudokuBoard[pos.row][pos.col] = input;
            updatePossibleDigits(root->pData->possibilities, root->pData->sudokuBoard, pos.row, pos.col, input);
            result = FillBoard(root->pData->sudokuBoard, root->pData->possibilities);

            // Check the board's new status

            if (result == FAIL) {
                // Remove player from activePlayers and set NULL in the tree
                removeFromLists(root->pData, activePlayers, true);
                root->pData = NULL;
            }
            else if (result == FILLED) {
                // Move player to winners and remove from activePlayers
                insertNodeToEndList(winners, root->pData);
                removeFromLists(root->pData, activePlayers, false);
                root->pData = NULL;
            }
        }
    }
}

bool isExists(int num, short arr[], int size) {

    int i;
    bool ok = false;

    for (i = 0; (i < size) && (ok == false); i++) {

        if (arr[i] == num) {
            ok = true;
        }
    }
    return ok;
}

void printsudokuBoardToFile(PLNODE* curr, FILE* file) {


    int i, j;

    fprintf(file, "  | 0 1 2| 3 4 5| 6 7 8\n");
    fprintf(file, "-----------------------\n");

    for (i = 0; i < BOARD_SIZE; i++) {
        // Print column separators
        fprintf(file, " %d|", i);
        if (i % 3 == 0 && i != 0) {
            fprintf(file, "-----------------------\n");
        }

        for (j = 0; j < BOARD_SIZE; j++) {
            // Print row separators
            if (j % 3 == 0 && j != 0) {
                fprintf(file, "| ");
            }

            // Print the value in the current cell
            if (curr->sudokuBoard[i][j] == 0) {
                fprintf(file, "  "); // Print empty space for zeros
            }
            else {
                fprintf(file, "%d ", curr->sudokuBoard[i][j]); // Print the number
            }
        }
        fprintf(file, "\n");
    }
}

void sudokoPrintBoard(short sudokuBoard[][BOARD_SIZE]) {

    int i,j; 
    
    printf("  | 0 1 2| 3 4 5| 6 7 8\n");
    printf("-----------------------\n");

    for (i = 0; i < BOARD_SIZE; i++) {
        printf(" %d|", i);
        for (j = 0; j < BOARD_SIZE; j++) {
            if (sudokuBoard[i][j] == -1)
                printf("  ");
            else
                printf("%2d", sudokuBoard[i][j]);

            if ((j + 1) % 3 == 0 && j != BOARD_SIZE - 1)
                printf("|");
        }
        printf("\n");
        if ((i + 1) % 3 == 0 && i != BOARD_SIZE - 1)
            printf("-----------------------\n");
    }
    printf("-----------------------\n");
}

bool boardIsFull(short sudokuBoard[][BOARD_SIZE]) {
    int i, j; 
    for (i = 0; i < BOARD_SIZE; i++) {
        for (j = 0; j < BOARD_SIZE; j++) {
            if (sudokuBoard[i][j] == -1) {
                return false; 
            }
        }
    }
    return true; 
}

void freeTree(TREE* tr)
{
    freeTreeRec(tr->root);
}

void freeTreeRec(TreeNode* root)
{
    if (root == NULL)
        return;
    else
    {
        freeTreeRec(root->left);
        freeTreeRec(root->right);
        free(root->pData); 
        free(root);
    }
}
