#ifndef BOARD_H
#define BOARD_H

int pieceExists(char *piece);

char* getPiece(char *board, int row, int column);
void setPiece(char *board, char* piece, char newPiece, char team, char flags);
void getCoordinate(char *board, char* piece, char coordinate[2]);
void setCoordinate(char *board, int row, int column, char newPiece, char team, char flags);

void getTeamPieces(char *board, char team, char *teamPieces[16], int *numPieces);
void getTeamKing(char *board, char team, char **king);

int isPathObstructed(char *board, char *from, char *to);
int isValidMove(char *board, char *piece, char* to, int isTakingPiece, int noErr);
int canTakePiece(char *board, char* pieceA, char* pieceB);

int moveTo(char *board, char* piece, char* to, char team, char pieceTaken[3]);
void undoMove(char *board, char* from, char* piece, char *pieceTaken);

void createTeamLayout(char *board, char team);
char* createBoard(char turn);

void printBoard(char *board, char turn, int inCheck);

#endif
