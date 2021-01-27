#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <fcntl.h> 
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


#define BUFSIZE 80

//childNode
struct childNode{
    int data;
    struct childNode* next;
};

//ProcessInfo  -   Structure to hold data related to each process
struct ProcessInfo{
    int procID;
    int numChild;
    int **childPipe;

};

//processTree  -   Structure to hold data initially in adjacency list format
struct processTree{
    int procID;
    int parId;
    int numChild;
    struct childNode *children;
};


//Prototyping
struct processTree* getInput(char *s, int *childNodes);
void initProcessTree(struct processTree l[],int n);
void fillProcessTree(struct processTree p[], int n, int c, int par);
int isEmpty(const char *input);
void printProcessTree(struct processTree p[], int n);
int getRoot(struct processTree* p, int n);
int findIndex(struct processTree p[], int n, int d);
struct ProcessInfo* initProcessInfo(struct processTree a[], int rootIndex);
void buildProcTree(struct processTree p[], struct ProcessInfo *r, struct ProcessInfo rDash, int n);
int leaderElection(struct ProcessInfo* pL, int root, int *loc);
int getSum(struct ProcessInfo* pL, int leader);
void terminateProcess(struct ProcessInfo* pL, int leader);


int main(int argc, char** argv){

    if (argc != 2){
                printf("Argument passed is not correct,Exactly 1 argument required\n");
                exit(0);
        }

    int numNodes,root;
    struct processTree* pT = getInput(argv[1], &numNodes);   //file to processTree

    root  = getRoot(pT,numNodes);     //root
    int rootIndex = findIndex(pT,numNodes,root);   //index of root

    struct ProcessInfo* pI = initProcessInfo(pT,rootIndex);   //process Info of current process
    struct ProcessInfo pIPrime;

    int mainID = getpid();

    buildProcTree(pT, pI, pIPrime, numNodes);    //build process Tree using forkn() and setup pipes



    int loc;
	
	//leader
    int leader = leaderElection(pI, root, &loc); 

    //for leader make him feel like root by making every pipe as its child
    if(pI->procID == leader){
        pI->numChild++;
        pI->childPipe[pI->numChild][0] = -1;
        pI->childPipe[pI->numChild][1] = -1;
    }
    else{
        // for root change one (in direction of leader) of its child's pipe as its parent's pipe
        if(pI->childPipe[pI->numChild][0] == -1){
            int apip = pI->childPipe[loc][0];
            int bpip = pI->childPipe[loc][1];

            pI->childPipe[loc][0] = pI->childPipe[pI->numChild-1][0];
            pI->childPipe[loc][1] = pI->childPipe[pI->numChild-1][1];

            pI->childPipe[pI->numChild-1][0] = apip;
            pI->childPipe[pI->numChild-1][1] = bpip;
            pI->numChild--;
        }

        //swap pipes to points towards the process which gave the leader(min tag)
        else{
            int apip = pI->childPipe[loc][0];
            int bpip = pI->childPipe[loc][1];

            pI->childPipe[loc][0] = pI->childPipe[pI->numChild][0];
            pI->childPipe[loc][1] = pI->childPipe[pI->numChild][1];

            pI->childPipe[pI->numChild][0] = apip;
            pI->childPipe[pI->numChild][1] = bpip;

        }
    }

    char line[BUFSIZE];
    char line2[BUFSIZE];

    sleep(2);

    int sum = getSum(pI,leader);
	
	//leader will print the sum
    if(pI->procID ==leader){  
        printf("Sum of ID's process IDs : %d\n",sum);
    }
    char rtds[BUFSIZE];

    sleep(2);


	 //termination of all process, initated by leader
    terminateProcess(pI,leader); 

    return 0;
}


//Function to parse input file
struct processTree* getInput(char *s, int *childNodes){
    FILE *fptr; 

        fptr = fopen(s, "r");
    if (fptr == NULL){
                printf("Error in opening the file\n");
                exit(0);
        }

    char *input = (char*)malloc(50*sizeof(char));
    int numEdges=0;
    while(fgets (input, 50, fptr)!=NULL){
        if(isEmpty(input)){
            continue;
        }

        numEdges++;  
    }
    fclose(fptr);

    *childNodes = numEdges + 1;
    struct processTree* pT = (struct processTree*)(malloc((*childNodes)*sizeof(struct processTree)));
    initProcessTree(pT,(*childNodes));

