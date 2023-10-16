#include <stdio.h>
#include "Board.h"

int isInCheck(char **board, char team) {
    char attackingTeam = team == 'b' ? 'a' : 'b';
    char **king;
    char **teamCells[16];
    int numPieces;
    getTeamCells(board, attackingTeam, teamCells, &numPieces);
    getTeamKing(board, team, &king);
    for (int i = 0; i < numPieces; i++) {
        char** cell = teamCells[i];
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                if (isValidMove(board, cell, king, 1, 1)) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

int isInCheckMate(char **board, char team) {
    char **teamCells[16];
    char **king;
    int numPieces;
    getTeamCells(board, team, teamCells, &numPieces);
    getTeamKing(board, team, &king);
    
    for (int i = 0; i < numPieces; i++) {
        char **cell = teamCells[i];
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                char **toCell = getCell(board, row, col);
                char *w;
                int success = !moveTo(board, cell, toCell, team, &w, 1);
                if (success) {
                    return 0;
                } else {
                }
            }
        }
    }
    return 1;
}
