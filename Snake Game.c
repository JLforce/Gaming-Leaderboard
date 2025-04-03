#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <stdbool.h>

#define WIDTH 40
#define HEIGHT 20
#define MAX_SNAKE_LENGTH 500

// Colors
#define COLOR_BLACK 0
#define COLOR_BLUE 1
#define COLOR_GREEN 2
#define COLOR_CYAN 3
#define COLOR_RED 4
#define COLOR_MAGENTA 5
#define COLOR_YELLOW 6
#define COLOR_WHITE 7
#define COLOR_BRIGHT_GREEN 10
#define COLOR_BRIGHT_RED 12
#define COLOR_BRIGHT_YELLOW 14

typedef struct {
    int x, y;
} Point;

typedef enum {
    MENU,
    GAME,
    GAME_OVER,
    HIGH_SCORES
} GameState;

// Game variables
Point snake[MAX_SNAKE_LENGTH];
int snakeLength = 1;
Point food;
int score = 0;
char direction = 'd'; // Initial movement to the right
bool gameOver = false;
int difficulty = 1; // 1-Easy, 2-Medium, 3-Hard
int highScores[5] = {0, 0, 0, 0, 0};
GameState currentState = MENU;
int menuSelection = 0;
char playerName[20] = "Player";
bool isPaused = false;

// Function prototypes
void setColor(int foreground, int background);
void gotoxy(int x, int y);
void hideCursor();
void drawBorder();
void drawSnake();
void drawFood();
void drawScore();
void showMenu();
void showGameOver();
void showHighScores();
void setupGame();
void input();
void gameLogic();
void mainLoop();
void generateFood();
bool isPositionValid(int x, int y);
void updateHighScores();
void drawTitle();
void clearScreen();
void drawPauseMenu();

void setColor(int foreground, int background) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
                           foreground + (background * 16));
}

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

void clearScreen() {
    system("cls");
}

void drawTitle() {
    gotoxy(WIDTH / 2 - 15, 2);
    setColor(COLOR_BRIGHT_GREEN, COLOR_BLACK);
    printf(" _____             _         _____                      ");
    gotoxy(WIDTH / 2 - 15, 3);
    printf("|   __|___ ___ ___| |_ ___  |   __|___ ___ ___ ___ ___ ");
    gotoxy(WIDTH / 2 - 15, 4);
    printf("|__   |   | .'|  _| '_| -_| |  |  | .'|   | -_|   | -_|");
    gotoxy(WIDTH / 2 - 15, 5);
    printf("|_____|_|_|__,|___|_,_|___| |_____|__,|_|_|___|_|_|___|");
    setColor(COLOR_WHITE, COLOR_BLACK);
}

void drawBorder() {
    setColor(COLOR_YELLOW, COLOR_BLACK);
    
    // Draw horizontal borders with corners
    gotoxy(0, 0);
    printf("╔");
    for (int i = 0; i < WIDTH; i++) printf("═");
    printf("╗");
    
    gotoxy(0, HEIGHT + 1);
    printf("╚");
    for (int i = 0; i < WIDTH; i++) printf("═");
    printf("╝");
    
    // Draw vertical borders
    for (int i = 1; i <= HEIGHT; i++) {
        gotoxy(0, i);
        printf("║");
        gotoxy(WIDTH + 1, i);
        printf("║");
    }
    
    setColor(COLOR_WHITE, COLOR_BLACK);
}

void drawSnake() {
    // Draw head
    gotoxy(snake[0].x + 1, snake[0].y + 1);  // +1 for border offset
    setColor(COLOR_BRIGHT_GREEN, COLOR_BLACK);
    
    // Different head shapes based on direction
    switch(direction) {
        case 'w': printf("▲"); break;
        case 's': printf("▼"); break;
        case 'a': printf("◄"); break;
        case 'd': printf("►"); break;
    }
    
    // Draw body
    setColor(COLOR_GREEN, COLOR_BLACK);
    for (int i = 1; i < snakeLength; i++) {
        gotoxy(snake[i].x + 1, snake[i].y + 1);  // +1 for border offset
        printf("■");
    }
    
    setColor(COLOR_WHITE, COLOR_BLACK);
}

void drawFood() {
    gotoxy(food.x + 1, food.y + 1);  // +1 for border offset
    setColor(COLOR_BRIGHT_RED, COLOR_BLACK);
    printf("♦");
    setColor(COLOR_WHITE, COLOR_BLACK);
}

