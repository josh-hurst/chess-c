#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "Board.h"
#include "Escape.h"
#include "Piece.h"
#include "Check.h"

int pieceExists(char *piece) {
    return *piece != '_';
}

char** getCell(char **board, int row, int column) {
    char **pRow = board + (column * 8);
    char **cell = pRow + row;
    return cell;
}

void setCell(char **board, int row, int column, char *piece) {
    char **cell = getCell(board, row, column);
    *cell = piece;
}

char* getPiece(char **board, int row, int column) {
    char *pRow = board[column * 8];
    char *piece = pRow + row;
    return piece;
}

void setPiece(char **board, char* piece, char newPiece, char team, char flags) {
    *piece = newPiece;
    *(piece + 1) = team;
}

struct CellCoordinate getCoordinate(char **board, char **cell) {
    double idx = cell - board;
    int row = floor((int)idx % 8);
    int column = floor(idx / 8.0);
    struct CellCoordinate coordinate = {row, column};
    return coordinate;
}

int isPathObstructed(char **board, char **fromCell, char **toCell) {
    CellCoordinate fromCoordinate = getCoordinate(board, fromCell);
    CellCoordinate toCoordinate = getCoordinate(board, toCell);
    int diffCol = toCoordinate.column - fromCoordinate.column;
    int diffRow = toCoordinate.row - fromCoordinate.row;

    int moves = abs(diffCol) > abs(diffRow) ? abs(diffCol) : abs(diffRow);

    int signV = diffCol == 0 ? 0 : diffCol / abs(diffCol);
    int signH = diffRow == 0 ? 0 : diffRow / abs(diffRow);
    for (int i = 1; i < moves; i++) {
        int lookCol = fromCoordinate.column + i * signV;
        int lookRow = fromCoordinate.row + i * signH;
        char **obstruction = getCell(board, lookRow, lookCol);
        if (*obstruction) {
            return 1;
        }
    }
    return 0;
}

int max(int a, int b) {
    return a > b ? a : b;
}

int min(int a, int b) {
    return a < b ? a : b;
}

int isPawnsFirstMove(char *piece) {
    return !(piece[2] & HAS_MOVED);
}

int isValidMove(char **board, char **fromCell, char **toCell, int isTakingPiece, int noErr) {
    char *piece = *fromCell;
    char type = piece[0];
    char team = piece[1];
    struct Piece pieceInfo = getPieceInfo(*piece);
    CellCoordinate fromCoordinate = getCoordinate(board, fromCell);
    CellCoordinate toCoordinate = getCoordinate(board, toCell);
    int diffCol = toCoordinate.column - fromCoordinate.column;
    int diffRow = toCoordinate.row - fromCoordinate.row;
    int teamDir = team == 'a' ? 1 : -1;

    if (abs(diffCol) == abs(diffRow)) { // diagonal move // todo: catches for pawn
        if (isPathObstructed(board, fromCell, toCell)) {
            if (!noErr)
                printf("View obstructed: ");
            return 0;
        }
        int rep = abs(diffCol);
        if (pieceInfo.dMove >= rep || (type == 'p' && isTakingPiece && rep == 1)) {
            if (pieceInfo.fwdOnly && diffRow * teamDir < 0) {
                return 0;
            }
            return 1;
        } else {
            return 0;
        }
    } else if (diffCol == 0 || diffRow == 0) { // straight move // todo: first move 2 for pawn
        if (type == 'p' && isTakingPiece) {
            if (!noErr) {
                printf("Pawn can only take diagonally!");
                return 0;
            }
        }
        if (isPathObstructed(board, fromCell, toCell)) {
            if (!noErr)
                printf("View obstructed: ");
            return 0;
        }
        int rep = diffCol ? diffCol : diffRow;
        if ((isPawnsFirstMove(piece) ? 2 : pieceInfo.sMove) >= abs(rep)) {
            if (pieceInfo.fwdOnly && rep * teamDir < 0) {
                return 0;
            }
            return 1;
        } else {
            return 0;
        }
    } else { // other (assumed knight)
        int maxMoveRule = max(pieceInfo.sMove, pieceInfo.pMove);
        int minMoveRule = min(pieceInfo.sMove, pieceInfo.pMove);
        int maxMove = max(abs(diffCol), abs(diffRow));
        int minMove = min(abs(diffCol), abs(diffRow));
        if (minMove == minMoveRule && maxMove == maxMoveRule) {
            return 1;
        } else
            return 0;
    }
    return 1;
}

