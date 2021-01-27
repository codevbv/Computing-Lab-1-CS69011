


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/reqTypes.h>
#include <sys/ipc.h>
#include <sys/shm.h>    /* Include this to use shared mem */
#include <sys/sem.h>	/* Include this to use semaphores */
#include <unistd.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <fcntl.h> // library for fcntl function
#include <string.h>

#define P(s) semop(s, &p_op, 1)  /* p_op is the structure we pass for doing the P(s) operation */
#define V(s) semop(s, &v_op, 1)  /* v_op is the structure we pass for doing the V(s) operation */




//To store the inputLine data

struct reservationReq{
    int reqType;  
    int pnr;
    int trainId;
    char passengerName[20], age[3], sex;
    char class[4];
};


//To store Train wise info

struct train{
    int trainId;
    int AC2, AC3, SC;
    struct reservation reservationList[2000];
	int numReserved;
};


//To store the details of the passenger

struct reservation{
    int pnr;
    char passengerName[20], age[3], sex;
    char class[4];
    int status; 
};

int shmid,shmid2,shmid3; 
struct sembuf p_op, v_op ;
int mutexW[3], mutexR[3],x[3],y[3],z[3], printLock,i ;
struct train *trainInfo;
int *a, *b, *countRead, *countWrite;


//Prototyping
int isEmpty(const char *inputLine);