void drawScore() {
    gotoxy(WIDTH + 5, 2);
    setColor(COLOR_BRIGHT_YELLOW, COLOR_BLACK);
    printf("┌─────────────────┐");
    gotoxy(WIDTH + 5, 3);
    printf("│ Score: %-8d │", score);
    gotoxy(WIDTH + 5, 4);
    printf("│ Length: %-7d │", snakeLength);
    gotoxy(WIDTH + 5, 5);
    printf("└─────────────────┘");
    
    // Controls reminder
    gotoxy(WIDTH + 5, 7);
    setColor(COLOR_CYAN, COLOR_BLACK);
    printf("┌─────────────────┐");
    gotoxy(WIDTH + 5, 8);
    printf("│ Controls:       │");
    gotoxy(WIDTH + 5, 9);
    printf("│ W - Up          │");
    gotoxy(WIDTH + 5, 10);
    printf("│ S - Down        │");
    gotoxy(WIDTH + 5, 11);
    printf("│ A - Left        │");
    gotoxy(WIDTH + 5, 12);
    printf("│ D - Right       │");
    gotoxy(WIDTH + 5, 13);
    printf("│ P - Pause       │");
    gotoxy(WIDTH + 5, 14);
    printf("│ X - Exit        │");
    gotoxy(WIDTH + 5, 15);
    printf("└─────────────────┘");
    
    // Difficulty level
    gotoxy(WIDTH + 5, 17);
    setColor(COLOR_MAGENTA, COLOR_BLACK);
    printf("┌─────────────────┐");
    gotoxy(WIDTH + 5, 18);
    printf("│ Difficulty:     │");
    gotoxy(WIDTH + 5, 19);
    switch(difficulty) {
        case 1: printf("│ ■□□ Easy       │"); break;
        case 2: printf("│ ■■□ Medium     │"); break;
        case 3: printf("│ ■■■ Hard       │"); break;
    }
    gotoxy(WIDTH + 5, 20);
    printf("└─────────────────┘");
    
    setColor(COLOR_WHITE, COLOR_BLACK);
}

void showMenu() {
    clearScreen();
    drawTitle();
    
    int menuY = 10;
    gotoxy(WIDTH / 2 - 8, menuY);
    setColor(COLOR_CYAN, COLOR_BLACK);
    printf("╔════════════════╗");
    gotoxy(WIDTH / 2 - 8, menuY + 1);
    printf("║  SNAKE  GAME   ║");
    gotoxy(WIDTH / 2 - 8, menuY + 2);
    printf("╚════════════════╝");
    
    gotoxy(WIDTH / 2 - 8, menuY + 4);
    setColor(menuSelection == 0 ? COLOR_BRIGHT_YELLOW : COLOR_WHITE, COLOR_BLACK);
    printf("  ▶ Start Game");
    
    gotoxy(WIDTH / 2 - 8, menuY + 6);
    setColor(menuSelection == 1 ? COLOR_BRIGHT_YELLOW : COLOR_WHITE, COLOR_BLACK);
    printf("  ▶ Difficulty: ");
    
    switch(difficulty) {
        case 1: printf("Easy"); break;
        case 2: printf("Medium"); break;
        case 3: printf("Hard"); break;
    }
    
    gotoxy(WIDTH / 2 - 8, menuY + 8);
    setColor(menuSelection == 2 ? COLOR_BRIGHT_YELLOW : COLOR_WHITE, COLOR_BLACK);
    printf("  ▶ High Scores");
    
    gotoxy(WIDTH / 2 - 8, menuY + 10);
    setColor(menuSelection == 3 ? COLOR_BRIGHT_YELLOW : COLOR_WHITE, COLOR_BLACK);
    printf("  ▶ Exit");
    
    setColor(COLOR_WHITE, COLOR_BLACK);
    
    gotoxy(WIDTH / 2 - 15, HEIGHT - 2);
    setColor(COLOR_CYAN, COLOR_BLACK);
    printf("Press Enter to select, ↑↓ to navigate");
    setColor(COLOR_WHITE, COLOR_BLACK);
}

void drawPauseMenu() {
    int menuY = HEIGHT / 2 - 3;
    int menuX = WIDTH / 2 - 8;
    
    setColor(COLOR_YELLOW, COLOR_BLACK);
    gotoxy(menuX, menuY);
    printf("╔════════════════╗");
    for (int i = 1; i <= 5; i++) {
        gotoxy(menuX, menuY + i);
        printf("║                ║");
    }
    gotoxy(menuX, menuY + 6);
    printf("╚════════════════╝");
    
    gotoxy(menuX + 4, menuY + 2);
    setColor(COLOR_BRIGHT_YELLOW, COLOR_BLACK);
    printf("GAME PAUSED");
    
    gotoxy(menuX + 1, menuY + 4);
    setColor(COLOR_WHITE, COLOR_BLACK);
    printf("Press P to resume");
    
    setColor(COLOR_WHITE, COLOR_BLACK);
}

