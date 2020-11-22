#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<math.h>
static int boardSize = 8;
char board[1024];
/**
 * this boils down to the following:
 * the board is an 8x8 2d array 
 * each player struct will be denoted by a boolean and their remaining pieces
 * each piece struct will be denoted by a character and its position on the board
 * the position on the board will be denoted by a character and an integer like how it commonly is presented in chess
 */

typedef struct piece {
    char name;  // the name of the piece
                // for player 1 the names will be one char in caps (i.e. "P" for pawn or "K" for king)
                // for player 2 the names will be one char in lowercase(i.e. "h" for horse or "r" for rook)
    // in chess, positions are denoted by a character (a - g) and a number (1 - 8)
    struct position {
        int row;
        char col;
    } position;
}piece;



// if bool player is true, the player will use the white pieces (moves first), other player will make their move after
// pieces will be represented by characters shown above
typedef struct player {
    bool player1;

    bool inCheck; // if inCheck is true, the player can only perform moves that save the king
    bool inCheckMate; // if either player is in checkmate the opposing player wins the game
    struct piece allPieces[16];
}player;

struct player p1;
struct player p2;

// converts an integer to char* so that we can concatenate to board later
char* itoa(int num) {
	
	static char str[10] = {0}; // we only use 1-8 buffer doesn't need to be larger
    
	int i;
	
	for(i = 10; num && i ; --i, num /= 10) {
        str[i] = "0123456789"[num % 10];
    }
	return &str[i+1];
	
}
// returns the row position 0-7 based on the board's rows 8-1
int rowPositionOnArray(int row) {
    return boardSize - row;
}

//retuns the column position  0-7 based on char 'a' - 'h'
int colPositionOnArray(char x) {
    int column = 0;
    char i = 'a';
    while (i != x) {
        i++;
        column++;
    }
    return column;
}

