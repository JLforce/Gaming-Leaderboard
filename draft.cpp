#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

#define WIDTH 30
#define HEIGHT 20
#define MAX_LENGTH 100

typedef struct {
    int x, y;
} Position;

Position snake[MAX_LENGTH];
Position food;
int snake_length = 3;
int direction = 0; // 0=Right, 1=Left, 2=Up, 3=Down
int score = 0;
int speed = 100;
bool game_over = false;

void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void generateFood() {
    food.x = rand() % (WIDTH - 2) + 1;
    food.y = rand() % (HEIGHT - 2) + 1;
}

void drawBoard() {
    system("cls");
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1) {
                printf("#");
            } else if (i == food.y && j == food.x) {
                printf("*");
            } else {
                bool printed = false;
                for (int k = 0; k < snake_length; k++) {
                    if (snake[k].x == j && snake[k].y == i) {
                        printf("O");
                        printed = true;
                        break;
                    }
                }
                if (!printed) printf(" ");
            }
        }
        printf("\n");
    }
    printf("Score: %d\n", score);
}

void moveSnake() {
    for (int i = snake_length - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }

    switch (direction) {
        case 0: snake[0].x++; break;
        case 1: snake[0].x--; break;
        case 2: snake[0].y--; break;
        case 3: snake[0].y++; break;
    }

    if (snake[0].x == food.x && snake[0].y == food.y) {
        snake_length++;
        score += 10;
        generateFood();
        speed = (speed > 30) ? speed - 5 : speed; // Increase difficulty
    }

    for (int i = 1; i < snake_length; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            game_over = true;
        }
    }

    if (snake[0].x == 0 || snake[0].x == WIDTH - 1 || snake[0].y == 0 || snake[0].y == HEIGHT - 1) {
        game_over = true;
    }
}

void saveScore() {
    FILE *file = fopen("leaderboard.txt", "a");
    fprintf(file, "%d\n", score);
    fclose(file);
}

void sortLeaderboard() {
    FILE *file = fopen("leaderboard.txt", "r");
    int scores[100], count = 0;
    while (fscanf(file, "%d", &scores[count]) != EOF) {
        count++;
    }
    fclose(file);
    
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (scores[j] > scores[i]) {
                int temp = scores[i];
                scores[i] = scores[j];
                scores[j] = temp;
            }
        }
    }
    
    file = fopen("leaderboard.txt", "w");
    for (int i = 0; i < count; i++) {
        fprintf(file, "%d\n", scores[i]);
    }
    fclose(file);
}

void displayLeaderboard() {
    FILE *file = fopen("leaderboard.txt", "r");
    int score, rank = 1;
    printf("\n=== Leaderboard ===\n");
    while (fscanf(file, "%d", &score) != EOF) {
        printf("%d. %d\n", rank++, score);
    }
    fclose(file);
}

int main() {
    srand(time(NULL));
    snake[0].x = WIDTH / 2;
    snake[0].y = HEIGHT / 2;
    generateFood();

    while (!game_over) {
        if (_kbhit()) {
            char key = _getch();
            if (key == 'a' && direction != 0) direction = 1;
            if (key == 'd' && direction != 1) direction = 0;
            if (key == 'w' && direction != 3) direction = 2;
            if (key == 's' && direction != 2) direction = 3;
            if (key == 'p') _getch(); // Pause feature
        }
        moveSnake();
        drawBoard();
        Sleep(speed);
    }

    saveScore();
    sortLeaderboard();
    displayLeaderboard();
    printf("\nGame Over!\n");
    return 0;
}

