#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
void addPlayer();
void updatePlayer();
void deletePlayer();
void saveLeaderboard();
void loadLeaderboard();
void showLeaderboard();
void sortLeaderboard();
Player* searchPlayer(int id);

void initializePlayers() {
    players = (Player *)malloc(maxPlayers * sizeof(Player));
    if (!players) {
        exit(EXIT_FAILURE);
    }
}

Player* searchPlayer(int id) {
    for (int i = 0; i < playerCount; i++) {
        if (players[i].id == id) {
            return &players[i];
        }
    }
    return NULL;
}

void addPlayer() {
    int id, score;
    char name[50];
    printf("\nEnter Player ID: ");
    scanf("%d", &id);
    if (searchPlayer(id)) {
        printf("Player ID already exists! Use update option.\n");
        return;
    }
    printf("Enter Player Name: ");
    scanf("%49s", name);
    printf("Enter Player Score: ");
    scanf("%d", &score);

    if (playerCount >= maxPlayers) {
        printf("Maximum player limit reached!\n");
        return;
    }
    players[playerCount].id = id;
    strncpy(players[playerCount].name, name, sizeof(players[playerCount].name) - 1);
    players[playerCount].name[sizeof(players[playerCount].name) - 1] = '\0';
    players[playerCount].score = score;
    playerCount++;
    sortLeaderboard();
    printf("\nPlayer added successfully!\n");
}

void updatePlayer() {
    int id, newScore;
    printf("\nEnter Player ID: ");
    scanf("%d", &id);
    Player *existingPlayer = searchPlayer(id);
    if (!existingPlayer) {
        printf("Player not found!\n");
        return;
    }
    printf("Enter New Score: ");
    scanf("%d", &newScore);
    existingPlayer->score = newScore;
    printf("\nPlayer score updated successfully!\n");
    sortLeaderboard();
}

void deletePlayer() {
    int id;
    printf("\nEnter Player ID to delete: ");
    scanf("%d", &id);
    int index = -1;
    for (int i = 0; i < playerCount; i++) {
        if (players[i].id == id) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        printf("Player not found!\n");
        return;
    }
    for (int i = index; i < playerCount - 1; i++) {
        players[i] = players[i + 1];
    }
    playerCount--;
    printf("\nPlayer deleted successfully!\n");
}

void sortLeaderboard() {
    for (int i = 0; i < playerCount - 1; i++) {
        for (int j = i + 1; j < playerCount; j++) {
            if (players[j].score > players[i].score) {
                Player temp = players[i];
                players[i] = players[j];
                players[j] = temp;
            }
        }
    }
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
    playerCount = 0;
    while (fscanf(file, "%d %49s %d", &players[playerCount].id, players[playerCount].name, &players[playerCount].score) != EOF) {
        playerCount++;
    }
    fclose(file);
    sortLeaderboard();
    printf("\nLeaderboard loaded successfully!\n");
    showLeaderboard();
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
        printf("2. Update Player Score\n");
        printf("3. Delete Player\n");
        printf("4. Show Leaderboard\n");
        printf("5. Save Leaderboard\n");
        printf("6. Load Leaderboard\n");
        printf("7. Exit\n");
        printf("\nEnter your choice: ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                addPlayer();
                break;
            case 2:
                updatePlayer();
                break;
            case 3:
            	deletePlayer();
            	break;
            case 4:
                showLeaderboard();
                break;
            case 5:
                saveLeaderboard();
                break;
            case 6:
                loadLeaderboard();
                break;
            case 7:
                printf("ByeBye!\n");
                free(players);
                break;
            default:
                printf("Invalid choice! Try again.\n");
        }
    } while (choice != 7);
    
    return 0;
}
