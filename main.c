/* This is code developed by Imane Sayd */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


struct process
{
    int bT, aT, priority, p_no;
    int wT, remaining_bT;
    struct process *next;
};

//Initializations:
struct process *header = NULL;
char *input_file = NULL;
char *output_file = NULL;

//Basic LL functions prototypes:
struct process *createProcess(int,int,int,int);
struct process *insertBack(struct process *,int,int,int,int);
struct process *deleteFront(struct process *);
int sizeofLL(struct process *); 

struct process *finding_highest_priority(struct process *, int);
struct process *copyLL(struct process *);

//File manipulation functions:
void readFile(const char *, struct process **);
void reinitializeFile();

//CPU Sheduling Algorithms:
void FCFS(struct process *, const char *);
void SJF_NonPreemptive(struct process *, const char*);
void SJF_Preemptive(struct process *, const char *);
void Priority_NonPreemptive(struct process *, const char *);
void Priority_Preemptive(struct process *, const char *);
void RR_algorithm(struct process *, int, const char *);

//Our main:
int main(int argc, char *argv[])
{
extern char *optarg;
//Getting arguments from outside using getopt
int opt;
int timeQuantum = 0;
while((opt = getopt(argc, argv, "t:f:o:")) != -1){
    switch (opt){
        case 't':
          timeQuantum = atoi(optarg);
          break;
        case 'f':
          input_file = optarg;
          break;
        case 'o':
          output_file = optarg;
          reinitializeFile();
          break;
        default:
         fprintf(stderr, "Usage: %s [-t] timeQuantum [-f] input_file [-o] output_file\n", argv[0]);
         exit(EXIT_FAILURE);

    }
}

if (timeQuantum == 0 || input_file == NULL || output_file == NULL){
    fprintf(stderr, "Invalid usage!\n");
}

readFile(input_file, &header);

//Creating copies for each scheduling algorithm
struct process *FCFS_list = copyLL(header);
struct process *SJF_NonPreemptive_list = copyLL(header);
struct process *SJF_Preemptive_list = copyLL(header);
struct process *Priority_NonPreemptive_list = copyLL(header);
struct process *Priority_Preemptive_list = copyLL(header);
struct process *RR_algorithm_list = copyLL(header);

FCFS(FCFS_list, output_file);
SJF_NonPreemptive(SJF_NonPreemptive_list, output_file);
SJF_Preemptive(SJF_Preemptive_list, output_file);
Priority_NonPreemptive(Priority_NonPreemptive_list, output_file);
Priority_Preemptive(Priority_Preemptive_list, output_file);
RR_algorithm(RR_algorithm_list, timeQuantum, output_file);

//Freeing the original list
while(header != NULL){
    struct process *temp = header;
    header = (*header).next;
    free(temp);
}

}

struct process *createProcess(int bt, int at, int pr, int p_num)
{
    struct process *temp;
    temp = (struct process *) malloc(sizeof(struct process));
    (*temp).bT = bt;
    (*temp).aT = at;
    (*temp).wT = 0;
    (*temp).priority = pr;
    (*temp).p_no = p_num;
    (*temp).remaining_bT = bt;
    (*temp).next = NULL;

    return temp;
}

struct process *insertBack(struct process *header, int bt, int at, int pr, int p_num)
{
    struct process *temp = createProcess(bt, at, pr, p_num);
    if(header == NULL){
        return temp;}
    
    struct process *headertemp = header;
    while (((*headertemp).next) != NULL){
        headertemp = (*headertemp).next;
    }
    (*headertemp).next = temp;

    return header;
}

struct process *deleteFront(struct process *header)
{
    struct process *temp;
    if(header == NULL){
        return header;
    }
    temp = header;
    header = (*header).next;
    free(temp);

    return header;
}


int sizeofLL(struct process *header)
{
    struct process *temp = header;
    int size = 0;
    while(temp != NULL){
        size++;
        temp = (*temp).next;
    }
    return size;
}

struct process *copyLL(struct process *header)
{
    struct process *newHeader = NULL;
    struct process *current = header;

