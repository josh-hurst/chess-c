#ifndef BOARD_H
#define BOARD_H

typedef struct CellCoordinate {
    int row;
    int column;
} CellCoordinate;

struct Board {
    char** layout;
    char* teamA;
    char* teamB;
};

int pieceExists(char *piece);

char** getCell(char **board, int row, int column);
void setCell(char **board, int row, int column, char *piece);
char* getPiece(char **board, int row, int column);
void setPiece(char **board, char* piece, char newPiece, char team, char flags);
struct CellCoordinate getCoordinate(char **board, char **cell);

void getTeamCells(char **board, char team, char **teamPieces[16], int *numPieces);
void getTeamKing(char **board, char team, char ***king);

int isPathObstructed(char **board, char **fromCell, char **toCell);
int isValidMove(char **board, char **fromCell, char **toCell, int isTakingPiece, int noErr);
int canTakePiece(char **board, char* pieceA, char* pieceB);

int moveTo(char **board, char** fromCell, char** toCell, char team, char **pieceTaken, int simulate);
void undoMove(char **board, char* from, char* piece, char *pieceTaken);

void createTeamLayout(char **board, char team, char *pieces);
struct Board createBoard(char turn);

void printBoard(char **board, char turn, int inCheck, int inCheckMate);

#endif
