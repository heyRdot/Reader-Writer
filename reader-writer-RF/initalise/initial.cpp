#include "initial.h"

initial::initial(int SEM_KEY, int NUMSEMS)
{
    semid = semget(SEM_KEY, NUMSEMS, 0666 | IPC_CREAT);

    // initialise each semaphore to !
    for (int i = 0; i < NUMSEMS; i++)
    {
        semctl(semid, i, SETVAL, 1);
    }

    // Initialise sem op buffers 
    // Wait Semaphore
    Wsembuf.sem_op = -1;        // decrement sem value operator
    Wsembuf.sem_flg = SEM_UNDO; // set SEM_UNDO flag

    // Signal Semaphore
    Ssembuf.sem_op = 1;         // increment sem value operator
    Ssembuf.sem_flg = SEM_UNDO; // set SEM_UNDO flag
}

// default destructor
initial::~initial()
{
}

void initial::read()
{

    cout << "Reading the file \n";
    ifstream myfile; //vairable to input file
    string data;     //variable to store data from file

    myfile.open("input.txt"); //open file

    if (myfile.is_open())
    {
        cout << "Contents of file: \n";
        cout << "------- Start of file: -------\n";

        while (myfile >> data)
        {
            cout << data << endl;
        }
        cout << "------- End of file: -------\n";
        myfile.close(); // close file
    }
    else
    { // if file cannot be accessed output error message
        cout << "Error reading in file" << endl;
    }
    return; // return to main
}

void initial::write()
{
    ofstream myfile; //vairable to output file
    string input;    //variable for user input
    cout << "Writing to file \n";
    ofstream myfile("input.txt"); //vairable to output file

    if (myfile.is_open())
    { //if file is open for outputting

        cout << "------- Enter a message using the keyboard:  -------\n";
        cin >> input;    //read in input from user
        myfile << input; // write the data to into file
        myfile.close();  // close file
        cout << "Message written is: " << input << endl;
        cin.ignore();
    }
    else
    { // if file cannot be accessed output error message
        cout << "Error reading in file" << endl;
    }
    return; // return to main
}
