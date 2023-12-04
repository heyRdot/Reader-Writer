#ifndef INITAL
#define INITAL

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <fstream>
#include <iostream>

using namespace std;

class initial
{
    public:

    initial(int SEM_KEY, int NUMSEMS);
    ~initial(); // default destructor
    void read(); // function which reads contents of file.txt and outputs to user
    void write(); // function which appends file.txt with user input

private:
    // signal and wait semaphore buffer
    struct sembuf Ssembuf, Wsembuf;
    int semid; // int to store semaphore id
};

#endif