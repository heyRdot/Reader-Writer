#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
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

	int shmid, semid;

	std::ofstream myfile;			//vairable to output file
	std::string input;				//variable to store input from file from user
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

	// -------------------- Writer ---------------
	while (1)
	{ // infinite loop

	cout << "Press enter to write to file "; // ask user to press enter to start the file read
		cin.ignore();								  // ignore input

		//select appropriate semaphore and perform wait operation until writing is done
		Wsembuf.sem_num = FILE;	   
		semop(semid, &Wsembuf, 1);

		cout << "Writing to file \n";
		ofstream myfile("input.txt"); //open output file

		if (myfile.is_open())
		{ //if file is open for outputting

			cout << "------- Enter a message using the keyboard:  -------\n";
			cin >> input;	 //read in input from user
			myfile << input; // write the data to into file
			myfile.close();	 // close file
			cout << "Message written is: " << input << endl;
			cin.ignore();
		}

		else
		{ // if file cannot be accessed output error message
			cout << "Error in reading file" << endl;
		}
		myfile.close(); // close file
		cin.ignore();

		Ssembuf.sem_num = FILE;	   //select appropriate semaphore and perform signal operation
		semop(semid, &Ssembuf, 1); //signal to reader and writer that writing op is done and are able to gain access
	}
}