void showGameOver() {
    clearScreen();
    
    int centerY = HEIGHT / 2 - 5;
    gotoxy(WIDTH / 2 - 20, centerY);
    setColor(COLOR_BRIGHT_RED, COLOR_BLACK);
    printf("  ▄████  ▄▄▄       ███▄ ▄███▓▓█████     ▒█████   ██▒   █▓▓█████  ██▀███  ");
    gotoxy(WIDTH / 2 - 20, centerY + 1);
    printf(" ██▒ ▀█▒▒████▄    ▓██▒▀█▀ ██▒▓█   ▀    ▒██▒  ██▒▓██░   █▒▓█   ▀ ▓██ ▒ ██▒");
    gotoxy(WIDTH / 2 - 20, centerY + 2);
    printf("▒██░▄▄▄░▒██  ▀█▄  ▓██    ▓██░▒███      ▒██░  ██▒ ▓██  █▒░▒███   ▓██ ░▄█ ▒");
    gotoxy(WIDTH / 2 - 20, centerY + 3);
    printf("░▓█  ██▓░██▄▄▄▄██ ▒██    ▒██ ▒▓█  ▄    ▒██   ██░  ▒██ █░░▒▓█  ▄ ▒██▀▀█▄  ");
    gotoxy(WIDTH / 2 - 20, centerY + 4);
    printf("░▒▓███▀▒ ▓█   ▓██▒▒██▒   ░██▒░▒████▒   ░ ████▓▒░   ▒▀█░  ░▒████▒░██▓ ▒██▒");
    gotoxy(WIDTH / 2 - 20, centerY + 5);
    printf(" ░▒   ▒  ▒▒   ▓▒█░░ ▒░   ░  ░░░ ▒░ ░   ░ ▒░▒░▒░    ░ ▐░  ░░ ▒░ ░░ ▒▓ ░▒▓░");
    
    gotoxy(WIDTH / 2 - 10, centerY + 8);
    setColor(COLOR_BRIGHT_YELLOW, COLOR_BLACK);
    printf("Final Score: %d", score);
    
    gotoxy(WIDTH / 2 - 10, centerY + 10);
    setColor(COLOR_WHITE, COLOR_BLACK);
    printf("Player: %s", playerName);
    
    gotoxy(WIDTH / 2 - 15, centerY + 13);
    setColor(COLOR_CYAN, COLOR_BLACK);
    printf("Press ENTER to return to menu");
    setColor(COLOR_WHITE, COLOR_BLACK);
    
    updateHighScores();
    
    // Wait for Enter key
    while (_getch() != 13) {}
    
    currentState = MENU;
}

void showHighScores() {
    clearScreen();
    
    gotoxy(WIDTH / 2 - 10, 5);
    setColor(COLOR_BRIGHT_YELLOW, COLOR_BLACK);
    printf("╔══════════════════╗");
    gotoxy(WIDTH / 2 - 10, 6);
    printf("║   HIGH SCORES    ║");
    gotoxy(WIDTH / 2 - 10, 7);
    printf("╚══════════════════╝");
    
    for (int i = 0; i < 5; i++) {
        gotoxy(WIDTH / 2 - 8, 10 + i * 2);
        if (highScores[i] > 0) {
            setColor(i == 0 ? COLOR_BRIGHT_YELLOW : COLOR_WHITE, COLOR_BLACK);
            printf("%d. %s%s: %d", i + 1, (i == 0) ? "👑 " : "", playerName, highScores[i]);
        } else {
            setColor(COLOR_WHITE, COLOR_BLACK);
            printf("%d. ---", i + 1);
        }
    }
    
    gotoxy(WIDTH / 2 - 15, HEIGHT - 5);
    setColor(COLOR_CYAN, COLOR_BLACK);
    printf("Press BACKSPACE to return to menu");
    setColor(COLOR_WHITE, COLOR_BLACK);
}

bool isPositionValid(int x, int y) {
    // Check if position is not occupied by snake body
    for (int i = 0; i < snakeLength; i++) {
        if (snake[i].x == x && snake[i].y == y) {
            return false;
        }
    }
    return true;
}

void generateFood() {
    do {
        food.x = rand() % WIDTH;
        food.y = rand() % HEIGHT;
    } while (!isPositionValid(food.x, food.y));
}

void setupGame() {
    clearScreen();
    snakeLength = 1;
    score = 0;
    direction = 'd';
    gameOver = false;
    
    // Initialize snake position in the middle
    snake[0].x = WIDTH / 2;
    snake[0].y = HEIGHT / 2;
    
    generateFood();
    
    // Draw initial game screen
    drawBorder();
    drawSnake();
    drawFood();
    drawScore();
}

