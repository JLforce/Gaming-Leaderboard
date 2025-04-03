#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <string.h>

#define WIDTH 40
#define HEIGHT 20
#define MAX_PLAYERS 20
#define MAX_NAME_LENGTH 20
#define LEADERBOARD_FILE "snake_scores.dat"

typedef struct {
    int x, y;
} Point;


typedef struct {
    char name[MAX_NAME_LENGTH];
    int score;
    time_t playTime;
} PlayerScore;

Point snake[100];
int snakeLength = 1;
Point food;
int score = 0;
char direction = 'd'; 
int gameOver = 0;
char playerName[MAX_NAME_LENGTH];
PlayerScore leaderboard[MAX_PLAYERS];
int leaderboardSize = 0;

// Function prototypes 
void gotoxy(int x, int y);
void hideCursor();
void showCursor();
void setup();
void draw();
void input();
void logic();
void run();
void getPlayerName();
void saveScore();
void loadLeaderboard();
void displayLeaderboard();
void sortLeaderboard();
void displayInstructions();
void changeGameSpeed();
void clearLeaderboard();

// Keep your original function implementations
void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void hideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.bVisible = FALSE;
    cursorInfo.dwSize = 1;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void showCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.bVisible = TRUE;
    cursorInfo.dwSize = 25;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void setup() {
    system("cls");
    hideCursor();
    srand(time(0));
    snake[0].x = WIDTH / 2;
    snake[0].y = HEIGHT / 2;
    food.x = rand() % WIDTH;
    food.y = rand() % HEIGHT;
    score = 0;
    snakeLength = 1;
    direction = 'd';
    gameOver = 0;
}

void draw() {
    gotoxy(0, 0);
    for (int i = 0; i < WIDTH + 2; i++) printf("#");
    printf("\n");
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == 0) printf("#");
            if (x == snake[0].x && y == snake[0].y) {
                printf("O"); // Head
            } else if (x == food.x && y == food.y) {
                printf("F"); // Food
            } else {
                int isBody = 0;
                for (int i = 1; i < snakeLength; i++) {
                    if (snake[i].x == x && snake[i].y == y) {
                        printf("o"); // Body
                        isBody = 1;
                        break;
                    }
                }
                if (!isBody) printf(" ");
            }
            if (x == WIDTH - 1) printf("#");
        }
        printf("\n");
    }
    for (int i = 0; i < WIDTH + 2; i++) printf("#");
    printf("\nScore: %d\n", score);
}

void input() {
    if (_kbhit()) {
        char key = _getch();
        switch (key) {
            case 'w': if (direction != 's') direction = 'w'; break;
            case 's': if (direction != 'w') direction = 's'; break;
            case 'a': if (direction != 'd') direction = 'a'; break;
            case 'd': if (direction != 'a') direction = 'd'; break;
            case 'x': gameOver = 1; break; // Exit game instantly
            case 'p': // Pause game
                gotoxy(0, HEIGHT + 3);
                printf("Game Paused. Press any key to continue...");
                _getch();
                gotoxy(0, HEIGHT + 3);
                printf("                                        ");
                break;
        }
    }
}

void logic() {
    Point prev = snake[0];
    Point prev2;
    for (int i = 1; i < snakeLength; i++) {
        prev2 = snake[i];
        snake[i] = prev;
        prev = prev2;
    }
    switch (direction) {
        case 'w': snake[0].y--; break;
        case 's': snake[0].y++; break;
        case 'a': snake[0].x--; break;
        case 'd': snake[0].x++; break;
    }
    if (snake[0].x < 0 || snake[0].x >= WIDTH || snake[0].y < 0 || snake[0].y >= HEIGHT) {
        gameOver = 1;
    }
    for (int i = 1; i < snakeLength; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            gameOver = 1;
        }
    }
    if (snake[0].x == food.x && snake[0].y == food.y) {
        score += 10;
        snakeLength++;
        food.x = rand() % WIDTH;
        food.y = rand() % HEIGHT;
    }
}

void getPlayerName() {
    system("cls");
    showCursor();
    printf("Enter your name: ", MAX_NAME_LENGTH - 1);
    fgets(playerName, MAX_NAME_LENGTH, stdin);
    // Remove newline character
    playerName[strcspn(playerName, "\n")] = 0;
    hideCursor();
}

void saveScore() {
    FILE *file = fopen(LEADERBOARD_FILE, "ab");
    if (file == NULL) {
        printf("Error opening leaderboard file!\n");
        return;
    }
    
    PlayerScore newScore;
    strcpy(newScore.name, playerName);
    newScore.score = score;
    newScore.playTime = time(NULL);
    
    fwrite(&newScore, sizeof(PlayerScore), 1, file);
    fclose(file);
}

