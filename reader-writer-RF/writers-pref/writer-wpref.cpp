#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>
#include <iostream>
#include <fstream>
using namespace std;

// define semaphore and shared memory keys
#define SEM_KEY 1234
#define RSHM_KEY 5678
#define WSHM_KEY 8765

//define each semaphore and its flag

#define READER 0  // READER SEM
#define FILE 1    // FILE SEM
#define TRYREAD 2 // TRYREAD SEM
#define WRITER 3  // WRITER SEM

//define number of semaphores
#define NUMSEMS 4

int main()
{

    // -------------------- Intialisation ---------------

    int shmidR, shmidW, semid, *readerCount, *writerCount;

    std::ofstream myfile;           //vairable to output file
    std::string input;              //variable to store input from file from user
    struct sembuf Ssembuf, Wsembuf; // struct for signal and wait operations on each semaphore

    // semaphore id, get semaphore(key, number of sems, 0666 set read write permission )
    semid = semget(SEM_KEY, NUMSEMS, 0666 | IPC_CREAT);
    // intializing the value of each semaphore to 1
    for (int i = 0; i < NUMSEMS; i++)
    {
        semctl(semid, i, SETVAL, 1);
    }

    // initialise shared memory for Reader of 4 bytes
    shmidR = shmget(RSHM_KEY, 4, 0666 | IPC_CREAT);
    // set the reader counter pointer and initialise to 0
    readerCount = (int *)shmat(shmidR, 0, 0);
    *readerCount = 0;

    // initialise shared memory for writer of 4 bytes
    shmidW = shmget(WSHM_KEY, 4, 0666 | IPC_CREAT);
    // set the writer counter pointer and initialise to 0
    writerCount = (int *)shmat(shmidW, 0, 0);
    *writerCount = 0;

    // Wait Semaphore
    Wsembuf.sem_op = -1;        // decrement sem value when called
    Wsembuf.sem_flg = SEM_UNDO; // set SEM_UNDO flag

    // Signal Semaphore
    Ssembuf.sem_op = 1;         // increment sem value when called
    Ssembuf.sem_flg = SEM_UNDO; // set SEM_UNDO flag

    // -------------------- Writer ---------------
    while (1)
    { // infinite loop

        cout << "Press enter to write into the file "; // requesting start a file write
        cin.ignore();                                  // ignoring input value

        //select appropriate semaphore and perform wait operation until writing is done
        Wsembuf.sem_num = FILE;
        semop(semid, &Wsembuf, 1);

        *writerCount += 1; // increment writer counter

        if (*writerCount == 1)
        {                               // if the writer is the first writer
            Wsembuf.sem_num == TRYREAD; // perform a wait operation on the try read semaphore
            semop(semid, &Wsembuf, 1);  // denies access to readers until writing operation is done
        }
        Ssembuf.sem_num = WRITER;  // select appropriate semaphore and perform signal operation
        semop(semid, &Ssembuf, 1); //allows other writers access

        Wsembuf.sem_num == FILE;   // select appropriate semaphore and wait operation
        semop(semid, &Wsembuf, 1); // denies access to file by readers and writers until writing operation is done

        // ** WRITING TO FILE **
        cout << "Writing to file \n";
        ofstream myfile("input.txt"); //vairable to output file

        if (myfile.is_open())
        { //if file is open for outputting

            cout << "------- Enter a message using the keyboard:  -------\n";
            cin >> input;                                    //read in input from user
            myfile << input;                                 // write the data to into file
            myfile.close();                                  // close file
            cout << "Message written is: " << input << endl; //print out written message
            cin.ignore();
        }
        else
        { // if file cannot be opened output error message
            cout << "Error reading in file" << endl;
        }

        Wsembuf.sem_num = WRITER;  // select appropriate semaphore and perform wait operation
        semop(semid, &Wsembuf, 1); //only one writer at a time

        *writerCount -= *writerCount; // decrement writer counter

        if (*writerCount == 0)
        { // if the current writer is the last

            Ssembuf.sem_num = TRYREAD; // select appropriate semaphore and signal operation
            semop(semid, &Ssembuf, 1); // writing operation is done so reader are allowed
        }
		//else the other writers are allows access
        Ssembuf.sem_num = WRITER;  // select appropriate semaphore and signal operation
        semop(semid, &Ssembuf, 1); // allow access for writers
    }
}