void input() {
    if (_kbhit()) {
        char key = _getch();
        
        // Check for arrow keys which produce two chars
        if (key == -32 || key == 0) {
            key = _getch();
            switch (key) {
                case 72: key = 'w'; break; // Up arrow
                case 80: key = 's'; break; // Down arrow
                case 75: key = 'a'; break; // Left arrow
                case 77: key = 'd'; break; // Right arrow
            }
        }
        
        // Convert uppercase to lowercase
        if (key >= 'A' && key <= 'Z') {
            key = key + 32;  // Convert to lowercase
        }
        
        switch (key) {
            case 'w': if (direction != 's') direction = 'w'; break;
            case 's': if (direction != 'w') direction = 's'; break;
            case 'a': if (direction != 'd') direction = 'a'; break;
            case 'd': if (direction != 'a') direction = 'd'; break;
            case 'x': gameOver = true; break;     // Exit game
            case 'p': isPaused = !isPaused; break; // Toggle pause
            
            // Menu navigation
            case 13:  // Enter key
                if (currentState == MENU) {
                    switch (menuSelection) {
                        case 0: // Start Game
                            currentState = GAME;
                            setupGame();
                            break;
                        case 1: // Difficulty - cycle through
                            difficulty = (difficulty % 3) + 1;
                            break;
                        case 2: // High Scores
                            currentState = HIGH_SCORES;
                            break;
                        case 3: // Exit
                            exit(0);
                            break;
                    }
                }
                break;
                
            case 8:   // Backspace key - return to menu
                if (currentState == HIGH_SCORES) {
                    currentState = MENU;
                }
                break;
                
            case 72: case 'k': // Up arrow or 'k'
                if (currentState == MENU) {
                    menuSelection = (menuSelection + 3) % 4; // Wrap around up
                }
                break;
                
            case 80: case 'j': // Down arrow or 'j'
                if (currentState == MENU) {
                    menuSelection = (menuSelection + 1) % 4; // Wrap around down
                }
                break;
        }
    }
}

void gameLogic() {
    if (isPaused || currentState != GAME) return;
    
    // Save previous position for body segments
    Point prev = snake[0];
    Point prev2;
    
    // Update head position based on direction
    switch (direction) {
        case 'w': snake[0].y--; break;
        case 's': snake[0].y++; break;
        case 'a': snake[0].x--; break;
        case 'd': snake[0].x++; break;
    }
    
    // Update body segments
    for (int i = 1; i < snakeLength; i++) {
        prev2 = snake[i];
        snake[i] = prev;
        prev = prev2;
    }
    
    // Collision with border
    if (snake[0].x < 0 || snake[0].x >= WIDTH || snake[0].y < 0 || snake[0].y >= HEIGHT) {
        gameOver = true;
    }
    
    // Collision with self
    for (int i = 1; i < snakeLength; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            gameOver = true;
        }
    }
    
    // Food collision
    if (snake[0].x == food.x && snake[0].y == food.y) {
        score += (difficulty * 10); // Score based on difficulty
        snakeLength++;
        
        // Play eating sound (Windows beep)
        Beep(1000, 50);
        
        // Generate new food
        generateFood();
    }
    
    // Check game over
    if (gameOver) {
        currentState = GAME_OVER;
    }
}

void updateHighScores() {
    // Simple insertion sort
    if (score > 0) {
        for (int i = 0; i < 5; i++) {
            if (score > highScores[i]) {
                // Shift all lower scores down
                for (int j = 4; j > i; j--) {
                    highScores[j] = highScores[j-1];
                }
                highScores[i] = score;
                break;
            }
        }
    }
}

void mainLoop() {
    hideCursor();
    srand(time(0));
    
    while (1) {
        switch (currentState) {
            case MENU:
                showMenu();
                break;
                
            case GAME:
                // Draw game elements
                clearScreen();
                drawBorder();
                drawSnake();
                drawFood();
                drawScore();
                
                if (isPaused) {
                    drawPauseMenu();
                }
                break;
                
            case GAME_OVER:
                showGameOver();
                break;
                
            case HIGH_SCORES:
                showHighScores();
                break;
        }
        
        input();
        gameLogic();
        
        // Different speeds based on difficulty
        int delayTime;
        switch (difficulty) {
            case 1: delayTime = 150; break; // Easy - Slower
            case 2: delayTime = 100; break; // Medium
            case 3: delayTime = 70; break;  // Hard - Faster
        }
        
        Sleep(delayTime);
    }
}

int main() {
    // Set console title and size
    SetConsoleTitle("Enhanced Snake Game");
    
    // Set console size
    COORD bufferSize = {WIDTH + 25, HEIGHT + 10};
    SMALL_RECT windowSize = {0, 0, WIDTH + 24, HEIGHT + 9};
    
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), bufferSize);
    SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &windowSize);
    
    mainLoop();
    return 0;
}
