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

char* getPiece(char *board, int row, int column) {
    char* pRow = board + column * 8 * PIECE_SIZE;
    char* piece = pRow + row * PIECE_SIZE;
    return piece;
}

void setPiece(char *board, char* piece, char newPiece, char team, char flags) {
    *piece = newPiece;
    *(piece + 1) = team;
}

void getCoordinate(char *board, char* piece, char coordinate[2]) {
    double idx = (piece - board) / (double)PIECE_SIZE;
    int row = floor((int)idx % 8);
    int column = floor(idx / 8.0);

    coordinate[0] = row;
    coordinate[1] = column;
}

void setCoordinate(char *board, int row, int column, char newPiece, char team, char flags) {
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

int max(int a, int b) {
    return a > b ? a : b;
}

int min(int a, int b) {
    return a < b ? a : b;
}

int isPawnsFirstMove(char *piece) {
    return !(piece[2] & HAS_MOVED);
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
        if (pieceInfo.dMove >= rep || (type == 'p' && isTakingPiece && rep == 1)) {
            if (pieceInfo.fwdOnly && moveH * teamDir < 0) {
                return 0;
            }
            return 1;
        } else {
            return 0;
        }
    } else if (moveV == 0 || moveH == 0) { // straight move // todo: first move 2 for pawn
        if (type == 'p' && isTakingPiece) {
            if (!noErr) {
                printf("Pawn can only take diagonally!");
                return 0;
            }
        }
        if (isPathObstructed(board, piece, to)) {
            if (!noErr)
                printf("View obstructed: ");
            return 0;
        }
        int rep = moveV ? moveV : moveH;
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
        int maxMove = max(abs(moveV), abs(moveH));
        int minMove = min(abs(moveV), abs(moveH));
        if (minMove == minMoveRule && maxMove == maxMoveRule) {
            return 1;
        } else
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

int moveTo(char *board, char* piece, char* to, char team, char pieceTaken[3]) {
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
    pieceTaken[2] = to[2];
    char name = *piece;
    char flags = piece[2];
    setPiece(board, piece, '_', '_', 0);
    setPiece(board, to, name, team, flags);
    if (isInCheck(board, team)) {
        setPiece(board, piece, name, team, flags);
        setPiece(board, to, pieceTaken[0], pieceTaken[1], pieceTaken[2]);
        pieceTaken[0] = 0;
        pieceTaken[1] = 0;
        pieceTaken[2] = 0;
        printf("Can't move into check!\n");
        return 0;
    } else {
        to[2] |= HAS_MOVED;
    }
    return 1;
}

void undoMove(char *board, char* from, char* piece, char *pieceTaken) {
    setPiece(board, from, *piece, piece[1], piece[2]);
    if (pieceExists(piece)) {
        setPiece(board, piece, pieceTaken[0], pieceTaken[1], pieceTaken[2]);
    } else {
        setPiece(board, piece, '_', '_', 0);
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
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 8; row++) {
            if (col < 2) {
                char piece = col == 0 ? layout[row] : 'p';
                setCoordinate(board, row, team == 'a' ? col : 7 - col, piece, team, 0);
            } else {
                setCoordinate(board, row, team == 'a' ? col : 7 - col, '_', '_', 0);
            }
        }
    }
}

char* createBoard(char turn) {
    char *board = malloc(sizeof(char) * 8 * 8 * PIECE_SIZE); // each piece is 3 chars, one for piece id, one for team id
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
