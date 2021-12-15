// Morgan Fisher.Project 2

#include "process.h"
#include "queue.h"
#include "memory.h"


PROCESS* assign_process_list(const char* file_path);

// main declaration
int main();

//  main loop declaration 
void mainLoop();

// returns a string (T = x) or a tab (\t) based on clock
char* get_announcement_prefix(int current_time);

// get number of processes 
int fileProcessNumber(FILE* filePtr);

// numeric input from the user 
int processNumbUser(const char* output, int (*func)(int));

// assigns any available memory to waiting procs
void assignMemoryWaitProc(int current_time);

//  extra chars cleared from stdin
void clrStd(char* buf);

// completed procs from memory removed
void clearCProcs(int current_time);


// adds any newly arrived procs to the input queue
void enNewProc(int current_time);


//user input: mem size/ # of pages / path to input file)
void usrInput(int* mem, int* page, char* file_path); 

// returns a 1 if the provided int (t) is 1, or 2 
int isOneOrTwo(int t);

// returns a 1 if the provided int (t) is a multiple of 100 and a 0 otherwise
int multipleHundred(int t);

// prints the average turnaround time
void printTurnTimes();

// prompts the user for a valid filename
void promptFileName(char* res);