    while(current != NULL){
        newHeader = insertBack(newHeader, (*current).bT, (*current).aT, (*current).priority, (*current).p_no);
        current = (*current).next;
    }
    return newHeader;
}

void readFile(const char *filename, struct process **header)
{
    int bursttime, arrivaltime, priority;
    int p_num = 1;
    FILE *file = fopen(filename, "r");
    if(file == NULL){
        printf("File not found!\n");
        exit(EXIT_FAILURE);
    }
    printf("Input file content:\n");
    /*Since array usage is not allowed I will use this method instead of fgets in order to check for the end of the file */
    while(fscanf(file, "%d:%d:%d", &bursttime, &arrivaltime, &priority) == 3){
        printf("%d:%d:%d\n", bursttime, arrivaltime, priority); /*to show in the terminal*/
        *header = insertBack(*header, bursttime, arrivaltime, priority, p_num);
        p_num++;
    }
//To ensure that if the input file is not empty it must have three columnns: bT, aT and priority 
    if(!feof(file)){
        fprintf(stderr, "Input file format is not valid!\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    
    fclose(file);
}

/*In order not to keep appending the values in the output file each time we run*/
void reinitializeFile(){
    FILE *file =fopen(output_file, "w");
    fprintf(file, " ");
    fclose(file);
}

void FCFS(struct process *header, const char *filename)
{
    int p_count = sizeofLL(header);
    int current_time = 0;
    int sumWT = 0;
    float avgWT = 0.0;

    FILE *file = fopen(filename, "w");
    if(file == NULL){
        printf("file failed to open!\n");
        return;
    }

//If input file is empty wT and avgWT are set to 0
    if(header == NULL){
        fprintf(file, "1:0:0.0\n");
        printf("1:0:0.0\n");
        fclose(file);
        return;
    }


    struct process *temp = header;
    fprintf(file, "1:");
    printf("Output file content:\n1:");

    //The scheduling begins here
    while (temp != NULL){
        if (current_time < (*temp).aT){
            current_time = (*temp).aT;
        }
        //Waiting time for the current process
        (*temp).wT = current_time - (*temp).aT;
        if((*temp).wT < 0) (*temp).wT = 0;

        sumWT  += (*temp).wT;
        fprintf(file, "%d:", (*temp).wT);
        printf("%d:", (*temp).wT);

        current_time += (*temp).bT;

        //Go to next process of the list
        temp = (*temp).next;
    }
    avgWT = (float)sumWT / p_count;
    fprintf(file, "%.1f\n", avgWT);
    printf("%.1f\n", avgWT);
    fclose(file);
}

void SJF_NonPreemptive(struct process *header, const char *filename)
{
    int p_count = sizeofLL(header);
    int current_time = 0;
    int sumWT = 0;
    float avgWT = 0.0;

    struct process *output = NULL;
    int flag = 0; //To check if a process was found
    int max_aT = 100000; //Defining a large arrival value

    FILE *file = fopen(filename, "a");
    if(file == NULL){
        printf("file failed to open!\n");
        return;
    }

//If input file is empty wT and avgWT are set to 0
    if(header == NULL){
        fprintf(file, "2:0:0.0\n");
        printf("2:0:0.0\n");
        fclose(file);
        return;
    }

    fprintf(file, "2:");
    printf("2:");

    //The scheduling begins here:
    while(header != NULL){
        struct process *temp = header;
        struct process *shortestJob = NULL;
        struct process *previous = NULL; //To track the previous process while iterating to help setting prevSelected
        struct process *prevShortest = NULL; //Points to the process before shortest

        while(temp != NULL){
            if((*temp).aT <= current_time){
                if((shortestJob == NULL) || (*temp).bT < (*shortestJob).bT || ((*temp).bT ==(*shortestJob).bT && ((*temp).aT < (*shortestJob).aT))){
                shortestJob = temp;
                prevShortest = previous;
                flag = 1;
            }
        }

        if ((*temp).aT > current_time && (*temp).aT < max_aT){
            max_aT = (*temp).aT;
        }
            previous = temp;
            temp = (*temp).next;
        }

        if(!flag){
            current_time = max_aT;
            continue;
        }

        (*shortestJob).wT = current_time - (*shortestJob).aT;

        if((*shortestJob).wT < 0){
            (*shortestJob).wT = 0;
        }

        sumWT += (*shortestJob).wT;

        struct process *sorted_output = createProcess((*shortestJob).bT, (*shortestJob).aT, (*shortestJob).priority, (*shortestJob).p_no);
        (*sorted_output).wT = (*shortestJob).wT;

        if(output == NULL || (*sorted_output).p_no < (*output).p_no){
            (*sorted_output).next = output;
            output = sorted_output;
        }
        else{
            struct process *current = output;
            while((*current).next != NULL && (*(*current).next).p_no < (*sorted_output).p_no){
                current = (*current).next;
            } 
            (*sorted_output).next = (*current).next;
            (*current).next = sorted_output;
        }
        
        current_time += (*shortestJob).bT;

        //Remove selected process from the list
        if(header == shortestJob){
            header = deleteFront(header);
        }else{
            struct process *current = header;
            while((*current).next != shortestJob){
                current = (*current).next;
            }
            (*current).next = (*shortestJob).next;
            free(shortestJob);
        }
    }
    //Display by the same order of process in input file
    struct process *current = output;
    struct process *temp;
    while(current != NULL){
        fprintf(file, "%d:", (*current).wT);
        printf("%d:", (*current).wT);
        temp = current;
        current = (*current).next;
    }

    avgWT = (float)sumWT / p_count;
    fprintf(file, "%.1f\n", avgWT);
    printf("%.1f\n", avgWT);
    fclose(file);
}

void SJF_Preemptive(struct process *header, const char *filename)
{
    int current_time = 0;
    int p_count = sizeofLL(header);
    int sumWT = 0;
    float avgWT = 0.0;
    struct process *output = NULL;
    int finished = 0;

    FILE *file = fopen(filename, "a");
    if(file == NULL){
        printf("File failed to open!\n");
        return;
    }

//If input file is empty wT and avgWT are set to 0
    if(header == NULL){
        fprintf(file, "3:0:0.0\n");
        printf("3:0:0.0\n");
        fclose(file);
        return;
    }

    fprintf(file, "3:");
    printf("3:");

    //List for tracking remaining_bT
    struct process *list = NULL;
    struct process *temp = header;

    while(temp != NULL){    
        list = insertBack(list, (*temp).bT, (*temp).aT, (*temp).priority, (*temp).p_no);
        temp = (*temp).next;
    }
    //The scheduling begins here
    while(finished < p_count){
        struct process *shortestJob = NULL;
        struct process *current = list;

        int max_aT = 10000; //Earliest aT
        current = list;
        while(current != NULL){
            if((*current).remaining_bT > 0 && (*current).aT < max_aT){
                max_aT = (*current).aT;
            }
            current = (*current).next;
        }
        if(max_aT > current_time){
            current_time = max_aT;
        }
        current = list;
        while(current != NULL){
            if((*current).remaining_bT > 0 && (*current).aT <= current_time){
                if(shortestJob == NULL || (*current).remaining_bT < (*shortestJob).remaining_bT ||
                ((*current).remaining_bT == (*shortestJob).remaining_bT &&
                (*current).aT <= (*shortestJob).aT &&
                (*current).p_no < (*shortestJob).p_no)){
                    shortestJob = current;
                }
            }
            current = (*current).next;
        }
        
        if(shortestJob != NULL){
            (*shortestJob).remaining_bT--;
            current_time++;

            if((*shortestJob).remaining_bT == 0){
                finished++;
                int waiting_Time = current_time - (*shortestJob).aT - (*shortestJob).bT;

                if(waiting_Time < 0){
                    waiting_Time = 0;
                }
                sumWT += waiting_Time;

                //Displaying by the same order of process in input file
                struct process *sorted_output = createProcess((*shortestJob).bT, (*shortestJob).aT, (*shortestJob).priority, (*shortestJob).p_no);
                (*sorted_output).wT = waiting_Time;
                (*sorted_output).next = NULL;

                if(output == NULL || (*sorted_output).p_no < (*output).p_no){
                    (*sorted_output).next = output;
                    output = sorted_output;
                }else{
                     struct process *now = output;
                     while((*now).next != NULL && (*(*now).next).p_no < (*sorted_output).p_no){
                        now = (*now).next;
                     }
                     (*sorted_output).next = (*now).next;
                     (*now).next = sorted_output;
                }
            }
        }
    }

    //Printing out the final results
    struct process *curr = output;
    while(curr != NULL){
        fprintf(file, "%d:", (*curr).wT);
        printf("%d:", (*curr).wT);
        struct process *temp = curr;
        curr = (*curr).next;
        free(temp);
    }

    avgWT = (float) sumWT / p_count;
    fprintf(file, "%.1f\n", avgWT);
    printf("%.1f\n", avgWT);
    fclose(file);

}


//Function to help in finding the highest priority ready process
struct process *finding_highest_priority(struct process *header, int current_time){
    struct process *highest_priority = NULL;
    int max = 10000; //Highest priority

    while(header != NULL){
        if((*header).aT <= current_time && (*header).remaining_bT > 0 && (*header).priority < max){
            max = (*header).priority;
            highest_priority = header;
        }
        header = (*header).next;
    }
    return highest_priority;
}


void Priority_NonPreemptive(struct process *header, const char *filename)
{
    int p_count = sizeofLL(header);
    int current_time = 0;
    int sumWT = 0;
    float avgWT = 0.0;
    struct process *output = NULL;

    FILE *file = fopen(filename, "a");
    if(file == NULL){
        printf("File failed to open!\n");
        return;
    }

//If input file is empty wT and avgWT are set to 0
    if(header == NULL){
        fprintf(file, "4:0:0.0\n");
        printf("4:0:0.0\n");
        fclose(file);
        return;
    }

    fprintf(file, "4:");
    printf("4:");

    //The scheduling begins here:
    while(header != NULL){
        struct process *selected = NULL;
        int nextArrival = 10000;

        selected = finding_highest_priority(header, current_time);

        struct process *temp = header;
        while(temp != NULL){
            if((*temp).aT > current_time && (*temp).aT < nextArrival){
                nextArrival = (*temp).aT;
            }
            temp = (*temp).next;
        }

        if(selected == NULL){
            current_time = nextArrival;
            continue;
        }

        (*selected).wT = current_time - (*selected).aT;

        if((*selected).wT < 0){
            (*selected).wT = 0;
        }
        sumWT += (*selected).wT;

        struct process *sorted_output = createProcess((*selected).bT, (*selected).aT, (*selected).priority, (*selected).p_no);
        (*sorted_output).wT = (*selected).wT;

        if(output == NULL || (*sorted_output).p_no < (*output).p_no){
            (*sorted_output).next = output;
            output = sorted_output;
        }else{
            struct process *current = output;
            while((*current).next != NULL && (*(*current).next).p_no < (*sorted_output).p_no){
                current = (*current).next;
            }
            (*sorted_output).next = (*current).next;
            (*current).next = sorted_output;
        }

        current_time += (*selected).bT;

        if(header == selected){
            header = deleteFront(header);
        }else{
            struct process *current = header;
            while((*current).next != selected){
                current = (*current).next;
                }
                (*current).next = (*selected).next;
                free(selected);
            }
        }

        struct process *current = output;
        struct process *temp;
        while(current != NULL){
            fprintf(file, "%d:", (*current).wT);
            printf("%d:", (*current).wT);
            temp = current;
            current = (*current).next;
            free(temp);
        }

        avgWT = (float) sumWT / p_count;
        fprintf(file, "%.1f\n", avgWT);
        printf("%.1f\n", avgWT);
        fclose(file);

}


void Priority_Preemptive(struct process *header, const char *filename)
{
    int p_count = sizeofLL(header);
    int current_time = 0;
    int sumWT = 0;
    int finished = 0;
    float avgWT = 0.0;
    struct process *temp = header;
    struct process *output = NULL;

    FILE *file = fopen(filename, "a");
    if(file == NULL){
        printf("File failed to open!\n");
        return;
    }

//If input file is empty wT and avgWT are set to 0
    if(header == NULL){
        fprintf(file, "5:0:0.0\n");
        printf("5:0:0.0\n");
        fclose(file);
        return;
    }

    fprintf(file, "5:");
    printf("5:");

    while(temp != NULL){
        (*temp).remaining_bT = (*temp).bT;
        temp = (*temp).next;
    }

    while(finished < p_count){
        struct process *current = finding_highest_priority(header, current_time);


        if(current){
            (*current).remaining_bT--;
            if((*current).remaining_bT == 0){
                finished++;
                int waiting_Time = current_time + 1 - (*current).aT - (*current).bT;
                (*current).wT = waiting_Time;

                if(waiting_Time < 0) waiting_Time = 0;

                sumWT += waiting_Time;
            }
        }
        current_time++;

    }

//Displaying the result by the same order of process in input file
    temp = header;
    while(temp){
        fprintf(file, "%d:", (*temp).wT);
        printf("%d:", (*temp).wT);
        temp = (*temp).next;
        
    }

    avgWT = (float) sumWT / p_count;
    fprintf(file, "%.1f\n", avgWT);
    printf("%.1f\n", avgWT);
    fclose(file);
}

void RR_algorithm(struct process *header, int timeQuantum, const char *filename)
{
    struct process *temp = header;
    int current_time = 0;
    int p_count = sizeofLL(header);
    int sumWT = 0;
    float avgWT = 0.0;

    FILE *file = fopen(filename, "a");
    if(file == NULL){
        printf("File failed to open!\n");
        return;
    }

    if(timeQuantum <= 0){
        if(file){
            printf("6:Error:time quantum is invalid!\n");
            fprintf(file, "6:Error:time quantum is invalid!\n");
            fclose(file);
        }
        return;
    }

//If input file is empty wT and avgWT are set to 0
    if(header == NULL){
        fprintf(file, "6:0:0.0\n");
        printf("6:0:0.0\n");
        fclose(file);
        return;
    }

    fprintf(file, "6:");
    printf("6:");


    int finished;
    do{
        finished = 1;
        temp = header;

        while(temp != NULL){ //Skipping processes with bT = 0
            if((*temp).bT == 0){
                (*temp).remaining_bT = 0;
                temp = (*temp).next;
                continue;
            }

            if((*temp).remaining_bT > 0){
                finished = 0; //Which means at least one process is not done

                if((*temp).aT <= current_time){
                    if((*temp).remaining_bT > timeQuantum){
                        current_time += timeQuantum;
                        (*temp).remaining_bT -= timeQuantum;
                    }else{
                        current_time += (*temp).remaining_bT;
                        (*temp).remaining_bT = 0;
                        (*temp).wT = current_time - (*temp).bT - (*temp).aT;

                        if((*temp).wT < 0){
                            (*temp).wT = 0;
                        }
                        sumWT += (*temp).wT;
                    }
                }
            }
            temp = (*temp).next;
        }
        //Advancing time if no process is ready yet
        if(finished == 0){
            temp = header;
            int next_aT = -1;
            while(temp != NULL){
                if((*temp).remaining_bT > 0 && (*temp).aT > current_time){
                    if(next_aT == -1 || (*temp).aT < next_aT){
                        next_aT = (*temp).aT;
                    }
                }
                temp = (*temp).next;
            }
            if(next_aT > current_time){
                current_time = next_aT;
            }
        }

    }while(!finished);

    temp = header;
    while(temp){
        fprintf(file, "%d:", (*temp).wT);
        printf("%d:", (*temp).wT);
        temp = (*temp).next;
    }

    avgWT = (float) sumWT / p_count;
    fprintf(file, "%.1f\n", avgWT);
    printf("%.1f\n", avgWT);

    fclose(file);

}
