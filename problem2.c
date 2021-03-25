#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <argz.h>
#include <pthread.h>

#define TEAM_SIZE 4
#define TOTAL_PACKAGES 50

int packageID = 0;
sem_t mutex, blue, red, green, yellow, orange;
int currPackage = 0;


//A package is a struct containing it's ID and an array of it's instructions
struct Packages {
    int package_id;
    int instructions[4];
};

struct Worker {
    char team[50];
    int id;
    pthread_t workerThread;
    struct Packages package;
};

struct Worker redTeamWorkers[TEAM_SIZE];
struct Worker blueTeamWorkers[TEAM_SIZE];
struct Worker greenTeamWorkers[TEAM_SIZE];
struct Worker yellowTeamWorkers[TEAM_SIZE];

void *redWaitingZone(void *arg);

void *blueWaitingZone(void *arg);

void *greenWaitingZone(void *arg);

void *yellowWaitingZone(void *arg);

int currRedWorker = 0;
int currBlueWorker = 0;
int currGreenWorker = 0;
int currYellowWorker = 0;


struct threadArg {
    char team[50];
    int id;
};

//The Pile of Pending Packages (PPP) is an array of Packages, which are of type Package
struct Packages PPP[TOTAL_PACKAGES];

void createPackage() {
    struct Packages currentPackage = {packageID, {0, 0, 0, 0}};
    int randNumInstructions = (rand() % 4) + 1;

    int orderedInstructions[4];

    // Initialzie array of 1-4 inclusive and take from there to ensure uniqueness
    for (int i = 0; i < 4; i++) {
        orderedInstructions[i] = i + 1;
    }

    //printf("Ordered instructions: %d, %d, %d, %d\n", orderedInstructions[0], orderedInstructions[1], orderedInstructions[2], orderedInstructions[3]);

    // Take values from ordered instructions, zeroing them as you go to avoid duplicates
    for (int i = 0; i < randNumInstructions; i++) {
        // Pick from 0-3 inclusive, index of ordered instructions array
        int randInstruction = (rand() % 4);
        while (orderedInstructions[randInstruction] == 0) {
            randInstruction = (rand() % 4);
        }
        currentPackage.instructions[i] = orderedInstructions[randInstruction];
        orderedInstructions[randInstruction] = 0;
    }
    PPP[packageID] = currentPackage;
    packageID++;
    printf("Package created with package ID %d with instructions %d, %d, %d, %d\n", currentPackage.package_id,
           currentPackage.instructions[0], currentPackage.instructions[1], currentPackage.instructions[2],
           currentPackage.instructions[3]);

}

//1
void barcode(char team[50], int id, int pid) {
    //sem_post(&mutex);
    printf("%s %d is now using the barcode station on package %d\n", team, id, pid);
    sleep(2);
}

//2
void xray(char team[50], int id, int pid) {
    //sem_post(&mutex);
    printf("%s %d is now using the xray station on package %d\n", team, id, pid);
    sleep(2);
}

//3
void shaker(char team[50], int id, int pid) {
    //sem_post(&mutex);
    printf("%s %d is now using the shaker station on package %d\n", team, id, pid);
    sleep(2);
}

//4
void weigh(char team[50], int id, int pid) {
    //sem_post(&mutex);
    printf("%s %d is now using the weigh station on package %d\n", team, id, pid);
    sleep(2);
}


