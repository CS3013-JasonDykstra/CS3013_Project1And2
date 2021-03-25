#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/utsname.h>

pthread_cond_t condf, condt, conds, condall;

pthread_mutex_t mutex;

int numFlamencoDancers = 15;
int numTorchJugglers = 8;
int numSoloists = 2;

int numPerformersOnStage = 0;
int performersOnStage [4];
/*
 * 1: Dancer
 * 2: Juggler
 * 3: Soloist
 */

int numFlamencoDancersPerformed = 0;
int numTorchJugglersPerformed = 0;
int numSoloistsPerformed = 0;

int maxFlamencoDancersPerformed = 8;
int maxTorchJugglersPerformed = 8;
int maxSoloistsPerformed = 2;

int jugglersOnStage(){
    for(int i=0;i < 4; i++){
        if(performersOnStage[i] == 2){
            return 1;
        }
    }
    return 0;
}

int flamencosOnStage(){
    for(int i=0;i < 4; i++){
        if(performersOnStage[i] == 1){
            return 1;
        }
    }
    return 0;
}

int soloistsOnStage(){
    for(int i=0;i < 4; i++){
        if(performersOnStage[i] == 3){
            return 1;
        }
    }
    return 0;
}

void * flamencoDancerAct(void * tid){
    int stagePosition = 0;
    long * myID = (long *) tid;
    printf("Dancer created, ID: %ld\n", *myID);
    // Halt when stage is full

    while(1) {
        // Lock mutex, add yourself to list
        pthread_mutex_lock(&mutex);

        // Check if stuff is true
        //printf("Num performers is %d, jugglers on stage is %d, soloists on stage is %d, flamencos on stage is %d\n",numPerformersOnStage,jugglersOnStage(),soloistsOnStage(),flamencosOnStage());
        //while(numPerformersOnStage >= 4){
        while ((numPerformersOnStage >= 4) || (jugglersOnStage() == 1) || (soloistsOnStage() == 1) || (numFlamencoDancersPerformed > maxFlamencoDancersPerformed)) {
            pthread_cond_wait(&condall, &mutex);
        }

        // Do stuff while in mutex
        numPerformersOnStage++;
        // Add to first available position on stage
        for (int i = 0; i < 4; i++) {
            if (performersOnStage[i] == 0) {
                performersOnStage[i] = 1;
                stagePosition = i + 1;
                numFlamencoDancersPerformed++;
                numTorchJugglersPerformed = 0;
                numSoloistsPerformed = 0;
                break;
            }
        }

        // Leave mutex
        pthread_mutex_unlock(&mutex);

        // Perform!
        // create random number to sleep
        int sleepTime = rand() % 5 + 1;
        printf("I am dancer %ld I have entered stage position %d, I will sleep for %d seconds. Bye!\n", *myID,
               stagePosition, sleepTime);
        sleep(sleepTime);

        // Leave the stage
        // Grab the mutex since it is free, other threads are waiting with free mutex
        pthread_mutex_lock(&mutex);
        printf("Dancer of id: %ld is getting off the stage from position %d.\n", *myID, stagePosition);
        performersOnStage[stagePosition - 1] = 0;
        numPerformersOnStage--;
        //printf("Num Performers left on stage: %d\n", numPerformersOnStage);
        stagePosition = 0;
        //pthread_cond_broadcast(&condf);
        pthread_cond_broadcast(&condall);
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(0);
}

void * torchJugglerAct(void * tid){
    int stagePosition = 0;
    long * myID = (long *) tid;
    printf("Juggler created, ID: %ld\n", *myID);
    while(1) {
        // Lock mutex, add yourself to list
        pthread_mutex_lock(&mutex);

        // Check if stuff is true
        //printf("Num performers is %d, jugglers on stage is %d, soloists on stage is %d, flamencos on stage is %d\n",numPerformersOnStage,jugglersOnStage(),soloistsOnStage(),flamencosOnStage());
        //while(numPerformersOnStage >= 4){
        while ((numPerformersOnStage >= 4) || ((flamencosOnStage()) == 1) || ((soloistsOnStage()) == 1) || (numTorchJugglersPerformed > maxTorchJugglersPerformed)) {
            pthread_cond_wait(&condall, &mutex);
        }

        // Do stuff while in mutex
        numPerformersOnStage++;
        // Add to first available position on stage
        for (int i = 0; i < 4; i++) {
            if (performersOnStage[i] == 0) {
                performersOnStage[i] = 2;
                stagePosition = i + 1;
                numTorchJugglersPerformed++;
                numFlamencoDancersPerformed = 0;
                numSoloistsPerformed = 0;
                break;
            }
        }

        // Leave mutex
        pthread_mutex_unlock(&mutex);

        // Perform!
        // create random number to sleep
        int sleepTime = rand() % 5 + 1;
        printf("I am juggler %ld I have entered stage position %d, I will sleep for %d seconds. Bye!\n", *myID,
               stagePosition, sleepTime);
        sleep(sleepTime);

        // Leave the stage
        // Grab the mutex since it is free, other threads are waiting with free mutex
        pthread_mutex_lock(&mutex);
        printf("Juggler of id: %ld is getting off the stage from position %d.\n", *myID, stagePosition);
        performersOnStage[stagePosition - 1] = 0;
        numPerformersOnStage--;
        stagePosition = 0;
        //pthread_cond_broadcast(&condt);
        pthread_cond_broadcast(&condall);
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(0);
}

void * soloistAct(void * tid){
    int stagePosition = 0;
    long * myID = (long *) tid;
    printf("Soloist created, ID: %ld\n", *myID);

    while(1) {
        // Lock mutex, add yourself to list
        pthread_mutex_lock(&mutex);

        // Check if stuff is true
        //printf("Num performers is %d, jugglers on stage is %d, soloists on stage is %d, flamencos on stage is %d\n",numPerformersOnStage,jugglersOnStage(),soloistsOnStage(),flamencosOnStage());
        //while(numPerformersOnStage >= 1){
        while ((numPerformersOnStage >= 4) || ((soloistsOnStage()) == 1) || ((flamencosOnStage()) == 1) || ((jugglersOnStage()) == 1) || (numSoloistsPerformed > maxSoloistsPerformed)) {
            pthread_cond_wait(&condall, &mutex);
        }

        // Do stuff while in mutex
        numPerformersOnStage++;
        // Add to first available position on stage
        for (int i = 0; i < 4; i++) {
            if (performersOnStage[i] == 0) {
                performersOnStage[i] = 3;
                stagePosition = i + 1;
                numSoloistsPerformed++;
                numFlamencoDancersPerformed = 0;
                numTorchJugglersPerformed = 0;
                break;
            }
        }

        // Leave mutex
        pthread_mutex_unlock(&mutex);

        // Perform!
        // create random number to sleep
        int sleepTime = rand() % 5 + 1;
        printf("I am soloist %ld I have entered stage position %d, I will sleep for %d seconds. Bye!\n", *myID,
               stagePosition, sleepTime);
        sleep(sleepTime);

        // Leave the stage
        // Grab the mutex since it is free, other threads are waiting with free mutex
        pthread_mutex_lock(&mutex);
        printf("Soloist of id: %ld is getting off the stage from position %d.\n", *myID, stagePosition);
        performersOnStage[stagePosition - 1] = 0;
        numPerformersOnStage--;
        stagePosition = 0;
        //pthread_cond_broadcast(&conds);
        pthread_cond_broadcast(&condall);
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(0);
}



int main() {
    srand(123);

    pthread_mutex_init(&mutex, 0);

    // Initialize condition variables
    pthread_cond_init(&condf, 0);
    pthread_cond_init(&condt, 0);
    pthread_cond_init(&conds, 0);
    pthread_cond_init(&condall, 0);

    pthread_t flamencoDancers[numFlamencoDancers];
    pthread_t torchJugglers[numTorchJugglers];
    pthread_t soloists[numSoloists];
//    for (int i = 0; i < 10; i++){
//        pthread_t temp;
//        pthreads[i] = &temp;
//    }

    // Initialize flamenco dancers
    for(int i = 0; i < numFlamencoDancers; i++){
        pthread_create(&flamencoDancers[i], NULL, flamencoDancerAct, (void *)&flamencoDancers[i]);
    }

    // Initialize torch jugglers
    for(int i = 0; i < numTorchJugglers; i++){
        pthread_create(&torchJugglers[i], NULL, torchJugglerAct, (void *)&torchJugglers[i]);
    }

    // Initialize soloists
    for(int i = 0; i < numSoloists; i++){
        pthread_create(&soloists[i], NULL, soloistAct, (void *)&soloists[i]);
    }

    pthread_exit(NULL);

    pthread_mutex_destroy(&mutex);


}