        fptr = fopen(s, "r");
    if (fptr == NULL){
                printf("Error in opening the file\n");
                exit(0);
        }

    char* token;
    int c,par;

    while(fgets (input, 50, fptr)!=NULL){
        if(isEmpty(input)){ 
            continue;
        }

        token = strtok(input, ",");
        c = atoi(token);

        token = strtok(0, ",");
        par = atoi(token);

        fillProcessTree(pT, (*childNodes), c, par);

    }
    fclose(fptr);
    return pT;
}

//Function to initialize Process tree struct
void initProcessTree(struct processTree l[],int n){
        for(int i=0;i<n;i++){
        l[i].numChild=0;
        l[i].procID = -1;
        l[i].parId = -1;
        l[i].children = NULL;
    }
}

//Function to add data in Process tree struct
void fillProcessTree(struct processTree p[], int n, int c, int par){
        struct childNode* newchildNode = (struct childNode*)malloc(sizeof(struct childNode));
        newchildNode->data = c;

    int rootIndex = findIndex(p, n, par);

    newchildNode->next = p[rootIndex].children;
    p[rootIndex].children = newchildNode;
    p[rootIndex].numChild++;
    p[rootIndex].procID = par;

    rootIndex = findIndex(p, n, c);
    p[rootIndex].procID = c;
    p[rootIndex].parId = par;

}