void conveyor(char teamName[50], int tid) {
    //struct threadArg *args = (struct threadArg *)arg;
    char team[50];
    int id = tid;
    strcpy(team, teamName);
    struct Worker currWorker;


    int instructionCounter = 0;

    if (strcmp(team, "red") == 0) {
        currWorker = redTeamWorkers[id];
    } else if (strcmp(team, "blue") == 0) {
        currWorker = blueTeamWorkers[id];
    } else if (strcmp(team, "green") == 0) {
        currWorker = greenTeamWorkers[id];
    } else if (strcmp(team, "yellow") == 0) {
        currWorker = yellowTeamWorkers[id];
    }
//    printf("Global worker package instruction 0 is: %d\n", currWorker.package.instructions[0]);
//    printf("First package instruction 0: %d\n", PPP[0].instructions[0]);
//    printf("Conveyor function reached, team and id: %s, %d, package ID is: %d\n", team, id, currWorker.package.package_id);


    int instruction = currWorker.package.instructions[instructionCounter++];

    while (instruction != 0 && instructionCounter <= 4) {
//        printf("New instruction: %d, instruction counter: %d\n", instruction, instructionCounter);
        switch (instruction) {
            case 1:
                barcode(team, id, currWorker.package.package_id);
                break;
            case 2:
                xray(team, id, currWorker.package.package_id);
                break;
            case 3:
                shaker(team, id, currWorker.package.package_id);
                break;
            case 4:
                weigh(team, id, currWorker.package.package_id);
                break;
            default:
                printf("you really messed up\n");
        }
        instruction = currWorker.package.instructions[instructionCounter++];
    }


    // If instruction = 0; end of instructions
    // TODO: make print statement wait for last function to sleep
    printf("%s (ID: %d) has finished with package %d, it has now been shipped!\n", team, id,
           currWorker.package.package_id);

    if (strcmp(team, "red") == 0) {
        sem_post(&red);
    } else if (strcmp(team, "blue") == 0) {
        sem_post(&blue);
    } else if (strcmp(team, "green") == 0) {
        sem_post(&green);
    } else if (strcmp(team, "yellow") == 0) {
        sem_post(&orange);
    }
}

void *waitForPackage(char teamName[50], int tid) {

    // If there are no packages left, just die
    if (currPackage >= TOTAL_PACKAGES) {
        if (strcmp(teamName, "red") == 0) {
            sem_post(&red);
            return 0;
        } else if (strcmp(teamName, "blue") == 0) {
            sem_post(&blue);
            return 0;
        } else if (strcmp(teamName, "green") == 0) {
            sem_post(&green);
            return 0;
        } else if (strcmp(teamName, "yellow") == 0) {
            sem_post(&orange);
            return 0;
        }
    }

    // Everyone waits on the same semaphore to try and get a package, no other restrictions
//    printf("Grabbing mutex: %s %d\n", teamName, tid);
    sem_wait(&mutex);
    char team[50];
    int id = tid;
    strcpy(team, teamName);


    // Has the mutex code
    // Manipulate package and worker variables
    if (strcmp(team, "red") == 0) {
        // "Pick up" the next package
        redTeamWorkers[id].package = PPP[currPackage++];
    } else if (strcmp(team, "blue") == 0) {
        // "Pick up" the next package
        blueTeamWorkers[id].package = PPP[currPackage++];
    } else if (strcmp(team, "green") == 0) {
        // "Pick up" the next package
        greenTeamWorkers[id].package = PPP[currPackage++];
    } else if (strcmp(team, "yellow") == 0) {
        // "Pick up" the next package
        yellowTeamWorkers[id].package = PPP[currPackage++];
    }

    // Release mutex
    sem_post(&mutex);
//    printf("left mutex: %s %d\n", teamName, tid);

    // Do the instructions for the package
    conveyor(team, id);

    // Repeat the process if there are still packages left

    // This allows the current thread to take a lil' break so the next thread in queue for the semaphore can grab the semaphore for their team
    /////////////////
    sleep(1);
    /////////////////
    // By: Colby Frechette Feb 19, 2021

    struct threadArg *currThreadArg;
    currThreadArg = (struct threadArg *) malloc(sizeof(struct threadArg));
    strcpy(currThreadArg->team, team);
    currThreadArg->id = id;
//    printf("%s %d is going back in line! values in struct: %s %d\n", team, id, currThreadArg->team, currThreadArg->id);
    if (strcmp(team, "red") == 0) {
        redWaitingZone((void *) currThreadArg);
    } else if (strcmp(team, "blue") == 0) {
        blueWaitingZone((void *) currThreadArg);
    } else if (strcmp(team, "green") == 0) {
        greenWaitingZone((void *) currThreadArg);
    } else if (strcmp(team, "yellow") == 0) {
        yellowWaitingZone((void *) currThreadArg);
    }
}

void *blueWaitingZone(void *arg) {
    struct threadArg *args = arg;

    // Stall while it's not your turn
    while (currBlueWorker != args->id);

    // When it is your turn, wait on sem and then perform task
    sem_wait(&blue);
    currBlueWorker++;
    // Make it loop back to first index
    if (currBlueWorker >= TEAM_SIZE) currBlueWorker = 0;
    waitForPackage(args->team, args->id);

}

