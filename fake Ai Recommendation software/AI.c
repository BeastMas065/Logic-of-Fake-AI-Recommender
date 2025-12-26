#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define MOVIE_FILE "movies.txt"
#define USERS_FILE "users.txt"

void toLowerCase(char *str);
int loadUser(char username[], int *a, int *r, int *c, int *f, int *s);
void saveOrUpdateUser(char username[], int a, int r, int c, int f, int s);

void takeTest(int *a, int *r, int *c, int *f, int *s);
void recommendMovies(int a, int r, int c, int f, int s);

int main() 
{
    char username[30];
    int action = 0, romance = 0, comedy = 0, fast = 0, slow = 0;
    int found, choice;

    printf("Enter your name: ");
    scanf("%s", username);
    toLowerCase(username);

    found = loadUser(username, &action, &romance, &comedy, &fast, &slow);

    if (found) 
    {
        printf("Welcome back, %s!\n", username);
        printf("Do you want to retake the preference quiz?\n");
        printf("1. Yes\n2. No\nChoice: ");
        scanf("%d", &choice);

        if (choice == 1) 
        {
            takeTest(&action, &romance, &comedy, &fast, &slow);
            saveOrUpdateUser(username, action, romance, comedy, fast, slow);
        }
    } 
    else 
    {
        printf("New user detected. Let's understand your taste!\n");
        takeTest(&action, &romance, &comedy, &fast, &slow);
        saveOrUpdateUser(username, action, romance, comedy, fast, slow);
    }

    recommendMovies(action, romance, comedy, fast, slow);
    printf("\nPress Enter to exit...");
    scanf(" ");

    return 0;
}

/* ---------------- UTILITY FUNCTIONS ---------------- */

void toLowerCase(char *str) 
{
    for (int i = 0; str[i]; i++)
        str[i] = tolower(str[i]);
}

/* ---------------- USER FILE HANDLING ---------------- */

int loadUser(char username[], int *a, int *r, int *c, int *f, int *s) 
{
    FILE *fp;
    char name[30];

    fp = fopen(USERS_FILE, "r");
    if (fp == NULL)
        return 0;

    while (fscanf(fp, "%s %d %d %d %d %d", name, a, r, c, f, s) != EOF) 
    {
        if (strcmp(name, username) == 0) 
        {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

void saveOrUpdateUser(char username[], int a, int r, int c, int f, int s) 
{
    FILE *fp, *temp;
    char name[30];
    int A, R, C, F, S;
    int updated = 0;

    fp = fopen(USERS_FILE, "r");
    temp = fopen("temp.txt", "w");

    if (fp != NULL) {
        while (fscanf(fp, "%s %d %d %d %d %d", name, &A, &R, &C, &F, &S) != EOF) 
        {
            if (strcmp(name, username) == 0) 
            {
                fprintf(temp, "%s %d %d %d %d %d\n", username, a, r, c, f, s);
                updated = 1;
            } else 
            {
                fprintf(temp, "%s %d %d %d %d %d\n", name, A, R, C, F, S);
            }
        }
        fclose(fp);
    }

    if (!updated) 
    {
        fprintf(temp, "%s %d %d %d %d %d\n", username, a, r, c, f, s);
    }

    fclose(temp);
    remove(USERS_FILE);
    rename("temp.txt", USERS_FILE);
}

/* ---------------- QUIZ ---------------- */

void takeTest(int *action, int *romance, int *comedy, int *fast, int *slow) 
{
    int choice;

    *action = *romance = *comedy = *fast = *slow = 0;

    printf("\nQ1. Would you rather watch:\n");
    printf("1. A high-speed car chase\n");
    printf("2. A deep emotional love story\n");
    scanf("%d", &choice);
    if (choice == 1) { *action += 2; (*fast)++; }
    else { *romance += 2; (*slow)++; }

    printf("\nQ2. Would you rather watch:\n");
    printf("1. Something that makes you laugh\n");
    printf("2. Something intense and thrilling\n");
    scanf("%d", &choice);
    if (choice == 1) *comedy += 2;
    else *action += 2;

    printf("\nQ3. Would you rather watch:\n");
    printf("1. A short, fun movie\n");
    printf("2. A long, emotional movie\n");
    scanf("%d", &choice);
    if (choice == 1) { (*comedy)++; (*fast)++; }
    else { (*romance)++; (*slow)++; }
}

/* ---------------- RECOMMENDER ---------------- */

void recommendMovies(int action, int romance, int comedy, int fast, int slow) 
{
    FILE *fp;

    char name[30], genre[20], pace[20];

    char candidates[10][30];
    char candidateGenres[10][20];
    char candidatePaces[10][20];

    int candidateCount = 0;
    int score, bestScore = -1;

    fp = fopen(MOVIE_FILE, "r");
    if (fp == NULL) 
    {
        printf("Movie file not found.\n");
        return;
    }

    while (fscanf(fp, "%s %s %s", name, genre, pace) != EOF) 
    {
        score = 0;

        if (strcmp(genre, "action") == 0) score += action;
        if (strcmp(genre, "romance") == 0) score += romance;
        if (strcmp(genre, "comedy") == 0) score += comedy;

        if (strcmp(pace, "fast") == 0) score += fast;
        if (strcmp(pace, "slow") == 0) score += slow;

        if (score > bestScore) 
        {
            bestScore = score;
            candidateCount = 0;

            strcpy(candidates[candidateCount], name);
            strcpy(candidateGenres[candidateCount], genre);
            strcpy(candidatePaces[candidateCount], pace);
            candidateCount++;
        }
        else if (score == bestScore || score == bestScore - 1) 
        {
            strcpy(candidates[candidateCount], name);
            strcpy(candidateGenres[candidateCount], genre);
            strcpy(candidatePaces[candidateCount], pace);
            candidateCount++;
        }
    }

    fclose(fp);

    if (candidateCount == 0) 
    {
        printf("No suitable movies found.\n");
        return;
    }

    srand(time(NULL));
    int pick = rand() % candidateCount;

    printf("\n🎬 Recommended Movie for you: %s\n", candidates[pick]);

    /* Explain why */
    explainRecommendation(candidateGenres[pick], candidatePaces[pick], action, romance, comedy, fast, slow);
}


void explainRecommendation(char genre[], char pace[], int action, int romance, int comedy, int fast, int slow) 
{
    int reasonsShown = 0;

    printf("\nWhy this movie?\n");

    /* Genre-based explanation */
    if (strcmp(genre, "action") == 0 && action > 0 && reasonsShown < 2) 
    {
        printf("- You prefer action-packed movies\n");
        reasonsShown++;
    }
    else if (strcmp(genre, "romance") == 0 && romance > 0 && reasonsShown < 2) 
    {
        printf("- You enjoy emotional and romantic stories\n");
        reasonsShown++;
    }
    else if (strcmp(genre, "comedy") == 0 && comedy > 0 && reasonsShown < 2) 
    {
        printf("- You like movies that make you laugh\n");
        reasonsShown++;
    }

    /* Pace-based explanation */
    if (strcmp(pace, "fast") == 0 && fast >= slow && reasonsShown < 2) 
    {
        printf("- You enjoy fast-paced storytelling\n");
        reasonsShown++;
    }
    else if (strcmp(pace, "slow") == 0 && slow > fast && reasonsShown < 2) 
    {
        printf("- You prefer slow and emotional movies\n");
        reasonsShown++;
    }
}