//Function to check if input line is empty or not
int isEmpty(const char *input)
{
    char ch;
    int len = strlen(input);
    //int i=0;
    while (len--)
    {
        ch = *(input++);
        if(ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r' && ch != '\0')
            return 0;
    }

    return 1;
}

//Function to print Process tree
void printProcessTree(struct processTree p[], int n){
        int i=0;
        struct childNode* tmp = NULL;
        printf("processTrees:\n\n");
        for(i=0; i<n; i++){
                printf("Tag%d, Par%d, ChildC%d: ",p[i].procID,p[i].parId,p[i].numChild);
        tmp = p[i].children;
                while(tmp!=NULL){
                        printf("%d, ",tmp->data);
                        tmp = tmp->next;
                }
                printf("\n");
        }
}

//Function to find root ID of processTree
int getRoot(struct processTree* p, int n){
    for(int i=0;i<n;i++){
        if(p[i].parId == -1){
            return p[i].procID;
        }
    }
}


//Function to find the index for a particular value
int findIndex(struct processTree p[], int n, int d){
    for(int i=0;i<n;i++){
        if(p[i].procID == d || p[i].procID == -1){//-1 represent empty location
            return i;
        }
    }
}


//Function to initialize Process Info for each process
struct ProcessInfo* initProcessInfo(struct processTree pT[], int rootIndex){
        struct ProcessInfo* pI = (struct ProcessInfo*)(malloc(sizeof(struct ProcessInfo)));

    pI->numChild = pT[rootIndex].numChild;
    pI->procID = pT[rootIndex].procID;

    int n = pT[rootIndex].numChild;

    pI->childPipe =  (int**)malloc((n+2)*sizeof(int*));
    for(int j=0;j<n+2;j++){
        pI->childPipe[j] =  (int*)malloc(2*sizeof(int));
    }

    pI->childPipe[n][0] = -1;
    pI->childPipe[n][1] = -1;

    return pI;
}

//Function to build process tree using fork()
void buildProcTree(struct processTree pT[], struct ProcessInfo *pI, struct ProcessInfo pIPrime, int n){
    if(pI->numChild == 0){
        return;
    }


    int root = pI->procID;
    int tag = root;
    int pid;
    int i=0;
    int rootIndex = findIndex(pT, n, root);    
	
    struct childNode* tmp = NULL;
    tmp = pT[rootIndex].children;

    while(tmp!=NULL){

        int childrootIndex = findIndex(pT, n, tmp->data);

        int pip[2];
        int pip2[2];
        pipe(pip);
        pipe(pip2);

        pI->childPipe[i][0] = pip[0];
        pI->childPipe[i++][1] = pip2[1];

        pIPrime.numChild = pT[childrootIndex].numChild;
        pIPrime.procID = pT[childrootIndex].procID;

        //For child
        if((pid = fork())==0){
            i=0;

            for(int j=0;j<pI->numChild+1;j++){
                free(pI->childPipe[j]);
            }
            free(pI->childPipe);


            pI->numChild = pIPrime.numChild;
            pI->procID = pIPrime.procID;

            int nChild = pIPrime.numChild;

            pI->childPipe =  (int**)malloc((nChild+2)*sizeof(int*));
            for(int j=0;j<nChild+2;j++){
                pI->childPipe[j] =  (int*)malloc(2*sizeof(int));
            }

            pI->childPipe[nChild][0] = pip2[0];
            pI->childPipe[nChild][1] = pip[1];

            close(pip[0]);
            close(pip2[1]);

            buildProcTree(pT, pI, pIPrime, n);

            return;
        }
        else{
           //For parent
           close(pip[1]);
           close(pip2[0]);
        }

        tmp = tmp->next;
    }
}

//Function to find leader
int leaderElection(struct ProcessInfo* pI, int root, int *loc){
    char line[BUFSIZE];
    char line2[BUFSIZE];

    bool w_sp = false;
    int numNeighbours = pI->numChild+1;

    if(pI->procID == root){
        numNeighbours--;
        w_sp = true;
        for(int r=0;r<numNeighbours;r++){
            sprintf(line,"WakeUp\n");
            write(pI->childPipe[r][1], line, BUFSIZE);
            line[0]='\0';
        }
    }

    for(int r=numNeighbours-1; r>=0; r--){
        read(pI->childPipe[r][0], line2, BUFSIZE);
        line2[0]='\0';
        if(!w_sp){
            w_sp = true;
            for(int r=0;r<numNeighbours;r++){
                sprintf(line,"WakeUp\n");
                write(pI->childPipe[r][1], line, BUFSIZE);
                line[0]='\0';
            }
        }
    }

    int neighbours = numNeighbours;
    neighbours--;
    int val_p = pI->procID;
    int value;
    for(int r=0; r<neighbours; r++){
        
        read(pI->childPipe[r][0], line2, BUFSIZE);
        sscanf(line2,"%d",&value);
        line2[0]='\0';

        if(value<val_p){
            val_p = value;
            *loc = r;
        }
    }

    sprintf(line,"%d\n",val_p);
    write(pI->childPipe[neighbours][1], line, BUFSIZE);
    line[0]='\0';

    read(pI->childPipe[neighbours][0], line2, BUFSIZE);
    sscanf(line2,"%d",&value);
    if(value<val_p){
        val_p = value;
        *loc = neighbours;
    }

    int status = 0; 
    if(val_p == pI->procID){
       status=1;
    }
    else{
        status=2;
    }


    printf("I am %d, My leader is %d\n",pI->procID, val_p);

    for(int r=0; r<neighbours; r++){
        sprintf(line,"%d\n",val_p);
        write(pI->childPipe[r][1], line, BUFSIZE);
        line[0]='\0';
    }

    return val_p;
}


//Function to find sum of all process's ID
int getSum(struct ProcessInfo* pI, int leader){

    char line[BUFSIZE];
    char line2[BUFSIZE];

    int numNeighbours = pI->numChild+1;
    if(pI->procID == leader){
        numNeighbours--;
    }

    int neighbours = numNeighbours;
    if(pI->procID != leader){
        neighbours--;
    }

    int val_p = pI->procID;
    int value;

    for(int r=0; r<neighbours; r++){
        read(pI->childPipe[r][0], line2, BUFSIZE);
        value=0;
        sscanf(line2,"%d",&value);
        line2[0]='\0';
        val_p = val_p + value;
    }


    if(pI->procID != leader){
        sprintf(line,"%d\n",val_p);
        write(pI->childPipe[neighbours][1], line, BUFSIZE);
        line[0]='\0';
    }

    if(pI->procID == leader){
        read(pI->childPipe[neighbours][0], line2, BUFSIZE);
        value=0;
        sscanf(line2,"%d",&value);
        val_p = val_p + value;
    }

    return val_p;

}



//Function to ask every process to terminate
void terminateProcess(struct ProcessInfo* pI, int leader){

    char line[BUFSIZE];
    char line2[BUFSIZE];
    int numNeighbours = pI->numChild;

    if(pI->procID != leader){
        read(pI->childPipe[pI->numChild][0], line2, BUFSIZE);
        line2[0]='\0';
    }

    for(int r=0;r<numNeighbours;r++){
        sprintf(line,"%d,%d\n",-100,pI->procID);
        write(pI->childPipe[r][1], line, BUFSIZE);
        line[0]='\0';
    }

    int procWaitStatus;
    while ((procWaitStatus = wait(NULL)) > 0);
    printf("I am process %d and i am terminating\n",pI->procID);

    exit(0);

}