// generates an empty board and stores it in a global char array for easy access/modification
void generateBoard() {
    int space = 6;
    strcat(board, "      a     b     c     d     e     f     g     h\n");
    for (int i = 0; i <= boardSize; i++) {
        int row = rowPositionOnArray(i);
        strcat(board,"   ");
        for (int horizontal = 0; horizontal <= space*boardSize; horizontal++) {
            if (horizontal%space ==0) {
                strcat(board, "|");
            } else {
                strcat(board,"-");
            }
        }
        strcat(board,"\n ");
        if (i != boardSize) {
            strcat(board, itoa(row));
            for (int vertical = 0; vertical < boardSize; vertical++) {
                strcat(board, " |    ");
            }
            strcat(board," | ");
            strcat(board, itoa(row));
            strcat(board,"\n");
        }
    }
    strcat(board, "     a     b     c     d     e     f     g     h\n");
    //printf("%s\n", board);
}
// use math to find which index of board to go to 
// using array positions rather than the board positions (two ints are easier to deal with than a 2darray with char columns and backwards int rows)
int findPositionOnBoard(int arrayRow, int arrayCol) {
    // middle of first cell is at index 109; distance between each cell is 6; distance between each row (same column) is 108
    int firstRow = 109;
    int colDistance = 6;
    return firstRow + ((firstRow - 1)*arrayRow) + (colDistance*arrayCol);
}

 void instantiatePlayerPieces() {

    // initialize player properties
    p1.player1 = true;
    p2.player1 = false;

    p1.inCheck = false;
    p2.inCheck = false;
    p1.inCheckMate = false;
    p2.inCheckMate = false;
        
    //initialize the pawns

    //iniitialize player 1 pawn properties
    struct piece pawn;
    pawn.name = 'P';
    char x = 'a';
    pawn.position.row = 2;

    // add pawns to p1's remaining pieces
    for (int i = 0; i < boardSize; i++) {
        pawn.position.col = x++;
        p1.allPieces[i] = pawn;
    }

    //initialize player 2 pawn properties
    pawn.name = 'p';
    x = 'a';
    pawn.position.row = 7;

    // add pawns to p2's remaining pieces
    for (int i = 0; i < boardSize; i++) {
        pawn.position.col = x++;
        p2.allPieces[i] = pawn;
    }

    //initialize the two rooks for each player
    struct piece rook;
    //start with player1

    rook.name = 'R';
    rook.position.col = 'a';
    rook.position.row = 1;
    p1.allPieces[8] = rook;
    // reinstantiate for second rook
    rook.position.col = 'h';
    p1.allPieces[9] = rook;

    // add rooks to player 2 pieces

    rook.name = 'r';
    rook.position.col = 'a';
    rook.position.row = 8;
    p2.allPieces[8] = rook;
    // reinstantiate for second rook
    rook.position.col = 'h';
    p2.allPieces[9] = rook;


    //initialize horses/knights for each player
    struct piece horse;

    //start with player1
    horse.name = 'H'; // using H for horse instead of knight since King also starts with k
    horse.position.col = 'b';
    horse.position.row = 1;
    p1.allPieces[10] = horse;

    //reinstantiate for second knight
    horse.position.col = 'g';
    p1.allPieces[11] = horse;

    //add knights to player 2 pieces
    //start with player1
    horse.name = 'h'; // using H for horse instead of knight since King also starts with k
    horse.position.col = 'b';
    horse.position.row = 8;
    p2.allPieces[10] = horse;

    //reinstantiate for second knight
    horse.position.col = 'g';
    p2.allPieces[11] = horse;

    //initialize bishops for each player
    struct piece bishop;

    //start with player1
    bishop.name = 'B';
    bishop.position.col = 'c';
    bishop.position.row = 1;
    p1.allPieces[12] = bishop;

    //reinstatiate for second bishop
    bishop.position.col = 'f';
    p1.allPieces[13] = bishop;

    //add bishops to player2 pieces
    bishop.name = 'b';
    bishop.position.col = 'c';
    bishop.position.row = 8;
    p2.allPieces[12] = bishop;

    //reinstatiate for second bishop
    bishop.position.col = 'f';
    p2.allPieces[13] = bishop;

    //initialize queens
    struct piece queen;

    //start with player1
    queen.name = 'Q';
    queen.position.col = 'd';
    queen.position.row = 1;
    p1.allPieces[14] = queen;

    //player 2 queen
    queen.name = 'q';
    queen.position.row = 8;
    p2.allPieces[14] = queen;

    //initialize kings
    struct piece king;

    //player1
    king.name = 'K';
    king.position.col = 'e';
    king.position.row = 1;
    p1.allPieces[15] = king;

    //player2
    king.name = 'k';
    king.position.row = 8;
    p2.allPieces[15] = king;
}

