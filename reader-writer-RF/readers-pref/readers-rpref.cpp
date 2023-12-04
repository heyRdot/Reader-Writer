#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>
#include <iostream>
#include <fstream>

using namespace std;

// define semaphore and shared memory keys
#define SEM_KEY 9876
#define SHM_KEY 6789

//define each semaphore and its flag
#define READER 0
#define FILE 1

//define number of semaphores
#define NUMSEMS 2

int main()
{

	// -------------------- Intialisation ---------------

	int shmid, semid, *readerCount;

	std::ifstream myfile;			//variable to input file
	std::string data;				//variable to store data from file
	struct sembuf Ssembuf, Wsembuf; // struct for signal and wait operations on each semaphore

	// semaphore id, get semaphore(key, number of sems, 0666 set read write permission )
	semid = semget(SEM_KEY, NUMSEMS, 0666 | IPC_CREAT);

	// intializing the value of each semaphore to 1
	for (int i = 0; i < NUMSEMS; i++)
	{
		semctl(semid, i, SETVAL, 1);
	}

	// initialise shared memory of 4 bytes
	shmid = shmget(SHM_KEY, 4, 0666 | IPC_CREAT);

	// Wait Semaphore
	Wsembuf.sem_op = -1;		// decrement sem value when called
	Wsembuf.sem_flg = SEM_UNDO; // set SEM_UNDO flag

	// Signal Semaphore
	Ssembuf.sem_op = 1;			// increment sem value when called
	Ssembuf.sem_flg = SEM_UNDO; // set SEM_UNDO flag

	// set the reader counter pointer and initialise to 0
	readerCount = (int *)shmat(shmid, 0, 0);
	*readerCount = 0;

	// -------------------- Reader ---------------
	while (1)
	{ // infinite loop

		cout << "Press enter to read from the file "; // ask user to press enter to start the file read
		cin.ignore();								  // ignore input

		//select appropriate semaphore and perform wait operation for reader
		Wsembuf.sem_num == READER;
		semop(semid, &Wsembuf, 1);
		*readerCount += 1; // increment reader counter

		if (*readerCount == 1) // if the reader is the first reader
		{
			Wsembuf.sem_num == FILE; // select appropriate semaphore and perform wait operation
			semop(semid, &Wsembuf, 1);
		}
		Ssembuf.sem_num = READER;  //select appropriate semaphore and perform signal operation
		semop(semid, &Ssembuf, 1); //signal to allow other readers access

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

			Ssembuf.sem_num = FILE;	   //select appropriate semaphore and perform signal operation
			semop(semid, &Ssembuf, 1); // and allow writers access
		}
		Ssembuf.sem_num = READER;  //select appropriate semaphore and perform signal operation
		semop(semid, &Ssembuf, 1); //allows the next reader to gain access
	}
}