void *redWaitingZone(void *arg) {
    struct threadArg *args = arg;

    // Stall while it's not your turn
    while (currRedWorker != args->id);

    // When it is your turn, wait on sem and then perform task
    sem_wait(&red);
    currRedWorker++;
    // Make it loop back to first index
    if (currRedWorker >= TEAM_SIZE) currRedWorker = 0;
    waitForPackage(args->team, args->id);
}

void *greenWaitingZone(void *arg) {
    struct threadArg *args = arg;

    // Stall while it's not your turn
    while (currGreenWorker != args->id);

    // When it is your turn, wait on sem and then perform task
    sem_wait(&green);
    currGreenWorker++;
    // Make it loop back to first index
    if (currGreenWorker >= TEAM_SIZE) currGreenWorker = 0;
    waitForPackage(args->team, args->id);

}

void *yellowWaitingZone(void *arg) {
    struct threadArg *args = arg;

    // Stall while it's not your turn
    while (currYellowWorker != args->id);

    // When it is your turn, wait on sem and then perform task
    sem_wait(&orange);
    currYellowWorker++;
    // Make it loop back to first index
    if (currYellowWorker >= TEAM_SIZE) currYellowWorker = 0;
    waitForPackage(args->team, args->id);

}

int main() {
    srand(12345);
    sem_init(&mutex, 0, 1);
    sem_init(&blue, 0, 1);
    sem_init(&red, 0, 1);
    sem_init(&green, 0, 1);
    sem_init(&orange, 0, 1);


    // Initialize the teams
    pthread_t redTeam[TEAM_SIZE];
    pthread_t blueTeam[TEAM_SIZE];
    pthread_t greenTeam[TEAM_SIZE];
    pthread_t yellowTeam[TEAM_SIZE];


    // Make packages
    for (int i = 0; i < TOTAL_PACKAGES; i++) {
        createPackage();
    }

    // Initialize red team
    for (int i = 0; i < TEAM_SIZE; i++) {
        struct Worker currWorker;
        strcpy(currWorker.team, "red");
        currWorker.id = i;
        struct threadArg *currThreadArg;
        currThreadArg = (struct threadArg *) malloc(sizeof(struct threadArg));
        strcpy(currThreadArg->team, currWorker.team);
        currThreadArg->id = i;
        currWorker.workerThread = pthread_create(&redTeam[i], NULL, redWaitingZone, (void *) currThreadArg);
        redTeamWorkers[i] = currWorker;
    }

// Initialize blue team
    for (int i = 0; i < TEAM_SIZE; i++) {
        struct Worker currWorker;
        strcpy(currWorker.team, "blue");
        currWorker.id = i;
        struct threadArg *currThreadArg;
        currThreadArg = (struct threadArg *) malloc(sizeof(struct threadArg));
        strcpy(currThreadArg->team, currWorker.team);
        currThreadArg->id = i;
        currWorker.workerThread = pthread_create(&blueTeam[i], NULL, blueWaitingZone, (void *) currThreadArg);
        blueTeamWorkers[i] = currWorker;

    }
// Initialize green team
    for (int i = 0; i < TEAM_SIZE; i++) {
        struct Worker currWorker;
        strcpy(currWorker.team, "green");
        currWorker.id = i;
        struct threadArg *currThreadArg;
        currThreadArg = (struct threadArg *) malloc(sizeof(struct threadArg));
        strcpy(currThreadArg->team, currWorker.team);
        currThreadArg->id = i;
        currWorker.workerThread = pthread_create(&greenTeam[i], NULL, greenWaitingZone, (void *) currThreadArg);
        greenTeamWorkers[i] = currWorker;

    }
// Initialize yellow team
    for (int i = 0; i < TEAM_SIZE; i++) {
        struct Worker currWorker;
        strcpy(currWorker.team, "yellow");
        currWorker.id = i;
        struct threadArg *currThreadArg;
        currThreadArg = (struct threadArg *) malloc(sizeof(struct threadArg));
        strcpy(currThreadArg->team, currWorker.team);
        currThreadArg->id = i;
        currWorker.workerThread = pthread_create(&yellowTeam[i], NULL, yellowWaitingZone, (void *) currThreadArg);
        yellowTeamWorkers[i] = currWorker;
        //printf("New red team worker created with ID: %d index: %d currThreadArg = %d\n", currWorker.id, i, currThreadArg->id);
    }
    pthread_exit(NULL);

}