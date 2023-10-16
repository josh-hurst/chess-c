#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <regex.h>
#include "Board.h"
#include "Check.h"

int resolveMoveInstructions(char* input, int *fromCoordinates, int *toCoordinates) {
    regmatch_t pmatch[5];
    regex_t regex;
    int compiled = regcomp(&regex, "([0-9]+),([0-9]+)->([0-9]+),([0-9]+)", REG_EXTENDED);
    if (compiled)
        return 0;
    int regRes = regexec(&regex, input, (size_t) 5, pmatch, 0);
    regfree(&regex);
    if (regRes)
        return 0;

    fromCoordinates[0] = atoi(&input[pmatch[1].rm_so]) - 1;
    fromCoordinates[1] = atoi(&input[pmatch[2].rm_so]) - 1;
    toCoordinates[0] = atoi(&input[pmatch[3].rm_so]) - 1;
    toCoordinates[1] = atoi(&input[pmatch[4].rm_so]) - 1;
    return 1;
}

int resolveUserInput(int *coordinates) {
    char input[9]; // max expected input length is 9
    scanf("%s", input);

    if (strcmp(input, "q") == 0) {
        return 0;
    } else {
        if (resolveMoveInstructions(input, coordinates, coordinates + 2))
            return 1;
        else
            return 0;
    }
}

void switchTurn(char *turn) {
    *turn = *turn == 'a' ? 'b' : 'a';
}

void deleteLine() {
    printf("\33[2K\r");
    printf("\033[A");
}

void clearTerminal(int *printedLines) {
    if (1) return;
    for (int i = 0; i < 10 + *printedLines; i++)
        deleteLine();
    *printedLines = 0;
}

int main() {
    char turn = 'a'; // which team is going
    char *board = createBoard(turn);
    int printedLines = 0;
    int moveSuccess = 1;
    while (1) {
        if (moveSuccess) {
            int inCheck = isInCheck(board, turn);
            printBoard(board, turn, inCheck);
            printedLines += inCheck;
        }

        int coordinates[4];
        int *fromCoordinates = coordinates;
        int *toCoordinates = coordinates + 2;
        int success = resolveUserInput(coordinates);
        if (!success) {
            printf("Failed to read input.\n");
            printedLines++;
            continue;
        }
        printedLines++;

        char pieceTaken[3];
        char *fromPiece = getPiece(board, fromCoordinates[1], fromCoordinates[0]);
        char *toPiece = getPiece(board, toCoordinates[1], toCoordinates[0]);
        if (moveTo(board, fromPiece, toPiece, turn, pieceTaken)) {
            clearTerminal(&printedLines);
            switchTurn(&turn);
            moveSuccess = 1;
        } else {
            printedLines ++;
            moveSuccess = 0;
        }
    }
    free(board);
    return 0;
}
