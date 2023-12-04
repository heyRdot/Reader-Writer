#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>
#include <iostream>
#include "read-write.h"

// define semaphore and shared memory keys
#define SHM_KEY 9876 
#define SHM_KEY 6788 

#define NUMSEMS 2

int main(){

    // -------------------- Intialisation ----------

	// int to store shared mem id
	// and int pointer to readercount
	int shmid, *readercount;
	struct sembuf Ssembuf,Wsembuf; // struct for signal and wait operations on each semaphore
	
	semRpref(int SHM_KEY, int NUMSEMS);
	
	std::ifstream dataFile; // file input stream variable
	std::string data;		// file data string variable

    cout << "Intializing Semaphores...\n";

    int semid = semget(SHM_KEY,NUMSEMS,0666|IPC_CREAT); // allocating two semaphores for usage using Sys V system call

    for(int i = 0; i < NUM_SEMS; i++) // intializing the value of each semaphore to 1
        semctl(semid,i,SETVAL,1);     // this is critical as the semaphores must only be initialized once

    cout << "Intializing Reader Shared Memory...\n";

    int *reader_counter; // creating pointer to read_counter integer
                         // this will be used as variable to reference shared memory of readers

    int shmid = shmget(SHMKEY,sizeof(*reader_counter),0666|IPC_CREAT); // creating 4 bytes (size of reader_counter integer) of shared memory 

    reader_counter = (int*)shmat(shmid, 0, 0); // assigning the address of shared memory to reader_counter
    
    *reader_counter = 0; // ensuring reader_counter is initialised to 0
                         // this is critical as reader_counter must only be initialized once

    cout << "** Intialization Complete! **\nPlease feel free to run any readers and writers now!\n";

    return 0;
    
}