void loadLeaderboard() {
    FILE *file = fopen(LEADERBOARD_FILE, "rb");
    if (file == NULL) {
    
        leaderboardSize = 0;
        return;
    }
    
    leaderboardSize = 0;
    while (fread(&leaderboard[leaderboardSize], sizeof(PlayerScore), 1, file) && 
           leaderboardSize < MAX_PLAYERS) {
        leaderboardSize++;
    }
    
    fclose(file);
    sortLeaderboard();
}

void sortLeaderboard() {
    for (int i = 0; i < leaderboardSize - 1; i++) {
        for (int j = 0; j < leaderboardSize - i - 1; j++) {
            if (leaderboard[j].score < leaderboard[j + 1].score) {
                PlayerScore temp = leaderboard[j];
                leaderboard[j] = leaderboard[j + 1];
                leaderboard[j + 1] = temp;
            }
        }
    }
}

void displayLeaderboard() {
    system("cls");
    printf("\n\n==========================================\n");
    printf("              LEADERBOARD                 \n");
    printf("==========================================\n");
    printf("Rank  Player Name           Score    Date\n");
    printf("------------------------------------------\n");
    
    for (int i = 0; i < leaderboardSize && i < 10; i++) {
        char dateStr[20];
        struct tm *timeinfo = localtime(&leaderboard[i].playTime);
        strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", timeinfo);
        
        printf("%-5d %-20s %-8d %s\n", 
               i + 1, 
               leaderboard[i].name, 
               leaderboard[i].score,
               dateStr);
    }
    
    printf("==========================================\n");
    printf("Press any key to continue...\n");
    _getch();
}

void showMenu() {
    int choice;
    do {
        system("cls");
        printf("\n");
        printf("+==================================================+\n");
        printf("|                                                  |\n");
        printf("|              >>> SNAKE MASTERS <<<               |\n");
        printf("|                  MAIN MENU                       |\n");
        printf("|                                                  |\n");
        printf("+==================================================+\n");
        printf("|                                                  |\n");
        printf("|       [1] Play Game                              |\n");
        printf("|                                                  |\n");
        printf("|       [2] View Leaderboard                       |\n");
        printf("|                                                  |\n");
        printf("|       [3] Game Instructions                      |\n");
        printf("|                                                  |\n");
        printf("|       [4] Exit                                   |\n");
        printf("|                                                  |\n");
        printf("+==================================================+\n");
        
        // Display last game score if available
        if (score > 0) {
            printf("|  Your last score: %-5d                           |\n", score);
            printf("+==================================================+\n");
        }
        
        printf("\n    Enter your choice [1-5]: ");
        scanf("%d", &choice);
        getchar(); // Clear input buffer
        
        switch (choice) {
            case 1:
                getPlayerName();
                run();
                saveScore();
                loadLeaderboard();
                break;
                
            case 2:
                loadLeaderboard();
                displayLeaderboard();
                break;
                
            case 3:
                displayInstructions();
                break;
                
            case 4:
                system("cls");
                printf("\n\n");
                printf("+==================================================+\n");
                printf("|                                                  |\n");
                printf("|           Thanks for playing SNAKE!              |\n");
                printf("|                                                  |\n");
                printf("+==================================================+\n\n");
                printf("            Press any key to exit...");
                _getch();
                break;
                
            default:
                printf("\n    Invalid choice! Press any key to try again...");
                _getch();
        }
    } while (choice != 4);
}

void displayInstructions() {
    system("cls");
    printf("\n");
    printf("+==================================================+\n");
    printf("|                                                  |\n");
    printf("|              GAME INSTRUCTIONS                   |\n");
    printf("|                                                  |\n");
    printf("+==================================================+\n");
    printf("| Controls:                                        |\n");
    printf("| - Use W, A, S, D keys to move the snake          |\n");
    printf("| - W: Up                                          |\n");
    printf("| - S: Down                                        |\n");
    printf("| - A: Left                                        |\n");
    printf("| - D: Right                                       |\n");
    printf("|                                                  |\n");
    printf("| - P: Pause game                                  |\n");
    printf("| - X: Exit game                                   |\n");
    printf("+--------------------------------------------------+\n");
    printf("| Rules:                                           |\n");
    printf("| - Eat food (F) to grow and score points          |\n");
    printf("| - Avoid hitting walls or yourself                |\n");
    printf("| - Each food eaten scores 10 points               |\n");
    printf("| - Game ends when you hit a wall or yourself      |\n");
    printf("+==================================================+\n\n");
    printf("        Press any key to return to menu...");
    _getch();
}

// Modify the run function to use the selected game speed
void run() {
    setup();
    int delay;
    
    switch (delay) {
    	default: delay = 100;
	}

    while (!gameOver) {
        draw();
        input();
        logic();
        Sleep(delay);
    }
    
    printf("\nGame Over! Final Score: %d\n", score);
    printf("Press any key to continue...\n");
    _getch();
}

int main() {
    loadLeaderboard();
    showMenu();
    return 0;
}
