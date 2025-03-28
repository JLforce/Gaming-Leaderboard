#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h> 
#include <windows.h>

#define maxPlayers 100

typedef struct {
    int id;
    char name[50];
    int score;
} Player;

Player *players;
int playerCount = 0;

typedef struct HistoryNode {
    Player player;
    struct HistoryNode *next;
} HistoryNode;

HistoryNode *historyHead = NULL;

void initializePlayers();
void addPlayer(int id, const char *name, int score);
void saveLeaderboard();
void loadLeaderboard();
void showLeaderboard();

void initializePlayers() {
    players = (Player *)malloc(maxPlayers * sizeof(Player));
    if (!players) {
        exit(EXIT_FAILURE);
    }
}

void addPlayer(int id, const char *name, int score) {
    if (playerCount >= maxPlayers) {
        printf("Maximum player limit reached!\n");
        return;
    }
    players[playerCount].id = id;
    strncpy(players[playerCount].name, name, sizeof(players[playerCount].name) - 1);
    players[playerCount].name[sizeof(players[playerCount].name) - 1] = '\0';
    players[playerCount].score = score;
    playerCount++;
    printf("\nPlayer added successfully!\n");
}

void saveLeaderboard() {
    FILE *file = fopen("leaderboard.txt", "w");
    if (!file) {
        printf("\nError saving leaderboard!\n");
        return;
    }
    for (int i = 0; i < playerCount; i++) {
        fprintf(file, "%d %s %d\n", players[i].id, players[i].name, players[i].score);
    }
    fclose(file);
    printf("\nLeaderboard saved successfully!\n");
}

void loadLeaderboard() {
    FILE *file = fopen("leaderboard.txt", "r");
    if (!file) {
        printf("No saved leaderboard found.\n");
        return;
    }

    playerCount = 0; // Reset player count before loading new data
    while (fscanf(file, "%d %49s %d", &players[playerCount].id, players[playerCount].name, &players[playerCount].score) != EOF) {
        playerCount++;
    }
    fclose(file);
    
    printf("\nLeaderboard loaded successfully!\n");
    showLeaderboard(); // Call this to display loaded players immediately
}
void showLeaderboard() {
    system("cls"); 
    printf("\n=== Leaderboard ===\n");
    if (playerCount == 0) {
        printf("\nNo players in the leaderboard.\n");
    } else {
        for (int i = 0; i < playerCount; i++) {
            printf("%d. %s - %d points\n", i + 1, players[i].name, players[i].score);
        }
    }
    printf("===================\n");
}

int main() {
    initializePlayers();
    loadLeaderboard();
    
    int choice;
    
    do {
        printf("\n=== Leaderboard Menu ===\n");
        printf("1. Add Player\n");
        printf("2. Show Leaderboard\n");
        printf("3. Save Leaderboard\n");
        printf("4. Load Leaderboard\n");
        printf("5. Exit\n");
        printf("\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                int id, score;
                char name[50];
                printf("\nEnter Player ID: ");
                scanf("%d", &id);
                printf("Enter Player Name: ");
                scanf("%s", name);
                printf("Enter Player Score: ");
                scanf("%d", &score);
                addPlayer(id, name, score);
                break;
            }
            case 2:
                showLeaderboard();
                break;
            case 3:
                saveLeaderboard();
                break;
            case 4:
                loadLeaderboard();
                break;
            case 5:
                printf("ByeBye!");
                break;
            default:
                printf("Invalid choice! Try again.\n");
        }
        
    } while (choice != 5);
    
    return 0;
}