void fillBoard() {
    int len = sizeof(p1.allPieces)/sizeof(p1.allPieces[0]);

    for (int i = 0; i < len; i++) {
        struct piece currentPiece = p1.allPieces[i];
        char name = currentPiece.name;
        int arrayRow = rowPositionOnArray(currentPiece.position.row);
        int arrayCol = colPositionOnArray(currentPiece.position.col);
        int boardPosition = findPositionOnBoard(arrayRow, arrayCol);
        board[boardPosition] = name;
        // fill p2's side of the board
        currentPiece = p2.allPieces[i];
        name = currentPiece.name;
        arrayRow = rowPositionOnArray(currentPiece.position.row);
        arrayCol = colPositionOnArray(currentPiece.position.col);
        boardPosition = findPositionOnBoard(arrayRow, arrayCol);
        board[boardPosition] = name;
    }
    printf("%s\n", board);
}
// store rookCheck in a function because we will use later for queen check
// rooks can move fully horizontally or fully vertically
// as long as nothing is in the path it can move completely up/down the board or side to side
// this whole thing will mostly be checking for collision with other pieces
bool rookCheck(int startRow, int startCol, int endRow, int endCol) {
    // firstly check if its moving only vertically or only horizontally
    if (startRow != endRow) { //moving vertically
        if (startCol == endCol) {
            // check if there's anything in the path
            if (startRow < endRow) { // moving down
                for (int i = startRow + 1; i < endRow; i++) { // stop before the end location
                    char currentPiece = board[findPositionOnBoard(i, startCol)];
                    if (currentPiece != ' ') { //check if there's a piece in the way
                        return false;
                    }
                } 
                // if we make it past the for loop there's no pieces in the way
                return true;
                    
            }
            else if (startRow > endRow) { //moving up 
                for (int i = startRow - 1; i > endRow; i--) { // stop before end location
                    char currentPiece = board[findPositionOnBoard(i, startCol)];
                    if (currentPiece != ' ') {
                        return false;
                    }
                 }
                // if we make it past the for loop there's no pieces in the way
                // check if the end location has an enemy piece or empty then return true
                return true;
            }
        }
    }
    if (startCol != endCol) { // moving horizontally
        if (startRow == endRow) {
            // check if there's anything in the path
            if (startCol < endCol) { // moving left
                for (int i = startCol + 1; i < endCol; i++) { // stop before the end location
                    char currentPiece = board[findPositionOnBoard(startRow, i)];
                    if (currentPiece != ' ') { //check if there's a piece in the way
                        return false;
                    }
                } 
                // if we make it past the for loop there's no pieces in the way
                return true;
            }
            else if (startCol > endCol) { //moving right 
                for (int i = startCol - 1; i > endCol; i--) { // stop before end location
                    char currentPiece = board[findPositionOnBoard(startRow, i)];
                    if (currentPiece != ' ') {
                        return false;
                    }
                }
                // if we make it past the for loop there's no pieces in the way
                return true;
            }
        }
    }
    return false;
}
// store bishop check if a function since we will use it for queencheck
// bishops can move in diagonals (up left up right down left down right) across the entire board
// so long as it does not collide with another piece 
// check if it is moving along a diagonal
// check its path to see if theres anything stopping it
// stop before the end location and see if enemy piece there 
bool bishopCheck(int startRow, int startCol, int endRow, int endCol) {
    if (abs(startRow - endRow) == abs(startCol - endCol)) { //moving along a diagonal
        if (startRow > endRow && startCol > endCol) { //moving up left

            int i = startRow - 1, j = startCol - 1;
            while (i > endRow && j > endCol) { // check if there's anything along the path
                char currentPiece = board[findPositionOnBoard(i, j)];
                if (currentPiece != ' ') {
                    return false;
                }
                i--; j--; // decrement
            }
            return true; // if we get through the loop return true

        }
        else if (startRow > endRow && startCol < endCol) { // moving up right

            int i = startRow - 1, j = startCol + 1;
            while (i > endRow && j < endCol) { // check if there's anything along the path
                char currentPiece = board[findPositionOnBoard(i, j)];
                if (currentPiece != ' ') {
                    return false;
                }
                i--, j++; // decrement row increment column
            }
            return true; // if we get through the loop return true
        } 
        else if (startRow < endRow && startCol > endCol) { // moving down left

            int i = startRow + 1, j = startCol - 1;
            while (i < endRow && j > endCol) { // check if there's anything along the path
                char currentPiece = board[findPositionOnBoard(i, j)];
                if (currentPiece != ' ') {
                    return false;
                }
                i++, j--; //increment row decrement column
            }
            return true; // if we get through the loop return true
        }
        else if (startRow < endRow && startCol < endCol) { // moving down right
            int i = startRow + 1, j = startCol + 1;
            while (i < endRow && j < endCol) { // check if there's anything along the path
                char currentPiece = board[findPositionOnBoard(i, j)];
                if (currentPiece != ' ') {
                    return false;
                }
                i++, j++; //increment
            }
            return true; // if we get through the loop return true
        }
    }
    return false;
}
//queens are essentially rooks combined with bishops (can move fully diagonally or fully horizontally/vertically)
// so call the other two helper functions
bool queenCheck(int startRow, int startCol, int endRow, int endCol) {
    return rookCheck(startRow, startCol, endRow, endCol) || bishopCheck(startRow,startCol,endRow,endCol);
}
/*
    checks if the move by the given piece is a legal move according to chess rules and the state of the board
    looking at this from the perpsective of boardArray[][] where the printed matrix is stored in memory as a 2d array under normal conventions
    don't have to worry about out of bounds because already taken care of in moveCheck
    queen bishop and rook checks are done above in helper functions because queens are essentially a combination of both rooks and bishops
*/
bool isLegalMove(char piece, struct position start, struct position end) {
    //initializing variables to the 2d array representation
    int startRow = rowPositionOnArray(start.row);
    int startCol = colPositionOnArray(start.col);
    int endRow = rowPositionOnArray(end.row);
    int endCol = colPositionOnArray(end.col);
    //the potential piece at the end location
    char endLocation = board[findPositionOnBoard(endRow, endCol)];
    // if there's no piece here endLocation == ' ';

    //start with pawns
    /*
        pawns can only move forward one space
        if they have not been touched since the start of the game they can move two spaces
        they cannot go past/collide with other pieces in front of them
        pawns can capture pieces diagonal to them but not in front of them
        i.e. pawns in front of each other block each other off; neither can capture the other
    */
    if (piece == 'P') { //player 1 pawn
        // checking if the player is only moving one piece up into an empty space
        // already established inability to move up if there's an allied piece in the way
        // legal as long as no opposing pieces in end spot
        if (startRow - endRow == 1 && startCol == endCol) {
            if (endLocation == ' ') {
                return true;
            }
        }

        // check if pawn moves up two spaces
        // only works if there's nothing in the path and the pawn is at the start
        // pawn can't skip over pieces so check location in the path as well
        if (startRow - endRow == 2 && startCol == endCol && startRow == 6) {
            char oneSpaceUp = board[findPositionOnBoard(endRow - 1, endCol)];
            // path must be clear of allied and enemy pieces for move to be valid
            if (oneSpaceUp  == ' ' && endLocation == ' ') {
                return true;
            }
        }

        // check if the pawn moves diagonally up
        // this only works if there's an enemy piece at the end location 
        if (startRow - endRow == 1) {
            if (startCol - endCol == 1) {
                if (islower(endLocation)) {
                    return true;
                }
            } else if (startCol - endCol == -1) {
                if (islower(endLocation)) {
                    return true;
                }
            }
        }
        // if one of the conditions was not satisfied return false
        return false;
    }
    if (piece == 'p') { //player 2 pawn
        // checking if the player is only moving one piece up into an empty space
        // already established inability to move up if there's an allied piece in the way
        // legal as long as no opposing pieces in end spot
        if (endRow - startRow == 1 && startCol == endCol) {
            if (endLocation == ' '){
                return true;
            }
        }

        // check if pawn moves up two spaces
        // only works if there's nothing in the path and the pawn is at the start
        // pawn can't skip over pieces so check location in the path as well
        if (endRow - startRow == 2 && startCol == endCol && startRow == 1) {
            char oneSpaceUp = board[findPositionOnBoard(endRow + 1, endCol)];
            // path must be clear of allied and enemy pieces for move to be valid
            if (oneSpaceUp  == ' ' && endLocation == ' ') {
                return true;
            }
        }

        // check if the pawn moves diagonally up
        // this only works if there's an enemy piece at the end location 
        if (endRow - startRow == 1) {
            if (startCol - endCol == 1) {
                if (isupper(endLocation)) {
                    return true;
                }
            } else if (startCol - endCol == -1) {
                if (isupper(endLocation)) {
                    return true;
                }
            }
        }
        // if one of the conditions was not satisfied return false
        return false;
    }

    //check validity of rook moves
    if (piece == 'R' || piece == 'r') { 
        return rookCheck(startRow, startCol, endRow, endCol);
    }
    

    // check validity of horse/knight moves
    // knights can move in L shapes and over other units 
    // the path doesnt matter
    // moveCheck already checks if the end location is a friendly piece/ out of bounds
    // just check if its moving in an L shape 
    if (piece == 'H' || piece == 'h') { // doesnt matter which player; can move in an L in any direction
        if (abs(startRow - endRow) == 2) {
            if (abs(startCol - endCol) == 1) {
                return true;
            }
        }
        else if (abs(startRow - endRow) == 1) {
            if (abs(startCol - endCol) == 2) {
                return true;
            }
        }
        return false;
    }

    // check validity of bishop moves
    if (piece == 'B' || piece == 'b') {
        return bishopCheck(startRow, startCol, endRow, endCol);
    }
    //check validity of queen moves
    if (piece == 'Q' || piece == 'q') {
        return queenCheck(startRow, startCol,endRow, endCol);
    }

    //check validity of king moves
    //king can move one space in any direction so long as it isnt in danger with that move
    //^ this will be taken care of later
    if (piece == 'K' || piece == 'k') {
        if (abs(startRow - endRow) <= 1 && abs(startCol-endCol) <= 1 ){
            return true;
        }
        return false;
    }
    return false;
}

