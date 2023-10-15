#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "Escape.h"
#include "Piece.h"

int pieceExists(char *piece) {
    return *piece != '_';
}

char* getPiece(char *board, int row, int column) {
    char* pRow = board + column * 16;
    char* piece = pRow + row * 2;
    return piece;
}

void setPiece(char *board, char* piece, char newPiece, char team) {
    *piece = newPiece;
    *(piece + 1) = team;
}

void getCoordinate(char *board, char* piece, char coordinate[2]) {
    double idx = (piece - board) * 0.5;
    int row = floor((int)idx % 8);
    int column = floor(idx / 8.0);

    coordinate[0] = row;
    coordinate[1] = column;
}

void setCoordinate(char *board, int row, int column, char newPiece, char team) {
    char *piece = getPiece(board, row, column);
    *piece = newPiece;
    *(piece + 1) = team;
}

int isPathObstructed(char *board, char *from, char *to) {
    char fromCoordinate[2];
    char toCoordinate[2];
    char moveH, moveV;
    getCoordinate(board, from, fromCoordinate);
    getCoordinate(board, to, toCoordinate);
    moveV = toCoordinate[0] - fromCoordinate[0];
    moveH = toCoordinate[1] - fromCoordinate[1];

    int moves = abs(moveV) > abs(moveH) ? abs(moveV) : abs(moveH);

    int signV = moveV == 0 ? 0 : moveV / abs(moveV);
    int signH = moveH == 0 ? 0 : moveH / abs(moveH);
    for (int i = 1; i < moves; i++) {
        int lookRow = fromCoordinate[0] + i * signV;
        int lookCol = fromCoordinate[1] + i * signH;
        // printf("%d, %d", lookRow, lookCol);
        if (pieceExists(getPiece(board, lookRow, lookCol))) {
            return 1;
        }
    }
    return 0;
}

int isValidMove(char *board, char *piece, char* to, int isTakingPiece, int noErr) {
    struct Piece pieceInfo = getPieceInfo(*piece);
    char type = piece[0];
    char team = piece[1];
    char fromCoordinate[2];
    char toCoordinate[2];
    char moveH, moveV;
    getCoordinate(board, piece, fromCoordinate);
    getCoordinate(board, to, toCoordinate);
    moveV = toCoordinate[0] - fromCoordinate[0];
    moveH = toCoordinate[1] - fromCoordinate[1];
    int teamDir = team == 'a' ? 1 : -1;
    // printf("%d, %d", moveH, moveV);
    if (abs(moveV) == abs(moveH)) { // diagonal move // todo: catches for pawn
        if (isPathObstructed(board, piece, to)) {
            if (!noErr)
                printf("View obstructed: ");
            return 0;
        }
        int rep = abs(moveV);
        if (pieceInfo.dMove >= rep || (type == 'p' && isTakingPiece)) {
            if (pieceInfo.fwdOnly && moveH * teamDir < 0) {
                return 0;
            }
            return 1;
        } else {
            return 0;
        }
    } else if (moveV == 0 || moveH == 0) { // straight move // todo: first move 2 for pawn
        if (isPathObstructed(board, piece, to)) {
            if (!noErr)
                printf("View obstructed: ");
            return 0;
        }
        int rep = moveV ? moveV : moveH;
        if (pieceInfo.sMove >= abs(rep)) {
            if (pieceInfo.fwdOnly && rep * teamDir < 0) {
                return 0;
            }
        } else {
            return 0;
        }
    } else { // other (assumed knight)
        return 0;
    }
    return 1;
}

int canTakePiece(char *board, char* pieceA, char* pieceB) {
    char teamA = pieceA[1];
    char teamB = pieceB[1];
    int opposingTeams = teamA != teamB;
    if (!opposingTeams) {
        return 0;
    }
    return 1;
}

int moveTo(char *board, char* piece, char* to, char team, char pieceTaken[2]) {
    if (!pieceExists(piece)) {
        printf("There's no piece there!\n");
        return 0;
    }
    int takingPiece = 0;
    if (pieceExists(to)) {
        takingPiece = canTakePiece(board, piece, to);
        if (!takingPiece) {
            printf("Can't move there!\n");
            return 0;
        }
    }
    if (!isValidMove(board, piece, to, takingPiece, 0)) {
        printf("Not a valid move!\n");
        return 0;
    }

    char owner = *(piece + 1);
    if (owner != team) {
        printf("Not your piece!\n");
        return 0;
    }
    pieceTaken[0] = to[0];
    pieceTaken[1] = to[1];
    char name = *piece;
    setPiece(board, piece, '_', '_');
    setPiece(board, to, name, team);
    return 1;
}

void undoMove(char *board, char* from, char* piece, char *pieceTaken) {
    setPiece(board, from, *piece, piece[1]);
    if (pieceExists(piece)) {
        setPiece(board, piece, pieceTaken[0], pieceTaken[1]);
    } else {
        setPiece(board, piece, '_', '_');
    }
}

void printBoard(char *board, char turn, int inCheck) {
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
            char* piece = getPiece(board, row, col);
            char owner = piece[1];
            printf("%s%c  " RESET, owner == 'a' ? GRN : (owner == 'b' ? BLU : WHT), *piece);
        }
        // printf("  %s", row == 3 ? "b" : "");
        printf("\n");
    }
}

void createTeamLayout(char *board, char team) {
    static char layout[] = "rnbqkbnr";
    for (int col = 0; col < 2; col++) {
        for (int row = 0; row < 8; row++) {
            char piece = col == 0 ? layout[row] : 'p';
            setCoordinate(board, row, team == 'a' ? col : 7 - col, piece, team);
        }
    }
}

char* createBoard(char turn) {
    char *board = malloc(sizeof(char) * 8 * 16); // each piece is 2 chars, one for piece id, one for team id
    memset(board, '_', sizeof(char) * 8 * 16);
    createTeamLayout(board, 'a');
    createTeamLayout(board, 'b');
    return board;
}

void getTeamKing(char *board, char team, char **king) {
    for (int column = 0; column < 8; column++) {
        for (int row = 0; row < 8; row++) {
            char* piece = getPiece(board, row, column);
            char pieceOwner = piece[1];
            char pieceType = piece[0];
            if (pieceOwner == team && pieceType == 'k') {
                *king = piece;
            }
        }
    }
}

void getTeamPieces(char *board, char team, char *teamPieces[16], int *numPieces) {
    *numPieces = 0;
    for (int column = 0; column < 8; column++) {
        for (int row = 0; row < 8; row++) {
            char* piece = getPiece(board, row, column);
            char pieceOwner = piece[1];
            if (pieceOwner == team) {
                teamPieces[*numPieces] = piece;
                (*numPieces)++;
            }
        }
    }
}
