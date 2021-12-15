// Morgan Fisher. Project 2


typedef struct FRAME {
    //  If assigned to process, it is 1, otherwise 0
    int  assigned;
    char location[40];
    int  procAssign;
    int  pageNum;
} FRAME;

typedef struct frameList {
    FRAME* frames;
    int noFrames;
    int pageSize;
} frameList;

frameList* create_frame_list(int noFrames, int pageSize) {
    int i;

    frameList *f;

    f = malloc(sizeof(frameList));

    f->frames = (FRAME*)malloc(sizeof(FRAME) * noFrames);
    f->pageSize = pageSize;
    f->noFrames = noFrames;

    for (i = 0; i < f->noFrames; i ++) {
        f->frames[i].assigned = 0;
    }

    return f;
}

int proc_can_fit_into_memory(frameList* list, PROCESS* proc) {
    int i = 0;
    int numFreeFrames = 0;

    for (i = 0; i < list->noFrames; i += 1) {
        if (!list->frames[i].assigned) {
            numFreeFrames += 1;
        }
    }

    // if the number of free frames * the page size is greater than the mem req
    // for the process in question we can fit it in.
    return (numFreeFrames * list->pageSize) >= proc->mem_reqs;
}

void fit_proc_into_memory(frameList* list, PROCESS* proc) {
    // if already checked that you can fit the proc into mem
    int i = 1;
    int remainingMem = 1;
    int currentPage = 1;

    remainingMem = proc->mem_reqs;

    for (i = 0; i < list->noFrames; i ++) {
        // if frame is not assigned
        if (!list->frames[i].assigned) {
            // assign
            list->frames[i].assigned = 1;
            // set page no.
            list->frames[i].pageNum = currentPage;
            // set proc #
            list->frames[i].procAssign = proc->pid;

            currentPage++;
            remainingMem -= list->pageSize;
        }

        if (remainingMem <= 0) {
            break;
        }
    }
}

void print_frame_list(frameList* list) {

    int i;
    int freeBlock = 0;
    int start = 0;

    printf("\tMemory map:\n");

    for (i = 0; i < list->noFrames; i += 1) {
        if (!freeBlock && !list->frames[i].assigned) {
            freeBlock = 1;
            start = i;
        } else if (freeBlock && list->frames[i].assigned) {
            freeBlock = 0;
            printf("\t\t%d-%d: Free frame(s)\n",
                   start * list->pageSize,
                   (i * list->pageSize) - 1);
        }

        if (list->frames[i].assigned) {
            printf("\t\t%d-%d: Process %d, Page %d\n",
                   i * list->pageSize,
                   ((i + 1) * list->pageSize) - 1,
                   list->frames[i].procAssign,
                   list->frames[i].pageNum);
        }
    }

    if (freeBlock) {
        printf("\t\t%d-%d: Free frame(s)\n",
               start * list->pageSize,
               ((i) * list->pageSize) - 1);
    }
}

int frame_list_is_empty(frameList* list) {
    int i;

    for (i = 0; i < list->noFrames; i ++) {
        if (list->frames[i].assigned) {
            return 0;
        }
    }

    return 1;
}

void free_memory_for_pid(frameList* list, int pid) {
    int i;
    FRAME* frame;

    for (i = 0; i < list->noFrames; i ++) {
        frame = &list->frames[i];

        if (frame->procAssign == pid) {
            frame->procAssign = 0;
            frame->pageNum = 0;
            frame->assigned = 0;
        }
    }
}