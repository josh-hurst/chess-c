#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <regex.h>
#include "Board.h"
#include "Check.h"

void deleteLine() {
    printf("\33[2K\r");
    printf("\033[A");
}

int main() {
    char turn = 'a'; // which team is going
    char *board = createBoard(turn);
    int printedLines = 0;
    int inCheck = 0;
    while (1) {
        char moveStr[9];
        scanf("%s", moveStr);
        printedLines++;

        regmatch_t pmatch[5];
        regex_t regex;
        int compiled = regcomp(&regex, "([0-9]+),([0-9]+)->([0-9]+),([0-9]+)", REG_EXTENDED);
        int regRes = regexec(&regex, moveStr, (size_t) 5, pmatch, 0);
        regfree(&regex);

        int fromRow, fromColumn, toRow, toColumn;
        fromColumn = atoi(&moveStr[pmatch[1].rm_so]);
        fromRow = atoi(&moveStr[pmatch[2].rm_so]);
        toColumn = atoi(&moveStr[pmatch[3].rm_so]);
        toRow = atoi(&moveStr[pmatch[4].rm_so]);
        // printf("%d, %d, %d, %d", fromRow, fromColumn, toRow, toColumn);
        char pieceTaken[2];
        if (moveTo(board, getPiece(board, fromRow, fromColumn), getPiece(board, toRow, toColumn), turn, pieceTaken)) {
            for (int i = 0; i < 10 + printedLines; i++)
                deleteLine();
            turn = turn == 'a' ? 'b' : 'a';
            inCheck = isInCheck(board, turn);
            printBoard(board, turn, inCheck);
            printedLines = inCheck;
            // printf("Piece taken: %c", pieceTaken[1]);
            // scanf("%s", moveStr);
            // undoMove(board, getPiece(board, fromRow, fromColumn), getPiece(board, toRow, toColumn), pieceTaken);
            // printBoard(board, turn);
        } else printedLines ++;
    }
    free(board);
    return 0;
}
