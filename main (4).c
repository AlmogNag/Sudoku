#include "header.h"



/*void main()

{

       short board[9][9] =

       { 5,-1, 4,-1, 7,-1,-1, 1,-1,

        6,-1, 2, 1,-1,-1, 3,-1,-1,

        1,-1, 8,-1, 4,-1,-1, 6,-1,

       -1, 5,-1,-1, 6,-1,-1, 2,-1,

       -1, 2,-1, 8,-1, 3,-1,-1,-1,

       -1,-1,-1,-1,-1, 4,-1, 5, 6,

       -1, 6, 1, 5, 3, 7, 2, 8, 4,

       -1, 8, 7,-1, 1, 9,-1, 3,-1,

       -1,-1,-1, 2, 8,-1,-1,-1, 9 };



       Array*** possible_solutions;



       printf("Initial board\n");

       sudokoPrintBoard(board);



       printf("Press enter to start playing...\n");

       getchar();

       possible_solutions = PossibleDigits(board);



       if (FillBoard(board, possible_solutions) == -1)

             printf("User's selections led to duplications\n");

       else

       {

             sudokoPrintBoard(board);

             printf("Board filled successfully\n");

       }



       // Don't forget to free memory!

}*/


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
    int n = (int)pow(2, ceil(log2(x + 1))) - 1;     // ?? = 2^?log2(?? + 1)? ? 1
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