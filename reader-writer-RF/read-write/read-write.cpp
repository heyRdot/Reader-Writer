#include <string>
#include <fstream>
#include <iostream>
using namespace std;

void read(){
    
    cout << "Reading the file \n";
    ifstream myfile; //vairable to input file
	string data;   //variable to store data from file

    if(myfile.open("input.txt", ios::in); { //if input file is open
    
        cout << "Contents of file: \n";
		cout << "------- Start of file: -------\n";

     	while (getline(myfile,data))
		{
			cout << data << endl;
		
        }
		cout << "------- End of file: -------\n";
        myfile.close(); // close file 
		cin.ignore();
    }
    else{ // if file cannot be accessed output error message 
       cout << "Error in reading file" << endl;
    }
    
    return 0; // return to main
}

void write(){

	cout << "Wrting to file \n";
    ofstream myfile; //vairable to output file
	string input;   //variable to store data from user 

    if(myfile.open("input.txt", ios::out); { //if file is open for outputting
    
		cout << "------- Enter a message using the keyboard:  -------\n";
		cin << input; //read in input from user
		myfile << input; // write the data to into file
        myfile.close(); // close file 
		cout << "Message written is: " << input << endl;
		cin.ignore();
    }
    else{ // if file cannot be accessed output error message 
       cout << "Error in reading file" << endl;
    }
    return 0; // return to main
}

// implementation of wait function

void wait(int sem_num)
{
	// set sem number to perform operation on
    Wsembuf.sem_num = sem_num;
	// call sem wait operation on sem set of id == semid
    semop(semid, &Wsembuf, 1);
}

void signal(int sem_num)
{
	// set sem number to perform operation on
    Ssembuf.sem_num = sem_num;
	// call sem signal operation on sem set of id == semid
    semop(semid, &Ssembuf, 1);
}


void semRpref(int SHM_KEY, int NUMSEMS)
{
    int semid = semget(SHM_KEY, NUMSEMS, 0666 | IPC_CREAT);
	
	// for each semaphore
    for (int i = 0; i < NUMSEMS; i++)
    {
		// initialise to 1 (unblocked)
        semctl(semid, i, SETVAL, 1);
    }

	    cout << "Intializing Reader Shared Memory...\n";

    int *readerCount; // creating pointer to read_counter integer
                         // this will be used as variable to reference shared memory of readers

    int shmid = shmget(SHMKEY,sizeof(*readerCount),0666|IPC_CREAT); // creating 4 bytes (size of reader_counter integer) of shared memory 

    readerCount = (int*)shmat(shmid, 0, 0); // assigning the address of shared memory to reader_counter
    
    *readerCount = 0; // ensuring reader_counter is initialised to 0
                         // this is critical as reader_counter must only be initialized once

	// Wait Semaphore
    Wsembuf.sem_op = -1; // decrement sem value when called
    Wsembuf.sem_flg = SEM_UNDO; // set SEM_UNDO flag

	// Signal Semaphore 
    Ssembuf.sem_op = 1; // increment sem value when called
    Ssembuf.sem_flg = SEM_UNDO; // set SEM_UNDO flag
}