/*
    moves the given piece from start position to end position
    this function will only be called once it passes all the conditions in moveCheck and isLegalMove
    modifies the board array using helper functions 
*/
void move(struct position start, struct position end) {
    // initializing array locations
    int startArrayRow = rowPositionOnArray(start.row);
    int startArrayCol = colPositionOnArray(start.col);
    int endArrayRow = rowPositionOnArray(end.row);
    int endArrayCol = colPositionOnArray(end.col);

    // find the locations on the board array get the start piece
    // replace the endlocation's piece/space with the start piece
    int startLocation = findPositionOnBoard(startArrayRow, startArrayCol);
    char piece = board[startLocation];
    int endLocation = findPositionOnBoard(endArrayRow, endArrayCol);
    board[endLocation] = piece;

    // delete the start piece from its original location
    board[startLocation] = ' ';
}
/*
// when a piece is legally moved check if that piece at its new location
// is a threat to any enemy pieces return the threatened pieces
char* isAttacking(char piece, struct position piecePosition){
    char threatened[9]; //  the max amount of threatened units by one piece is 9; one on each side/corner
    int arrayRow = rowPositionOnArray(piecePosition.row);
    int arrayCol = colPositionOnArray(piecePosition.col);
    int enemyPosition;
    if (piece == 'P') {
        enemyPosition = findPositionOnBoard(arrayRow -1, arrayCol + 1);
        if (board[enemyPosition] != ' ' && islower(board[enemyPosition])) {
            strcat(threatened, board[enemyPosition]);
        }
        enemyPosition = findPositionOnBoard(arrayRow-1, arrayCol +1); 
            if (board[enemyPosition] != ' ' && islower(board[enemyPosition])) {
            strcat(threatened, board[enemyPosition]);
        }
    }
    return threatened;
}
// check all sides of king outwards see if there are enemy pieces/allied pieces blocking
// if we hit an allied piece/nonthreatening piece we can stop searching
// check forward back and sides of king to see if theres an enemy queen/rook and 1 space out for protected enemy king
// check diagonal one space out to see if theres a pawn (right direction) or protected enemy king, continue searching for bishops if no allied/nonthreatening pieces before
// just always check if theres a horse in an L shape away from the king
bool kingInDanger(struct position kingPosition) {
    char danger;
    

}

// if you're in check see if there's any way to eliminate/block the threat; 
// those will be the only possible moves
// if there aren't any possible moves then checkmate returns true
bool checkMate(struct player player, struct position kingPosition) {

}
*/
/**
 * checks the validity of a move
 * will either move the piece or print an error message depending on what went wrong
 * scans the board for the piece that is in the start position and checks if 
 * moving to the end location is a legal move/possible move
 */
