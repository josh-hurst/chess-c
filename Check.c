#include <stdio.h>
#include "Board.h"

int isInCheck(char **board, char team) {
    char attackingTeam = team == 'b' ? 'a' : 'b';
    Team teamBoard = getTeamBoard(board, attackingTeam);
    char **king = getTeamBoard(board, team).king;
    char ***teamCells = teamBoard.cells;
    for (int i = 0; i < teamBoard.len; i++) {
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
    Team teamBoard = getTeamBoard(board, team);
    char ***teamCells = teamBoard.cells;
    
    for (int i = 0; i < teamBoard.len; i++) {
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
