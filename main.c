// Morgan Fisher. Project 2
// Operating Systems 2021
// 

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "declarations.h"



const int TIMEMAX = 10000;


int numbProcs = 0;
int lastAnnounce = -1;
PROCESS* proc_list;
proc_queue* queue;
frameList* flist;


void mainLoop() {
    long current_time = 0;

    while (1) {
        // queue any procs that have arrived
        enNewProc(current_time);

        // remove any completed procs
        clearCProcs(current_time);

        // assign memory to procs that is available
        assignMemoryWaitProc(current_time);

        current_time++;

        if (current_time > TIMEMAX) {
            printf("DEADLOCK: over the max time\n");
            break;
        }

        if (queue->size == 0 && frame_list_is_empty(flist)) {
            break;
        }
    }

    printTurnTimes();
}

int main() {
    int page_size  = 0;
    int mem_size   = 0;

    char* file_path = malloc(100 * sizeof(char));


    //user input
    usrInput(&mem_size, &page_size, file_path);

    // , file_path

    // read values from file into a shared proc list
    proc_list = assign_process_list(file_path);

    // create a shared queue with a capacity = # of procs
    queue = create_proc_queue(numbProcs);

    // create a shared framelist
    flist = create_frame_list(mem_size / page_size, page_size);

    mainLoop();

    return 0;
}

void enNewProc(int current_time) {
    int i;
    PROCESS* proc;

    for (i = 0; i < numbProcs; i += 1) {
        proc = &proc_list[i];

        if (proc->arrival_time == current_time) {
            printf("%sProcess %d arrives\n",
                   get_announcement_prefix(current_time),
                   proc->pid);

            enqueue_proc(queue, proc);

            print_proc_queue(queue);
            print_frame_list(flist);
        }
    }
}

void clearCProcs(int current_time) {
    int i, time_spent_in_memory;
    PROCESS* proc;

    // dequeue any procs that need it
    for (i = 0; i < numbProcs; i ++) {
        proc = &proc_list[i];
        time_spent_in_memory = current_time - proc->time_added_to_memory;

        if (proc->is_active && (time_spent_in_memory >= proc->life_time)) {
            printf("%sProcess %d completes\n",
                   get_announcement_prefix(current_time),
                   proc->pid);

            proc->is_active = 0;
            proc->time_finished = current_time;

            free_memory_for_pid(flist, proc->pid);

            print_frame_list(flist);
        }
    }
}

void assignMemoryWaitProc(int current_time) {
    int i, index, limit;
    PROCESS* proc;

    limit = queue->size;

    // enqueue any procs that can be put into mem
    for (i = 0; i < limit; i ++) {
        index = iterate_queue_index(queue, i);
        proc = queue->elements[index];

        if (proc_can_fit_into_memory(flist, proc)) {
            printf("%sMM moves Process %d to memory\n",
                   get_announcement_prefix(current_time),
                   proc->pid);

            fit_proc_into_memory(flist, proc);

            proc->is_active = 1;
            proc->time_added_to_memory = current_time;

            dequeue_proc_at_index(queue, i);
            print_proc_queue(queue);
            print_frame_list(flist);
        }
    }
}

char* get_announcement_prefix(int current_time) {
    char* result;

    result = malloc(20 * sizeof(char));

    if (lastAnnounce == current_time) {
        sprintf(result, "\t");
    } else {
        sprintf(result, "t = %d: ", current_time);
    }

    lastAnnounce = current_time;

    return result;
}

void printTurnTimes() {
    int i;
    float total = 0;

    for (i = 0; i < numbProcs; i ++) {
        total += proc_list[i].time_finished - proc_list[i].arrival_time;
    }

    printf("Average Turnaround Time: %2.2f\n", total / numbProcs);
}

int multipleHundred(int t) {
    return (t % 100) == 0 ? 1 : 0;
}

int isOneOrTwo(int t) {
    return (t >= 1 && t <= 2) ? 1 : 0;
}

void clrStd(char* buf) {
    if (buf[strlen(buf) - 1] != '\n') {
        int ch;
        while (((ch = getchar()) != '\n') && (ch != EOF)) ;
    }
}

int processNumbUser(const char* output, int (*func)(int)) {
    char buf[10];
    int success = 0;
    int res = 0;

    while (!success) {
        printf("%s: ", output);

        if (fgets(buf, 10, stdin) == NULL) {
            clrStd(buf);
            printf("ERROR: There is no data entered\n");

            continue;
        }

        if (sscanf(buf, "%d", &res) <= 0) {
            clrStd(buf);
            printf("ERROR: A number was not entered\n");

            continue;
        }

        if (!(success = (*func)(res))) {
            clrStd(buf);
            printf("ERROR: Not a valid number\n");
        }
    }

    return res;
}

void promptFileName(char* res) {
    char buf[100];
    FILE* fp;


    while (1) {
        printf("Input file: ");

        if (fgets(buf, 100, stdin) == NULL) {
            clrStd(buf);
            printf("ERROR: No data was entered\n");

            continue;
        }

        if (sscanf(buf, "%s", res) <= 0) {
            clrStd(buf);
            printf("ERROR: No string was entered\n");

            continue;
        }

        if (!(fp = fopen(res, "r"))) {
            perror("ERROR: File not opened\n");
        } else {
            break;
        }
    }
}

// prompts for memory size and page size
void usrInput(int* mem, int* page,  char* file_path) {


    while (1) {
        *mem = processNumbUser(
            "Memory size", multipleHundred);

        *page = processNumbUser(
            "Page size (1: 150, 2: 250)", isOneOrTwo);

        if (*page =150) {
        break;
        }
        else if(*page =250){
        break;
        }
        if ((*mem) % (*page) == 0) {
            break;
        }
        else{
        
        printf("ERROR: Memory size must be a multiple of the page");
        printf(" %d is not a multiple of %d, please retry.\n", *mem, *page);
        }
    }

    promptFileName(file_path);

}

// # of processes from file
int fileProcessNumber(FILE* filePtr) {
    int num = 0;

    fscanf(filePtr, "%d", &num);

    return num;
}

// stores values processes in process array
PROCESS* assign_process_list(const char* file_path) {
    int numSpace;
    int tmp;
    int counter = 0;
    int totalSpace = 0;
    FILE* filePtr = fopen(file_path, "r");

    numbProcs = fileProcessNumber(filePtr);

    // allocate space for process array
    PROCESS* procList = malloc(numbProcs * sizeof(PROCESS));

    if (!filePtr) {
        printf("ERROR: Failed to open file %s", file_path);
        exit(1);
    }

    while (!feof(filePtr) && counter < numbProcs) {
        // store values for processes
        fscanf(filePtr, "%d %d %d %d",
               &(procList[counter].pid),
               &(procList[counter].arrival_time),
               &(procList[counter].life_time),
               &numSpace);

        // get total memory requirements for process
        totalSpace = 0;
        for (int i = 0; i < numSpace; i++) {
            fscanf(filePtr, "%d", &tmp);
            totalSpace += tmp;
        }
        procList[counter].mem_reqs = totalSpace;

        procList[counter].is_active = 0;
        procList[counter].time_added_to_memory = -1;
        procList[counter].time_finished = -1;

        counter++;
    }

    fclose(filePtr);

    return procList;
}