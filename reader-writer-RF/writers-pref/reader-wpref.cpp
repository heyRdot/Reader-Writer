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

    int shmidR, shmidW, semid, *readerCount;

    std::ifstream myfile;           //vairable to input file
    std::string data;               //variable to store data from file
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

    // Wait Semaphore
    Wsembuf.sem_op = -1;        // decrement sem value when called
    Wsembuf.sem_flg = SEM_UNDO; // set SEM_UNDO flag

    // Signal Semaphore
    Ssembuf.sem_op = 1;         // increment sem value when called
    Ssembuf.sem_flg = SEM_UNDO; // set SEM_UNDO flag

    // -------------------- Reader ---------------
    while (1)
    { // infinite loop

        cout << "Press enter to read from the file "; // ask user to press enter to start the file read
        cin.ignore();                                 // ignore input

        Wsembuf.sem_num == TRYREAD; //select appropriate semaphore and perform wait operation for try read
        semop(semid, &Wsembuf, 1);  // readers not allowed until writers are done with file
        Wsembuf.sem_num == READER;  //select appropriate semaphore and perform wait operation for reader
        semop(semid, &Wsembuf, 1);

        *readerCount += 1; // increment reader counter

        if (*readerCount == 1) // if the reader is the first reader
        {
            Wsembuf.sem_num == FILE; // select appropriate semaphore and perform wait operation
            semop(semid, &Wsembuf, 1); //wait for access to file 
        }
        Ssembuf.sem_num = READER;  //select appropriate semaphore and perform signal operation
        semop(semid, &Ssembuf, 1); //signal to allow other readers access

        Ssembuf.sem_num == TRYREAD; //select appropriate semaphore and perform signal operation
        semop(semid, &Ssembuf, 1);  //readers not allowed until writers are done with file

        cout << "Reading the file \n";
        myfile.open("input.txt"); //open input file

        if (myfile.is_open()) //if myfile is open
        {
            cout << "Contents of file: \n";
            cout << "------- Start of file: -------\n";

            //store data from myfile
            while (myfile >> data)
            {
                cout << data << endl; //print out data
            }
            cout << "------- End of file: -------\n";
            myfile.close(); // close file
        }
        else
        { // if file cannot be opened output error message
            cout << "Error in reading file" << endl;
        }
        Wsembuf.sem_num = READER; //select appropriate semaphore and perform wait operation
        semop(semid, &Wsembuf, 1);

        *readerCount -= 1; // decrement reader counter

        if (*readerCount == 0)
        { // if the current reader is the last

            Ssembuf.sem_num = FILE;    //select appropriate semaphore and perform signal operation
            semop(semid, &Ssembuf, 1); // and allow writers access
        }
        Ssembuf.sem_num = READER;  //select appropriate semaphore and perform signal operation
        semop(semid, &Ssembuf, 1); //allows the next reader to gain access
    }
}