int main(int argc, char** argv){
    
    if (argc != 5){ 
		printf("Argument passed is not correct,Exactly 5 argument required\n \n"); 
		exit(0); 
	}
	
	
	int numChild = 4;
    int tempNumChild = numChild;
	int p_IDs[numChild];
    int myPID=991;//909 changed to 991
    int tag=100;
	
	
	//Semaphores
	for(int i=0;i<3;i++){
        mutexW[i] = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
        mutexR[i] = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
        x[i] = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
        y[i] = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
        z[i] = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
    
        semctl(mutexW[i], 0, SETVAL, 1);
        semctl(mutexR[i], 0, SETVAL, 1);
        semctl(x[i], 0, SETVAL, 1);
        semctl(y[i], 0, SETVAL, 1);
        semctl(z[i], 0, SETVAL, 1);
    }
	
	//Shared memory
    printLock = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
    semctl(printLock, 0, SETVAL, 1);
	shmid = shmget(IPC_PRIVATE, 3*sizeof(struct train), 0777|IPC_CREAT);
    shmid2 = shmget(IPC_PRIVATE, 3*sizeof(int), 0777|IPC_CREAT);
    shmid3 = shmget(IPC_PRIVATE, 3*sizeof(int), 0777|IPC_CREAT);
	
	countRead = (int *) shmat(shmid2, 0, 0);
    countWrite = (int *) shmat(shmid3, 0, 0);

	p_op.sem_num = v_op.sem_num = 0;
	p_op.sem_flg = v_op.sem_flg = SEM_UNDO;
	p_op.sem_op = -1 ; v_op.sem_op = 1 ;


    trainInfo = (struct train *) shmat(shmid, 0, 0);
	
	int p = 0;
    while(p!=3){
        countRead[p] = 0;
        countWrite[p] = 0;
		p++;
    }
    
	p =0;
    while(p!=3){
        trainInfo[p].AC2 = 10;
        trainInfo[p].AC3 = 10;
        trainInfo[p].SC = 10;
        trainInfo[p].numReserved = 0;
        trainInfo[p].trainId = i;
    }
	
    while(tempNumChild--){
        if((p_IDs[tempNumChild] = fork()) == 0){
            myPID = 0;
            tag = tempNumChild;
            trainInfo = (struct train *) shmat(shmid, 0, 0);
            countRead = (int *) shmat(shmid2, 0, 0);
            countWrite = (int *) shmat(shmid3, 0, 0);
            break;
		}  
	}

    if(myPID == 0){
		
        sleep(1);
        
        FILE *ipFile;  
        char inputLine[100];   
        
        ipFile = fopen(argv[tag+1], "r"); 
        if (ipFile == NULL){ 
            exit(0); 
        }

        while(fgets (inputLine, 100, ipFile)!=NULL){
            if(isEmpty(inputLine)){
                continue;
            }
            
            struct reservationReq req;
            parseInput(inputLine,&req);

            int reqTrainID;
            if(req.reqType == 0){
                reqTrainID = req.trainId;
            }
            else{
                int pnrId = req.pnr;
                reqTrainID = pnrId % 10;
            }
            
            get_read_lock(reqTrainID);
        
            if(req.reqType !=0){ 
				int pnrId = req.pnr;
                P(printLock);
                printf("\n%d: Ticket  having PNR %d is to be cancelled\n",tag+1,pnrId);
                V(printLock);
            }
            else{
                 
                if(req.class[0] == 'S'){
                    P(printLock);
                    printf("\nTag %d:Number of available seats in Train %d and Class SC is => %d\n",tag+1,reqTrainID,trainInfo[reqTrainID].SC);
                    V(printLock);
                }
                else if(req.class[2] == '2'){
                    P(printLock);
                    printf("\nTag %d:Number of available seats in Train %d and Class AC2 is => %d\n",tag+1,reqTrainID,trainInfo[reqTrainID].AC2);
                    V(printLock);
                }
                else if(req.class[2] == '3'){
                    P(printLock);
                    printf("\nTag %d:Number of available seats in Train %d and Class AC3 is => %d\n",tag+1,reqTrainID,trainInfo[reqTrainID].AC3);
                    V(printLock);
                }
            }
            
            release_read_lock(reqTrainID);
            
            get_write_lock(reqTrainID);
            
            
            if(req.reqType !=0){ 
                  int pnrId = req.pnr;
                int numSeats = pnrId / 10;

                if(numSeats < trainInfo[reqTrainID].numReserved && trainInfo[reqTrainID].reservationList[numSeats].status!=2){
                
                    if(trainInfo[reqTrainID].reservationList[numSeats].status == 1){
                        char curClass[4];
                        strcpy(curClass, trainInfo[reqTrainID].reservationList[numSeats].class);
                        curClass[3] ='\0';
                        
                        int waitFlag=0; 

                        if(curClass[0] == 'S'){
                            trainInfo[reqTrainID].SC++;
                            if(trainInfo[reqTrainID].SC<=0){
                                waitFlag =1;
                            }
                            curClass[2] ='\0';
                        }
                        else if(curClass[2] == '2'){
                            trainInfo[reqTrainID].AC2++;
                            if(trainInfo[reqTrainID].AC2<=0){
                                waitFlag =1;
                            }
                        }
                        else if(curClass[2] == '3'){
                            trainInfo[reqTrainID].AC3++;
                            if(trainInfo[reqTrainID].AC3<=0){
                                waitFlag =1;
                            }
                        }

                        for(int p=numSeats+1;(p<trainInfo[reqTrainID].numReserved && waitFlag);p++){
                            if(strcmp(trainInfo[reqTrainID].reservationList[p].class,curClass) == 0 && trainInfo[reqTrainID].reservationList[p].status == 0){
                                trainInfo[reqTrainID].reservationList[p].status = 1;
                                P(printLock);
                                printf("\nTag %d: Passenger %s has been removed from waitlist by process %d, allocated PNR %d\n",tag+1,trainInfo[reqTrainID].reservationList[p].passengerName,tag+1,trainInfo[reqTrainID].reservationList[p].pnr);
                                V(printLock);
                                break;
                            }
                        }
                    }
                    
                    trainInfo[reqTrainID].reservationList[numSeats].status = 2;
                    
                    P(printLock);
                    printf("\nTag %d: Reservation for PNR %d of passenger %s has been cancelled by process %d\n",tag+1,trainInfo[reqTrainID].reservationList[numSeats].pnr, trainInfo[reqTrainID].reservationList[numSeats].passengerName,tag+1);
                    V(printLock);
                    
                }
                else{
                    P(printLock);
                    printf("\nTag %d: No valid ticket with PNR %d is found\n",tag+1,pnrId);
                    V(printLock);  
                }
            }
            else{
				int curReserved = trainInfo[reqTrainID].numReserved;
                
                if(req.class[0] == 'S'){
                    trainInfo[reqTrainID].SC--;
                    if(trainInfo[reqTrainID].SC<0){
                        trainInfo[reqTrainID].reservationList[curReserved].status = 0;
                    }
                    else{
                        trainInfo[reqTrainID].reservationList[curReserved].status = 1;
                    }
                }
                else if(req.class[2] == '2'){
                    trainInfo[reqTrainID].AC2--;
                    if(trainInfo[reqTrainID].AC2<0){
                        trainInfo[reqTrainID].reservationList[curReserved].status = 0;
                    }
                    else{
                        trainInfo[reqTrainID].reservationList[curReserved].status = 1;
                    }
                }
                else if(req.class[2] == '3'){
                    trainInfo[reqTrainID].AC3--;
                    if(trainInfo[reqTrainID].AC3<0){
                        trainInfo[reqTrainID].reservationList[curReserved].status = 0; 
                    else{
                        trainInfo[reqTrainID].reservationList[curReserved].status = 1; 
                    }
                }
                strcpy(trainInfo[reqTrainID].reservationList[curReserved].age, req.age);
                strcpy(trainInfo[reqTrainID].reservationList[curReserved].class, req.class);
                strcpy(trainInfo[reqTrainID].reservationList[curReserved].passengerName, req.passengerName);
                trainInfo[reqTrainID].reservationList[curReserved].sex = req.sex;
                
                char line[10];
                sprintf(line,"%d%d\n",curReserved,reqTrainID);    
                trainInfo[reqTrainID].reservationList[curReserved].pnr = atoi(line);
                
                trainInfo[reqTrainID].numReserved++;

                if(trainInfo[reqTrainID].reservationList[curReserved].status){
                    P(printLock);
                    printf("\nTag %d: Reservation made succesfully for passenger %s in process %d, allocated PNR %d\n",tag+1,trainInfo[reqTrainID].reservationList[curReserved].passengerName,tag+1,trainInfo[reqTrainID].reservationList[curReserved].pnr);
                    V(printLock);
                }
                else{
                    P(printLock);
                    printf("\nTag %d: Passenger %s has been added to waitlist by process %d\n",tag+1,trainInfo[reqTrainID].reservationList[curReserved].passengerName,tag+1);
                    V(printLock);
                }
                 
            }
            
            release_write_lock(reqTrainID);

        }
        fclose(ipFile);
        shmdt(trainInfo);
		shmdt(countRead);
		shmdt(countWrite);
        sleep(5);
        exit(0);
    }

    if(myPID!=0){
        int terminateStatus;
        while ((terminateStatus = wait(NULL)) > 0);
        shmdt(trainInfo);
		shmdt(countRead);
		shmdt(countWrite);
		
		shmctl(shmid,IPC_RMID,NULL);
		shmctl(shmid2,IPC_RMID,NULL);
		shmctl(shmid3,IPC_RMID,NULL);  
	
        for(int i=0;i<3;i++){
			semctl(mutexW[i], 0, IPC_RMID, 1);
			semctl(mutexR[i], 0, IPC_RMID, 1);
			semctl(x[i], 0, IPC_RMID, 1);
			semctl(y[i], 0, IPC_RMID, 1);
			semctl(z[i], 0, IPC_RMID, 1);
		}
		semctl(printLock, 0, IPC_RMID, 1);
        
        printf("Parent %d terminated\n",getpid());
		exit(0);
    }
    
}

//To check if line is empty or not

int isEmpty(const char *inputLine)
{
    int length = strlen(inputLine);
	char c;
    for(int p = 0;p<3;p++)
    {
        c = *(inputLine++);
        if(c != ' '){
			return 0;
		}
		if(c != '\t'){
			return 0;
		}
		if(c != '\n'){
			return 0;
		}
		if(c != '\r'){
			return 0;
		}
		if(c != '\0'){
			return 0;
		}
    }
    return 1;
}


/*
    parseInput() -   function to parse line of file
    inputLine   -   line content
    req -   structure to store file data in structural and understanding way

*/
void parseInput(char *inputLine, struct reservationReq* req){
    char* tok;
    int c;
        
    tok = strtok(inputLine, " ");
    
    if(tok[0] == 'r' || tok[0] == 'R' ){
        //reqType
        (*req).reqType = 0;

        //name1
        tok = strtok(0, " ");
        strcpy((*req).passengerName, tok);
        
        
        strcat((*req).passengerName, " ");
        //name2
        tok = strtok(0, " ");
        strcat((*req).passengerName, tok);
        
        
        //Age
        tok = strtok(0, " ");
        //c = atoi(tok);
        strcpy((*req).age, tok);
        
        //sex
        tok = strtok(0, " ");
        (*req).sex = tok[0];

        //TrainId
        tok = strtok(0, " ");
        c = atoi(tok);
        (*req).trainId = c;
        
        //Class
        tok = strtok(0, " ");
        strcpy((*req).class, tok);
        if((*req).class[2]=='2'||(*req).class[2]=='3'){
            (*req).class[3]='\0';
        }
        else{
            (*req).class[2]='\0';
        }
        

    }
    else{
        (*req).reqType = 1;

        //pnrId
        tok = strtok(0, " ");
        c = atoi(tok);
        (*req).pnr = c;
    }
}




/*
    get_read_lock() -   Get the read lock on perticular Train

*/

void get_read_lock(int reqTrainID){
    P(z[reqTrainID]);
    P(mutexR[reqTrainID]);
    P(x[reqTrainID]);

    countRead[reqTrainID]++;
    if(countRead[reqTrainID] == 1){
        P(mutexW[reqTrainID]);
    }

    V(x[reqTrainID]);
    V(mutexR[reqTrainID]);
    V(z[reqTrainID]);        
}




/*
    release_read_lock() -   Release the read lock on perticular Train

*/

void release_read_lock(int reqTrainID){
    P(x[reqTrainID]);

    countRead[reqTrainID]--;
    if(countRead[reqTrainID] == 0){
        V(mutexW[reqTrainID]);
    }
    
    V(x[reqTrainID]);         
}



/*
    get_write_lock() -   Get the right lock on perticular Train

*/

void get_write_lock(int reqTrainID){
    P(y[reqTrainID]);

    countWrite[reqTrainID]++;
    if (countWrite[reqTrainID] == 1){ 
        P(mutexR[reqTrainID]);
    }
    
    V(y[reqTrainID]); 
    P(mutexW[reqTrainID]);
            
}



/*
    release_write_lock() -   Relase the right lock on perticular Train

*/

void release_write_lock(int reqTrainID){
    V(mutexW[reqTrainID]);
    P(y[reqTrainID]);

    countWrite[reqTrainID]--;
    if (countWrite[reqTrainID] == 0)
        V(mutexR[reqTrainID]);
    
    V(y[reqTrainID]);

}









