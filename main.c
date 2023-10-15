#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <regex.h>
#include "Board.h"
#include "Check.h"

void resolveMoveInstructions(char* input, int *fromCoordinates, int *toCoordinates) {
    regmatch_t pmatch[5];
    regex_t regex;
    int compiled = regcomp(&regex, "([0-9]+),([0-9]+)->([0-9]+),([0-9]+)", REG_EXTENDED);
    int regRes = regexec(&regex, input, (size_t) 5, pmatch, 0);
    regfree(&regex);

    fromCoordinates[0] = atoi(&input[pmatch[1].rm_so]);
    fromCoordinates[1] = atoi(&input[pmatch[2].rm_so]);
    toCoordinates[0] = atoi(&input[pmatch[3].rm_so]);
    toCoordinates[1] = atoi(&input[pmatch[4].rm_so]);
}

int resolveUserInput(int *coordinates) {
    char input[9]; // max expected input length is 9
    scanf("%s", input);

    if (strcmp(input, "q") == 0) {
        return 0;
    } else {
        resolveMoveInstructions(input, coordinates, coordinates + 2);
        return 1;
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
    for (int i = 0; i < 10 + *printedLines; i++)
        deleteLine();
    *printedLines = 0;
}

int main() {
    char turn = 'a'; // which team is going
    char *board = createBoard(turn);
    int printedLines = 0;
    int inCheck = 0;
    while (1) {
        inCheck = isInCheck(board, turn);
        printBoard(board, turn, inCheck);
        printedLines += inCheck;

        int coordinates[4];
        int *fromCoordinates = coordinates;
        int *toCoordinates = coordinates + 2;
        int res = resolveUserInput(coordinates);
        printedLines++;

        char pieceTaken[2];
        // printf("%d %d %d %d", fromCoordinates[0], fromCoordinates[1], toCoordinates[0], toCoordinates[1]);
        if (moveTo(board, getPiece(board, fromCoordinates[1], fromCoordinates[0]), getPiece(board, toCoordinates[1], toCoordinates[0]), turn, pieceTaken)) {
            clearTerminal(&printedLines);
            switchTurn(&turn);
        } else printedLines ++;
    }
    free(board);
    return 0;
}