int canTakePiece(char **board, char* pieceA, char* pieceB) {
    char teamA = pieceA[1];
    char teamB = pieceB ? pieceB[1] : '0';
    int opposingTeams = teamA != teamB;
    if (!opposingTeams) {
        return 0;
    }
    return 1;
}

int moveTo(char **board, char** fromCell, char** toCell, char team, char **pieceTaken) {
    if (!*fromCell) {
        printf("There's no piece there!\n");
        return 0;
    }

    char fromPieceOwner = (*fromCell)[1];
    if (fromPieceOwner != team) {
        printf("Not your piece!\n");
        return 0;
    }

    int takingPiece = 0;
    if (*toCell) {
        takingPiece = canTakePiece(board, *fromCell, *toCell);
        if (!takingPiece) {
            printf("Can't move there!\n");
            return 0;
        }
    }

    if (!isValidMove(board, fromCell, toCell, takingPiece, 0)) {
        printf("Not a valid move!\n");
        return 0;
    }

    
    if (*toCell)
        *pieceTaken = *toCell;

    *toCell = *fromCell;
    *fromCell = 0;
    if (isInCheck(board, team)) {
        *fromCell = *toCell;
        *toCell = *pieceTaken;
        if (*pieceTaken)
            *pieceTaken = NULL;

        printf("Can't move into check!\n");
        return 0;
    } else {
        (*toCell)[2] |= HAS_MOVED;
    }
    return 1;
}

void printBoard(char **board, char turn, int inCheck) {
    printf("\t%s%c's turn\n" RESET, turn == 'a' ? GRN : (turn == 'b' ? BLU : WHT), turn);
    if (inCheck) {
        printf(RED "\tCHECK\n");
    }
    printf(" " ANSI_UNDERLINED_PRE "  ");
    for (int col = 0; col < 8; col++) {
        printf("%d  ", col + 1);
    }
    printf(ANSI_UNDERLINED_POST "\n");
    for (int row = 0; row < 8; row++) {
        printf("%d│ ", row + 1);
        for (int col = 0; col < 8; col++) {
            char** cell = getCell(board, row, col);
            if (*cell) {
                char *piece = *cell;
                char type = piece[0], owner = piece[1];
                printf("%s%c  " RESET, owner == 'a' ? GRN : (owner == 'b' ? BLU : WHT), type);
            } else
                printf("_  ");
        }
        printf("\n");
    }
}

void createTeamLayout(char **board, char team, char *pieces) {
    static char layout[] = "rnbqkbnr";
    for (int col = 0; col < 2; col++) {
        for (int row = 0; row < 8; row++) {
            int idx = col * 8 + row;
            char type = col == 0 ? layout[row] : 'p';
            char *piece = pieces + idx * PIECE_SIZE;
            piece[0] = type;
            piece[1] = team;
            piece[2] = 0;
            board[team == 'a' ? idx : 63 - idx] = piece;
        }
    }
}

struct Board createBoard(char turn) {
    char *aPieces = calloc(16, sizeof(char) * PIECE_SIZE);
    char *bPieces = calloc(16, sizeof(char) * PIECE_SIZE);
    char **layout = calloc(64, sizeof(char*)); // each piece is 3 chars, one for piece id, one for team id
    createTeamLayout(layout, 'a', aPieces);
    createTeamLayout(layout, 'b', bPieces);
    struct Board board = {layout, aPieces, bPieces};
    return board;
}

void getTeamKing(char **board, char team, char ***king) {
    for (int column = 0; column < 8; column++) {
        for (int row = 0; row < 8; row++) {
            char **cell = getCell(board, row, column);
            char *piece = *cell;
            if (piece) {
                char pieceOwner = piece[1];
                char pieceType = piece[0];
                if (pieceOwner == team && pieceType == 'k') {
                    *king = cell;
                }
            }
        }
    }
}

void getTeamCells(char **board, char team, char **teamCells[16], int *numPieces) {
    *numPieces = 0;
    for (int column = 0; column < 8; column++) {
        for (int row = 0; row < 8; row++) {
            char **cell = getCell(board, row, column);
            char *piece = *cell;
            if (piece) {
                char pieceOwner = piece[1];
                if (pieceOwner == team) {
                    teamCells[*numPieces] = cell;
                    (*numPieces)++;
                }
            }
        }
    }
}