bool moveCheck(struct player player, struct position start, struct position end) {

    //handle out of bounds cases 
    if (start.row < 1 || start.row > 8 || start.col < 'a' || start.col > 'h') {
        printf("%c%d is not a valid start location!\n", start.col, start.row);
        return false;
    }
    if (end.row < 1 || end.row > 8 || end.col < 'a' || end.col > 'h') {
        printf("%c%d is not a valid end location!\n", start.col, start.row);
        return false;
    }

    if (start.row == end.row && start.col == end.col) {
        printf("You have to move a piece!\n");
        return false;
    }

    //check/get the actual piece at that location
    int rowStart = rowPositionOnArray(start.row);
    int colStart = colPositionOnArray(start.col);

    char startPiece = board[findPositionOnBoard(rowStart,colStart)];
    if (startPiece == ' ') {
        printf("There is no piece at %c%d!\n", start.col, start.row);
        return false;
    }
    if ((player.player1 && islower(startPiece)) || (!player.player1 && isupper(startPiece))) {
        printf("The piece at %c%d is not yours!\n", start.col, start.row);
        return false;
    }

    int rowEnd = rowPositionOnArray(end.row);
    int colEnd = colPositionOnArray(end.col);
    char endPiece = board[findPositionOnBoard(rowEnd, colEnd)];

    if ((player.player1 && isupper(endPiece)) || (!player.player1 && islower(endPiece))) { // cant overlap with own pieces
        printf("One of your pieces is at %c%d already\n", end.col, end.row);
        return false;
    }
    /* at this point we've established the following: 
        the start and end locations are within bounds of the board
        the piece belongs to the appropriate player
        the end location does not overlap with a piece from the same player
        next we have to check if the end location is a valid move given the piece/state of the board
    */
   if (isLegalMove(startPiece, start, end)) {
    //call inCheck twice
    //check if after the move the player will still be in check; that move will be invalid
       // check if the move puts your king in danger of if it checks the enemy king 
       // check if a pawn is being promoted
       printf("LEGAL MOVE\n");
       move(start, end);
       return true;
   } else {
       printf("Not a valid move\n");
       return false;
   }
   return false;

}
/*
implement a way to check/checkmate
*/
int main() {
    generateBoard();
    instantiatePlayerPieces();
    fillBoard();
    int turnCounter = 1;
    static char termination[4] = "stop"; 
    do { 
        struct position start;
        struct position end;
        char startCol;
        int startRow;
        char endCol;
        int endRow;
        char userInput[4];
        bool valid = false;
        if (turnCounter %2 == 1){
            while (valid == false) {
                printf("Player 1 turn (your pieces are in uppercase):\n");
                printf("Enter your move (format: type 'd2d4' to move piece on d2 to d4)\n");
                printf("Enter 'stop' to terminate the program\n");
                // scan the user input and check if its formatted and what it says
                
                scanf("%s", userInput);
                
                //check if the user inputs 'stop' keyword
                if (strcasecmp(userInput, termination) == 0) {
                    printf("program terminated.");
                    goto terminate;
                }
                // the following if statements check the formatting of the 
                // the user input and go back to the print statements if bad formatting
                
                
                if (userInput[0] < 'a' || userInput[0] > 'z') {
                    printf("format as shown in example\n");
                    continue;
                }
                if (userInput[1] < '1' || userInput[1] > '8') {
                    printf("format as shown in example\n");
                    continue;
                }
                if (userInput[2] < 'a' || userInput[2] > 'z') {
                    printf("format as shown in example\n");
                    continue;
                }
                if (userInput[3] < '1' || userInput[3] > '8') {
                    printf("format as shown in example\n");
                    continue;
                }
                char copy[5]; 
                strncpy(copy, userInput, 4);
                copy[4] = '\0'; 
                sscanf(copy, "%c%d%c%d", &startCol, &startRow, &endCol, &endRow);
                start.row = startRow;
                start.col = startCol;
                end.row = endRow;
                end.col = endCol;
                valid = moveCheck(p1, start, end);
            }
            

        } else {
            while (valid == false) {
                printf("Player 2 turn (your pieces are in lowercase):\n");
                printf("Enter your move (format: type 'g8f6' to move piece on g8 to f6)\n");
                printf("Enter 'stop' to terminate the program\n");
                // scan the user input and check if its formatted and what it says
                scanf("%s", userInput);
                
                //check if the user inputs 'stop' keyword
                if (strcasecmp(userInput, termination) == 0) {
                    goto terminate;
                }

                // the following if statements check the formatting of the 
                // the user input and go back to the print statements if bad formatting
                if (userInput[0] < 'a' || userInput[0] > 'z') {
                    printf("format as shown in example\n");
                    continue;
                }
                if (userInput[1] < '1' || userInput[1] > '8') {
                    printf("format as shown in example\n");
                    continue;
                }
                if (userInput[2] < 'a' || userInput[2] > 'z') {
                    printf("format as shown in example\n");
                    continue;
                }
                if (userInput[3] < '1' || userInput[3] > '8') {
                    printf("format as shown in example\n");
                    continue;
                }
                char copy[5]; 
                strncpy(copy, userInput, 4);
                copy[4] = '\0'; 
                sscanf(copy, "%c%d%c%d", &startCol, &startRow, &endCol, &endRow);
                start.row = startRow;
                start.col = startCol;
                end.row = endRow;
                end.col = endCol;
                valid = moveCheck(p2, start, end);
            }
            
        }
        turnCounter++;
        printf("%s\n", board);
    } while (p1.inCheckMate == false && p2.inCheckMate == false);
    terminate: printf("program terminated.");

} 
