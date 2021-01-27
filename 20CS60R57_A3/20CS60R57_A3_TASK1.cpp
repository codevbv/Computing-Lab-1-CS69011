#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include<cmath>

#define MAX 5000
#define ECGMT 10.0
#define ECHOMT 20.0
#define TMMT 40.0
#define REGMT 5.0
#define CTMT 30.0
#define MRIMT 40.0


using namespace std;

struct custEvent{
    int cId;
    double arrivalTime = 0.0;
    double completionTime = 0.0;
    double time;
    int package;
    bool ECG = false;
    bool Echo = false;
    bool Treadmill = false;
    double interArrival;
    double interArrival2;
    double meanService;
    bool registered = false;
    bool serviced = false;
    bool semiServiced = false;
};

class BCRT
{
    public:
        BCRT()
        {
            EventQ = new custEvent[MAX];
            IndexLastEvent = -1;
            lastCustId = 0;
            for(int i = 0;i<6;i++){
                custP[i] = 0;
                custTotalTimeP[i] = 0.0;
            }
        }
        void InitEventQueue(custEvent *Init,int p,int q)
        {
            int n = q-p+1;
            lastCustId = n;
            for(int i =0 ;i<n;i++){
                EventQ[i] = Init[i];
                custP[i] = 1;
                IndexLastEvent++;
            }
            for(int i = n/2;i>=0;i--){
                MinHeapifyTop(i);
            }
        }
        void MinHeapifyTop(int k)
        {
            int left = 2*k+1;
            int right = 2*k +2;
            int n = IndexLastEvent+1;
            if((left<=n-1)&&(EventQ[k].time>EventQ[left].time)){
                custEvent temp = EventQ[left];
                EventQ[left] = EventQ[k];
                EventQ[k] = temp;
                MinHeapifyTop(left);
            }
            if((right<=n-1)&&EventQ[k].time>EventQ[right].time){
                custEvent temp = EventQ[right];
                EventQ[right] = EventQ[k];
                EventQ[k] = temp;
                MinHeapifyTop(right);
            }
        }
        void InsertInBCRT(custEvent newEvent)
        {
            EventQ[IndexLastEvent+1] = newEvent;
            MinHeapifyBottom(IndexLastEvent+1);
            IndexLastEvent++;
        }
        void MinHeapifyBottom(int k)
        {
            int parent = (k-1)/2;
            if((parent>=0)&&(EventQ[parent].time>EventQ[k].time)){
                custEvent temp = EventQ[parent];
                EventQ[parent] = EventQ[k];
                EventQ[k] = temp;
                MinHeapifyBottom(parent);
            }
        }
        custEvent pickEvent()
        {
            custEvent picked = EventQ[0];
            EventQ[0] = EventQ[IndexLastEvent];
            EventQ[IndexLastEvent].time = -1;
            IndexLastEvent--;
            MinHeapifyTop(0);
            return picked;
        }
        void printEventQueue(){
            //cout<<"Last index as of now is : "<<IndexLastEvent<<endl;
            for(int i =0;i<=IndexLastEvent;i++){
                cout<<EventQ[i].cId<<" with time "<<EventQ[i].time<<endl;;
            }
        }
        void setCustId(int id){
            lastCustId = id;
        }
        int getCustId(){
            return lastCustId;
        }
        void incrementCustP(int i){
            custP[i]+=1;
        }
        void printNumCustomerPack(){
            for(int i =0;i<6;i++){
                cout<<"The number of customers in package"<<i+1<<" is : "<<custP[i]<<endl;
            }
        }
        void addTotalTimeP(double total,int i){
            custTotalTimeP[i]+=total;
        }
        void printTotalTimePack(){
            for(int i =0;i<6;i++){
                cout<<"The total time taken by customers in package"<<i+1<<" is : "<<custTotalTimeP[i]<<endl;
            }
        }
        void printAverageTotalTimePack(){
            for(int i =0;i<6;i++){
                cout<<"The average total time taken by customers in package"<<i+1<<" is : "<<custTotalTimeP[i]/custP[i]<<endl;
            }
        }
        double getTotalTime(int i){
            return custTotalTimeP[i]/custP[i];
        }
    private:
        custEvent *EventQ;
        int IndexLastEvent,lastCustId,custP[6];
        double custTotalTimeP[6];
};

class Registration{
private:
    int frontQ, rearQ, capacityQ,IndexLastEvent,maxCountInQ;
    custEvent *REGQueue;
    double closeTime,serveTime,waitTime;
    bool busy;
public:
    Registration()
    {
        closeTime = 0.0;
        serveTime = 0.0;
        waitTime = 0.0;
        maxCountInQ = -1;
        IndexLastEvent = -1;
        busy = false;
        frontQ = rearQ = 0;
        capacityQ = MAX;
        REGQueue = new custEvent[MAX];
    }

    ~Registration()
    {
        delete[] REGQueue;
    }

    // function to insert an element
    // at the rear of the queue
    void AddToWait(custEvent Event)
    {
        // check queue is full or not
        if (capacityQ == rearQ) {
            cout<<"\nOpen list is  full\n";
            return;
        }

        // insert element at the rear
        else {
            REGQueue[rearQ] = Event;
            rearQ++;
            IndexLastEvent++;
            if(IndexLastEvent>maxCountInQ)
                maxCountInQ = IndexLastEvent;

        }
        return;
    }

    // Function to delete an element from the front of openList
    void RemoveFromWait()
    {
        // if queue is empty
        if (frontQ == rearQ) {
            cout<<"\nQueue is  empty"<<endl;
            return;
        }

        // shift all the elements from index 2 till rear
        // to the left by one
        else {
            for (int i = 0; i <rearQ - 1; i++) {
                REGQueue[i] = REGQueue[i + 1];
            }

            IndexLastEvent--;
            // decrement rear
            rearQ--;
        }
        return;
    }

    // Return first element of open list
    custEvent PickToRegister()
    {
        if (frontQ == rearQ) {
            cout<<"\nQueue is Empty"<<endl;
            return REGQueue[rearQ+1];
        }
        busy = true;
        return REGQueue[frontQ];
    }

    bool isEmpty(){
        if (frontQ == rearQ)
            return true;
        return false;
    }
    bool getStatus(){
        return busy;
    }
    void setStatus(bool stat){
        busy = stat;
    }
    void printREGQueue(){
        //cout<<"Last index as of now is : "<<IndexLastEvent<<endl;
        for(int i =0;i<=IndexLastEvent;i++){
            cout<<REGQueue[i].cId<<" with time "<<REGQueue[i].time<<" and package "<<REGQueue[i].package<<endl;;
        }
    }
    void updateCloseTime(double time){
        closeTime = time;
    }
    double getCloseTime(){
        return closeTime;
    }
    void addToServeTime(double time){
        serveTime+=time;
    }
    double getServeTime(){
        return serveTime;
    }
    void addToWaitTime(double time){
        waitTime+=time;
    }
    double getWaitTime(){
        return waitTime;
    }
    int getMaxLengthQ(){
        return maxCountInQ+1;
    }
};

class ECG{
private:
    int frontECG, rearECG, capacityECG,IndexLastEvent,maxCountInQ;
    custEvent *ECGQueue;
    bool busyECG;
    double closeTime,serveTime,waitTime;
public:
    ECG()
    {
        closeTime = 0.0;
        serveTime = 0.0;
        waitTime = 0.0;
        maxCountInQ = -1;
        IndexLastEvent = -1;
        busyECG = false;
        frontECG = rearECG = 0;
        capacityECG = MAX;
        ECGQueue = new custEvent[MAX];
    }

    ~ECG()
    {
        delete[] ECGQueue;
    }

    // function to insert an element
    // at the rear of the queue
    void AddToWaitECG(custEvent Event)
    {
        // check queue is full or not
        if (capacityECG == rearECG) {
            cout<<"\nOpen list is  full\n";
            return;
        }

        // insert element at the rear
        else {
            ECGQueue[rearECG] = Event;
            IndexLastEvent++;
            rearECG++;
            if(IndexLastEvent>maxCountInQ)
                maxCountInQ = IndexLastEvent;
        }
        return;
    }

    // Function to delete an element from the front of openList
    void RemoveFromWaitECG()
    {
        // if queue is empty
        if (frontECG == rearECG) {
            cout<<"\nQueue is  empty"<<endl;
            return;
        }

        // shift all the elements from index 2 till rear
        // to the left by one
        else {
            for (int i = 0; i <rearECG - 1; i++) {
                ECGQueue[i] = ECGQueue[i + 1];
            }
            IndexLastEvent--;
            // decrement rear
            rearECG--;
        }
        return;
    }

    // Return first element of open list
    custEvent PickToServeECG()
    {
        if (frontECG == rearECG) {
            cout<<"\nQueue is Empty"<<endl;
            return ECGQueue[rearECG+1];
        }
        busyECG = true;
        return ECGQueue[frontECG];
    }

    bool isEmptyECG(){
        if (frontECG == rearECG)
            return true;
        return false;
    }
    bool getStatusECG(){
        return busyECG;
    }
    void setStatusECG(bool stat){
        busyECG = stat;
    }
    void printECGQueue(){
        //cout<<"Last index as of now is : "<<IndexLastEvent<<endl;
        for(int i =0;i<=IndexLastEvent;i++){
            cout<<ECGQueue[i].cId<<" with time "<<ECGQueue[i].time<<endl;;
        }
    }
    void updateCloseTime(double time){
        closeTime = time;
    }
    double getCloseTime(){
        return closeTime;
    }
    void addToServeTime(double time){
        serveTime+=time;
    }
    double getServeTime(){
        return serveTime;
    }
    void addToWaitTime(double time){
        waitTime+=time;
    }
    double getWaitTime(){
        return waitTime;
    }
    int getMaxLengthQ(){
        return maxCountInQ+1;
    }
};

class Echocardiogram{
private:
    int frontECHO, rearECHO, capacityECHO,IndexLastEvent,maxCountInQ;
    custEvent *ECHOQueue;
    bool busyECHO;
    double closeTime,serveTime,waitTime;
public:
    Echocardiogram()
    {
        closeTime = 0.0;
        serveTime = 0.0;
        waitTime = 0.0;
        maxCountInQ = -1;
        IndexLastEvent = -1;
        busyECHO = false;
        frontECHO = rearECHO = 0;
        capacityECHO = MAX;
        ECHOQueue = new custEvent[MAX];
    }

    ~Echocardiogram()
    {
        delete[] ECHOQueue;
    }

    // function to insert an element
    // at the rear of the queue
    void AddToWaitECHO(custEvent Event)
    {
        // check queue is full or not
        if (capacityECHO == rearECHO) {
            cout<<"\nOpen list is  full\n";
            return;
        }

        // insert element at the rear
        else {
            ECHOQueue[rearECHO] = Event;
            IndexLastEvent++;
            rearECHO++;
            if(IndexLastEvent>maxCountInQ)
                maxCountInQ = IndexLastEvent;
        }
        return;
    }

    // Function to delete an element from the front of openList
    void RemoveFromWaitECHO()
    {
        // if queue is empty
        if (frontECHO == rearECHO) {
            cout<<"\nQueue is  empty"<<endl;
            return;
        }

        // shift all the elements from index 2 till rear
        // to the left by one
        else {
            for (int i = 0; i <rearECHO - 1; i++) {
                ECHOQueue[i] = ECHOQueue[i + 1];
            }
            IndexLastEvent--;
            // decrement rear
            rearECHO--;
        }
        return;
    }

    // Return first element of open list
    custEvent PickToServeECHO()
    {
        if (frontECHO == rearECHO) {
            cout<<"\nQueue is Empty"<<endl;
            return ECHOQueue[rearECHO+1];
        }
        busyECHO = true;
        return ECHOQueue[frontECHO];
    }

    bool isEmptyECHO(){
        if (frontECHO == rearECHO)
            return true;
        return false;
    }

    bool getStatusECHO(){
        return busyECHO;
    }
    void setStatusECHO(bool stat){
        busyECHO = stat;
    }
    void printECHOQueue(){
        //cout<<"Last index as of now is : "<<IndexLastEvent<<endl;
        for(int i =0;i<=IndexLastEvent;i++){
            cout<<ECHOQueue[i].cId<<" with time "<<ECHOQueue[i].time<<endl;;
        }
    }
    void updateCloseTime(double time){
        closeTime = time;
    }
    double getCloseTime(){
        return closeTime;
    }
    void addToServeTime(double time){
        serveTime+=time;
    }
    double getServeTime(){
        return serveTime;
    }
    void addToWaitTime(double time){
        waitTime+=time;
    }
    double getWaitTime(){
        return waitTime;
    }
    int getMaxLengthQ(){
        return maxCountInQ+1;
    }
};

class Treadmill{
private:
    int frontTM, rearTM, capacityTM,IndexLastEvent,maxCountInQ;
    custEvent *TMQueue;
    bool busyTM;
    double closeTime,serveTime,waitTime;
public:
    Treadmill()
    {
        closeTime = 0.0;
        serveTime = 0.0;
        waitTime = 0.0;
        maxCountInQ = -1;
        IndexLastEvent = -1;
        busyTM = false;
        frontTM = rearTM = 0;
        capacityTM = MAX;
        TMQueue = new custEvent[MAX];
    }

    ~Treadmill()
    {
        delete[] TMQueue;
    }

    // function to insert an element
    // at the rear of the queue
    void AddToWaitTM(custEvent Event)
    {
        // check queue is full or not
        if (capacityTM == rearTM) {
            cout<<"\nOpen list is  full\n";
            return;
        }

        // insert element at the rear
        else {
            TMQueue[rearTM] = Event;
            IndexLastEvent++;
            rearTM++;
            if(IndexLastEvent>maxCountInQ)
                maxCountInQ = IndexLastEvent;
        }
        return;
    }

    // Function to delete an element from the front of openList
    void RemoveFromWaitTM()
    {
        // if queue is empty
        if (frontTM == rearTM) {
            cout<<"\nQueue is  empty"<<endl;
            return;
        }

        // shift all the elements from index 2 till rear
        // to the left by one
        else {
            for (int i = 0; i <rearTM - 1; i++) {
                TMQueue[i] = TMQueue[i + 1];
            }
            IndexLastEvent--;
            // decrement rear
            rearTM--;
        }
        return;
    }

    // Return first element of open list
    custEvent PickToServeTM()
    {
        if (frontTM == rearTM) {
            cout<<"\nQueue is Empty"<<endl;
            return TMQueue[rearTM+1];
        }
        busyTM = true;
        return TMQueue[frontTM];
    }

    bool isEmptyTM(){
        if (frontTM == rearTM)
            return true;
        return false;
    }
    bool getStatusTM(){
        return busyTM;
    }
    void setStatusTM(bool stat){
        busyTM = stat;
    }
    void printTMQueue(){
        //cout<<"Last index as of now is : "<<IndexLastEvent<<endl;
        for(int i =0;i<=IndexLastEvent;i++){
            cout<<TMQueue[i].cId<<" with time "<<TMQueue[i].time<<endl;;
        }
    }
    void updateCloseTime(double time){
        closeTime = time;
    }
    double getCloseTime(){
        return closeTime;
    }
    void addToServeTime(double time){
        serveTime+=time;
    }
    double getServeTime(){
        return serveTime;
    }
    void addToWaitTime(double time){
        waitTime+=time;
    }
    double getWaitTime(){
        return waitTime;
    }
    int getMaxLengthQ(){
        return maxCountInQ+1;
    }
};

class MRI{
private:
    int frontMRI, rearMRI, capacityMRI,IndexLastEvent,maxCountInQ;
    custEvent *MRIQueue;
    bool busyMRI;
    double closeTime,serveTime,waitTime;
public:
    MRI()
    {
        closeTime = 0.0;
        serveTime = 0.0;
        waitTime = 0.0;
        maxCountInQ = -1;
        IndexLastEvent = -1;
        busyMRI = false;
        frontMRI = rearMRI = 0;
        capacityMRI = MAX;
        MRIQueue = new custEvent[MAX];
    }

    ~MRI()
    {
        delete[] MRIQueue;
    }

    // function to insert an element
    // at the rear of the queue
    void AddToWaitMRI(custEvent Event)
    {
        // check queue is full or not
        if (capacityMRI == rearMRI) {
            cout<<"\nOpen list is  full\n";
            return;
        }

        // insert element at the rear
        else {
            MRIQueue[rearMRI] = Event;
            IndexLastEvent++;
            rearMRI++;
            if(IndexLastEvent>maxCountInQ)
                maxCountInQ = IndexLastEvent;
        }
        return;
    }

    // Function to delete an element from the front of openList
    void RemoveFromWaitMRI()
    {
        // if queue is empty
        if (frontMRI == rearMRI) {
            cout<<"\nQueue is  empty"<<endl;
            return;
        }

        // shift all the elements from index 2 till rear
        // to the left by one
        else {
            for (int i = 0; i <rearMRI - 1; i++) {
                MRIQueue[i] = MRIQueue[i + 1];
            }
            IndexLastEvent--;
            // decrement rear
            rearMRI--;
        }
        return;
    }

    // Return first element of open list
    custEvent PickToServeMRI()
    {
        if (frontMRI == rearMRI) {
            cout<<"\nQueue is Empty"<<endl;
            return MRIQueue[rearMRI+1];
        }
        busyMRI = true;
        return MRIQueue[frontMRI];
    }

    bool isEmptyMRI(){
        if (frontMRI == rearMRI)
            return true;
        return false;
    }
    bool getStatusMRI(){
        return busyMRI;
    }
    void setStatusMRI(bool stat){
        busyMRI = stat;
    }
    void printMRIQueue(){
        //cout<<"Last index as of now is : "<<IndexLastEvent<<endl;
        for(int i =0;i<=IndexLastEvent;i++){
            cout<<MRIQueue[i].cId<<" with time "<<MRIQueue[i].time<<endl;;
        }
    }
    void updateCloseTime(double time){
        closeTime = time;
    }
    double getCloseTime(){
        return closeTime;
    }
    void addToServeTime(double time){
        serveTime+=time;
    }
    double getServeTime(){
        return serveTime;
    }
    void addToWaitTime(double time){
        waitTime+=time;
    }
    double getWaitTime(){
        return waitTime;
    }
    int getMaxLengthQ(){
        return maxCountInQ+1;
    }
};

class CT{
private:
    int frontCT, rearCT, capacityCT,IndexLastEvent,maxCountInQ;
    custEvent *CTQueue;
    bool busyCT;
    double closeTime,serveTime,waitTime;
public:
    CT()
    {
        closeTime = 0.0;
        serveTime = 0.0;
        waitTime = 0.0;
        maxCountInQ = -1;
        IndexLastEvent = -1;
        busyCT = false;
        frontCT = rearCT = 0;
        capacityCT = MAX;
        CTQueue = new custEvent[MAX];
    }

    ~CT()
    {
        delete[] CTQueue;
    }

    // function to insert an element
    // at the rear of the queue
    void AddToWaitCT(custEvent Event)
    {
        // check queue is full or not
        if (capacityCT == rearCT) {
            cout<<"\nOpen list is  full\n";
            return;
        }

        // insert element at the rear
        else {
            CTQueue[rearCT] = Event;
            IndexLastEvent++;
            rearCT++;
            if(IndexLastEvent>maxCountInQ)
                maxCountInQ = IndexLastEvent;
        }
        return;
    }

    // Function to delete an element from the front of openList
    void RemoveFromWaitCT()
    {
        // if queue is empty
        if (frontCT == rearCT) {
            cout<<"\nQueue is  empty"<<endl;
            return;
        }

        // shift all the elements from index 2 till rear
        // to the left by one
        else {
            for (int i = 0; i <rearCT - 1; i++) {
                CTQueue[i] = CTQueue[i + 1];
            }
            IndexLastEvent--;
            // decrement rear
            rearCT--;
        }
        return;
    }

    // Return first element of open list
    custEvent PickToServeCT()
    {
        if (frontCT == rearCT) {
            cout<<"\nQueue is Empty"<<endl;
            return CTQueue[rearCT+1];
        }
        busyCT = true;
        return CTQueue[frontCT];
    }

    bool isEmptyCT(){
        if (frontCT == rearCT)
            return true;
        return false;
    }

    bool getStatusCT(){
        return busyCT;
    }
    void setStatusCT(bool stat){
        busyCT = stat;
    }
    void printCTQueue(){
        //cout<<"Last index as of now is : "<<IndexLastEvent<<endl;
        for(int i =0;i<=IndexLastEvent;i++){
            cout<<CTQueue[i].cId<<" with time "<<CTQueue[i].time<<endl;;
        }
    }
    void updateCloseTime(double time){
        closeTime = time;
    }
    double getCloseTime(){
        return closeTime;
    }
    void addToServeTime(double time){
        serveTime+=time;
    }
    double getServeTime(){
        return serveTime;
    }
    void addToWaitTime(double time){
        waitTime+=time;
    }
    double getWaitTime(){
        return waitTime;
    }
    int getMaxLengthQ(){
        return maxCountInQ+1;
    }
};

//Prototyping
void InitArrival(custEvent *Init,double *L,int InitLength);
double generateInterArrival(double tInit,double l);
double getServiceTime(double tInit,double l);
int findMin(double *util,int n);
int findMax(double *util,int n);

//Takes input file name from command line arg
int main(int argc,char *argv[])
{
    double AvgTotalTime[6] = {0.0,0.0,0.0,0.0,0.0,0.0};
    double AvgUtil[6] = {0.0,0.0,0.0,0.0,0.0,0.0};
    double AvgCloseTime[6] = {0.0,0.0,0.0,0.0,0.0,0.0};
    double AvgWait[6] = {0.0,0.0,0.0,0.0,0.0,0.0};

    int countDays = 0;
    int sequence = 7 ;
    /*cout<<"Enter any one of the sequence from below : "<<endl;
    cout<<"1. P1 : ECG->ECHO->TM ; P6 : ECG->ECHO"<<endl;
    cout<<"2. P1 : ECG->TM->ECHO ; P6 : ECG->ECHO"<<endl;
    cout<<"3. P1 : ECHO->ECG->TM ; P6 : ECG->ECHO"<<endl;
    cout<<"4. P1 : ECHO->TM->ECG ; P6 : ECG->ECHO"<<endl;
    cout<<"5. P1 : TM->ECHO->ECG ; P6 : ECG->ECHO"<<endl;
    cout<<"6. P1 : TM->ECG->ECHO ; P6 : ECG->ECHO"<<endl;
    cout<<"7. P1 : ECG->ECHO->TM ; P6 : ECHO->ECG"<<endl;
    cout<<"8. P1 : ECG->TM->ECHO ; P6 : ECHO->ECG"<<endl;
    cout<<"9. P1 : ECHO->ECG->TM ; P6 : ECHO->ECG"<<endl;
    cout<<"10. P1 : ECHO->TM->ECG ; P6 : ECHO->ECG"<<endl;
    cout<<"11. P1 : TM->ECHO->ECG ; P6 : ECHO->ECG"<<endl;
    cout<<"12. P1 : TM->ECG->ECHO ; P6 : ECHO->ECG"<<endl;
    cin>>sequence;*/

    //Loop to gather data for 10 days
    for(int i = 0;i<10;i++){

        cout<<"Enter the inter arrival times for Day"<<i+1<<endl;
        //TO store inter arrival times given as input
        double L[5];
        int minL = 0;
        if(argc==1){
            for(int i =0;i<5;i++){
                cout<<"Enter lambda"<<i+1<<" : "<<endl;
                cin>>L[i];
                if(L[i]<minL)
                    minL = L[i];
            }
        }else{
            for(int i =0;i<5;i++){
                L[i] = atof(argv[i+1]);
                if(L[i]<minL)
                    minL = L[i];
            }
        }

        //If at any point lambda entered is negative then break
        if(minL<0)
           break;

        //Count number of days for which simulation  has run
        countDays++;

        //Initialize the first set of customers
        custEvent *Init = new custEvent[6];
        InitArrival(Init,L,6);
        BCRT BCRT1;

        //Initialize the event queue
        BCRT1.InitEventQueue(Init,0,5);

        //Open all the facilities.
        Registration reg;
        ECG ecg;
        Echocardiogram echo;
        MRI mri;
        Treadmill tm;
        CT ct;

        while(true){
            int regtype;
            custEvent event = BCRT1.pickEvent();
            double timeCur = event.time;

            //If there are no more events then break
            if(timeCur==-1)
                break;

            //Check if event is registered or not
            if(!event.registered)
                regtype = 1;
            else if(event.registered)
                regtype = 2;
            switch(regtype){
            case 1 :
                {
                    int packtype = event.package;
                    event.arrivalTime = event.time;
                    switch(packtype){
                    case 1:
                        {
                            if(timeCur<=300.0){
                                custEvent newEvent;
                                newEvent.cId = BCRT1.getCustId()+1;
                                newEvent.package = event.package;
                                newEvent.interArrival = event.interArrival;
                                newEvent.meanService = event.meanService;
                                newEvent.time = generateInterArrival(event.time,event.interArrival);
                                BCRT1.setCustId(newEvent.cId);
                                BCRT1.InsertInBCRT(newEvent);
                                BCRT1.incrementCustP(packtype-1);

                                //If the Registration counter is free
                                if(!reg.getStatus()){
                                    event.registered = true;
                                    event.time = getServiceTime(event.time,REGMT);
                                    //cout<<"Customer"<<event.cId<<" was scheduled for registration successfully at "<<timeCur<<" will be registered at "<<event.time<<endl;
                                    reg.setStatus(true);
                                    reg.addToServeTime(event.time-timeCur);
                                    BCRT1.InsertInBCRT(event);
                                }

                                //If the Registration counter is busy add the event in wait queue of registration
                                else if(reg.getStatus()){
                                    reg.AddToWait(event);
                                    //cout<<"Customer"<<event.cId<<" was added to the wait queue of registration facility at time "<<timeCur<<endl;
                                }
                            }
                        }
                        break;
                    case 2:
                        {
                            if(timeCur<=540.0){
                                custEvent newEvent;
                                newEvent.cId = BCRT1.getCustId()+1;
                                newEvent.package = event.package;
                                newEvent.interArrival = event.interArrival;
                                newEvent.meanService = event.meanService;
                                newEvent.time = generateInterArrival(event.time,event.interArrival);
                                BCRT1.setCustId(newEvent.cId);
                                BCRT1.InsertInBCRT(newEvent);
                                BCRT1.incrementCustP(packtype-1);

                                //If the Registration counter is free
                                if(!reg.getStatus()){
                                    event.registered = true;
                                    event.time = getServiceTime(event.time,REGMT);
                                    //cout<<"Customer"<<event.cId<<" was scheduled for registration successfully at "<<timeCur<<" will be registered at "<<event.time<<endl;
                                    reg.setStatus(true);
                                    reg.addToServeTime(event.time-timeCur);
                                    BCRT1.InsertInBCRT(event);
                                }

                                //If the Registration counter is busy add the event in wait queue of registration
                                else if(reg.getStatus()){
                                    reg.AddToWait(event);
                                    //cout<<"Customer"<<event.cId<<" was added to the wait queue of registration facility at time "<<timeCur<<endl;
                                }
                            }
                        }
                        break;
                    case 3:
                        {
                            if(timeCur<=540.0){
                                custEvent newEvent;
                                newEvent.cId = BCRT1.getCustId()+1;
                                newEvent.package = event.package;
                                newEvent.interArrival = event.interArrival;
                                newEvent.meanService = event.meanService;
                                newEvent.time = generateInterArrival(event.time,event.interArrival);
                                BCRT1.setCustId(newEvent.cId);
                                BCRT1.InsertInBCRT(newEvent);
                                BCRT1.incrementCustP(packtype-1);

                                //If the Registration counter is free
                                if(!reg.getStatus()){
                                    event.registered = true;
                                    event.time = getServiceTime(event.time,REGMT);
                                    //cout<<"Customer"<<event.cId<<" was scheduled for registration successfully at "<<timeCur<<" will be registered at "<<event.time<<endl;
                                    reg.setStatus(true);
                                    reg.addToServeTime(event.time-timeCur);
                                    BCRT1.InsertInBCRT(event);
                                }

                                //If the Registration counter is busy add the event in wait queue of registration
                                else if(reg.getStatus()){
                                    reg.AddToWait(event);
                                    //cout<<"Customer"<<event.cId<<" was added to the wait queue of registration facility at time "<<timeCur<<endl;
                                }
                            }
                        }
                        break;
                    case 4:
                        {
                            if(timeCur<=660.0){
                                custEvent newEvent;
                                newEvent.cId = BCRT1.getCustId()+1;
                                newEvent.package = event.package;
                                newEvent.interArrival = event.interArrival;
                                newEvent.interArrival2 = event.interArrival2;
                                newEvent.meanService = event.meanService;
                                if(timeCur<=360.0)
                                    newEvent.time = generateInterArrival(event.time,event.interArrival);
                                else
                                    newEvent.time = generateInterArrival(event.time,event.interArrival2);
                                BCRT1.setCustId(newEvent.cId);
                                BCRT1.InsertInBCRT(newEvent);
                                BCRT1.incrementCustP(packtype-1);

                                //If the Registration counter is free
                                if(!reg.getStatus()){
                                    event.registered = true;
                                    event.time = getServiceTime(event.time,REGMT);
                                    //cout<<"Customer"<<event.cId<<" was scheduled for registration successfully at "<<timeCur<<" will be registered at "<<event.time<<endl;
                                    reg.setStatus(true);
                                    reg.addToServeTime(event.time-timeCur);
                                    BCRT1.InsertInBCRT(event);
                                }

                                //If the Registration counter is busy add the event in wait queue of registration
                                else if(reg.getStatus()){
                                    reg.AddToWait(event);
                                    //cout<<"Customer"<<event.cId<<" was added to the wait queue of registration facility at time "<<timeCur<<endl;
                                }
                            }
                        }
                        break;
                    case 5:
                        {
                            if(timeCur<=540.0){
                                custEvent newEvent;
                                newEvent.cId = BCRT1.getCustId()+1;
                                newEvent.package = event.package;
                                newEvent.interArrival = event.interArrival;
                                newEvent.meanService = event.meanService;
                                newEvent.time = generateInterArrival(event.time,event.interArrival);
                                BCRT1.setCustId(newEvent.cId);
                                BCRT1.InsertInBCRT(newEvent);
                                BCRT1.incrementCustP(packtype-1);

                                //If the Registration counter is free
                                if(!reg.getStatus()){
                                    event.registered = true;
                                    event.time = getServiceTime(event.time,REGMT);
                                    //cout<<"Customer"<<event.cId<<" was scheduled for registration successfully at "<<timeCur<<" will be registered at "<<event.time<<endl;
                                    reg.setStatus(true);
                                    reg.addToServeTime(event.time-timeCur);
                                    BCRT1.InsertInBCRT(event);
                                }

                                //If the Registration counter is busy add the event in wait queue of registration
                                else if(reg.getStatus()){
                                    reg.AddToWait(event);
                                    //cout<<"Customer"<<event.cId<<" was added to the wait queue of registration facility at time "<<timeCur<<endl;
                                }
                            }
                        }
                        break;
                    case 6:
                        {
                            if(timeCur<=540.0){
                                custEvent newEvent;
                                newEvent.cId = BCRT1.getCustId()+1;
                                newEvent.package = event.package;
                                newEvent.interArrival = event.interArrival;
                                newEvent.meanService = event.meanService;
                                newEvent.time = generateInterArrival(event.time,event.interArrival);
                                BCRT1.setCustId(newEvent.cId);
                                BCRT1.InsertInBCRT(newEvent);
                                BCRT1.incrementCustP(packtype-1);

                                //If the Registration counter is free
                                if(!reg.getStatus()){
                                    event.registered = true;
                                    event.time = getServiceTime(event.time,REGMT);
                                    //cout<<"Customer"<<event.cId<<" was scheduled for registration successfully at "<<timeCur<<" will be registered at "<<event.time<<endl;
                                    reg.setStatus(true);
                                    reg.addToServeTime(event.time-timeCur);
                                    BCRT1.InsertInBCRT(event);
                                }

                                //If the Registration counter is busy add the event in wait queue of registration
                                else if(reg.getStatus()){
                                    reg.AddToWait(event);
                                    //cout<<"Customer"<<event.cId<<" was added to the wait queue of registration facility at time "<<timeCur<<endl;
                                }
                            }
                        }
                        break;

                    }
                }
                break;
            case 2:
                {
                    //The current event has been registered successfully
                    if(event.registered&&!event.serviced&&!event.semiServiced){
                        if(reg.getCloseTime()<event.time)
                            reg.updateCloseTime(event.time);
                        //cout<<"Customer"<<event.cId<<" was registered successfully at "<<timeCur<<endl;

                        //If there are events waiting in registration queue then pick one and service
                        if(!reg.isEmpty()){
                            custEvent eventNext = reg.PickToRegister();
                            reg.RemoveFromWait();
                            eventNext.registered = true;
                            reg.addToWaitTime(timeCur - eventNext.time);
                            eventNext.time = getServiceTime(timeCur,REGMT);
                            reg.addToServeTime(eventNext.time-timeCur);
                            //cout<<"Customer"<<eventNext.cId<<" was scheduled for registration successfully at "<<timeCur<<" will be registered at "<<eventNext.time<<endl;
                            reg.setStatus(true);
                            BCRT1.InsertInBCRT(eventNext);
                        }

                        //If there is no event in registration queue then set status registration counter to free.
                        else
                        {
                            reg.setStatus(false);
                        }
                    }
                    //Service the registered event as per the package
                    int packType = event.package;
                    int serveType;
                    if(event.semiServiced)
                        serveType = 3;
                    else if(event.serviced)
                        serveType = 2;
                    else if(!event.serviced)
                        serveType = 1;
                    switch(serveType)
                        {
                        case 2:
                            {
                                event.completionTime = timeCur;
                                double totalTime = event.completionTime- event.arrivalTime;
                                BCRT1.addTotalTimeP(totalTime,packType-1);
                                switch(packType)
                                {

                                    case 2:
                                        {
                                            if(mri.getCloseTime()<timeCur)
                                                mri.updateCloseTime(timeCur);
                                            //cout<<"Customer"<<event.cId<<" was serviced successfully at "<<timeCur<<endl;

                                            //If there are events waiting in registration queue then pick one and service
                                            if(!mri.isEmptyMRI()){
                                                custEvent eventNext = mri.PickToServeMRI();
                                                mri.RemoveFromWaitMRI();
                                                eventNext.serviced = true;
                                                mri.addToWaitTime(timeCur-eventNext.time);
                                                eventNext.time = getServiceTime(timeCur,eventNext.meanService);
                                                //cout<<"Customer"<<eventNext.cId<<" was sent for MRI successfully at "<<timeCur<<" will be served at "<<eventNext.time<<endl;
                                                mri.addToServeTime(eventNext.time- timeCur);
                                                mri.setStatusMRI(true);
                                                BCRT1.InsertInBCRT(eventNext);
                                            }

                                            //If there is no event in registration queue then set status registration counter to free.
                                            else
                                            {
                                                mri.setStatusMRI(false);
                                            }
                                            break;
                                        }
                                    case 3:
                                        {
                                            if(ct.getCloseTime()<timeCur)
                                                ct.updateCloseTime(timeCur);
                                            //cout<<"Customer"<<event.cId<<" was serviced successfully at "<<timeCur<<endl;

                                            //If there are events waiting in registration queue then pick one and service
                                            if(!ct.isEmptyCT()){
                                                custEvent eventNext = ct.PickToServeCT();
                                                ct.RemoveFromWaitCT();
                                                eventNext.serviced = true;
                                                ct.addToWaitTime(timeCur-eventNext.time);
                                                eventNext.time = getServiceTime(timeCur,eventNext.meanService);
                                                ct.addToServeTime(eventNext.time-timeCur);
                                                //cout<<"Customer"<<eventNext.cId<<" was sent for CT successfully at "<<timeCur<<" will be served at "<<eventNext.time<<endl;
                                                ct.setStatusCT(true);
                                                BCRT1.InsertInBCRT(eventNext);
                                            }

                                            //If there is no event in registration queue then set status registration counter to free.
                                            else
                                            {
                                                ct.setStatusCT(false);
                                            }
                                            break;
                                        }
                                    case 4:
                                        {
                                            if(ecg.getCloseTime()<timeCur)
                                                ecg.updateCloseTime(timeCur);
                                            //cout<<"Customer"<<event.cId<<" was serviced successfully at "<<timeCur<<endl;

                                            //If there are events waiting in registration queue then pick one and service
                                            if(!ecg.isEmptyECG()){
                                                custEvent eventNext = ecg.PickToServeECG();
                                                ecg.RemoveFromWaitECG();
                                                if(eventNext.package==1||eventNext.package==6){
                                                    eventNext.semiServiced= true;
                                                    eventNext.ECG = true;
                                                }
                                                else
                                                    eventNext.serviced= true;
                                                ecg.addToWaitTime(timeCur-eventNext.time);
                                                eventNext.time = getServiceTime(timeCur,ECGMT);
                                                ecg.addToServeTime(eventNext.time-timeCur);
                                                //cout<<"Customer"<<eventNext.cId<<" was sent for ECG successfully at "<<timeCur<<" will be served at "<<eventNext.time<<endl;
                                                ecg.setStatusECG(true);
                                                BCRT1.InsertInBCRT(eventNext);
                                            }

                                            //If there is no event in registration queue then set status registration counter to free.
                                            else
                                            {
                                                ecg.setStatusECG(false);
                                            }
                                            break;
                                        }
                                    case 5:
                                        {
                                            if(echo.getCloseTime()<timeCur)
                                                echo.updateCloseTime(timeCur);
                                            //cout<<"Customer"<<event.cId<<" was serviced successfully at "<<timeCur<<endl;

                                            //If there are events waiting in registration queue then pick one and service
                                            if(!echo.isEmptyECHO()){
                                                custEvent eventNext = echo.PickToServeECHO();
                                                echo.RemoveFromWaitECHO();
                                                if(eventNext.package==1||eventNext.package==6){
                                                    eventNext.semiServiced= true;
                                                    eventNext.Echo = true;
                                                }
                                                else
                                                    eventNext.serviced= true;
                                                echo.addToWaitTime(timeCur-eventNext.time);
                                                eventNext.time = getServiceTime(timeCur,ECHOMT);
                                                echo.addToServeTime(eventNext.time-timeCur);
                                                //cout<<"Customer"<<eventNext.cId<<" was sent for ECHO successfully at "<<timeCur<<" will be served at "<<eventNext.time<<endl;
                                                echo.setStatusECHO(true);
                                                BCRT1.InsertInBCRT(eventNext);
                                            }

                                            //If there is no event in registration queue then set status registration counter to free.
                                            else
                                            {
                                                echo.setStatusECHO(false);
                                            }
                                            break;
                                        }

                                }

                            }
                            break;
                        case 1:
                            {
                                switch(packType)
                                {
                                    case 1:
                                        {
                                            if(sequence==1||sequence==2||sequence==7||sequence==8){
                                                    if(!event.ECG){
                                                        if(!ecg.getStatusECG()){
                                                            event.ECG = true;
                                                            event.time = getServiceTime(event.time,ECGMT);
                                                            event.semiServiced = true;
                                                            //cout<<"Customer"<<event.cId<<" was sent for ECG  successfully at "<<timeCur<<" will be serviced at "<<event.time<<endl;
                                                            ecg.setStatusECG(true);
                                                            ecg.addToServeTime(event.time-timeCur);
                                                            BCRT1.InsertInBCRT(event);
                                                        }
                                                        else if(ecg.getStatusECG()){
                                                            ecg.AddToWaitECG(event);
                                                            //cout<<"Customer"<<event.cId<<" was added to the wait queue of ECG facility at time "<<timeCur<<endl;
                                                        }
                                                    }
                                                }else if(sequence==3||sequence==4||sequence==9||sequence==10){
                                                    if(!event.Echo){
                                                        if(!echo.getStatusECHO()){
                                                            event.Echo = true;
                                                            event.time = getServiceTime(event.time,ECHOMT);
                                                            event.semiServiced = true;
                                                            //cout<<"Customer"<<event.cId<<" was sent for ECG  successfully at "<<timeCur<<" will be serviced at "<<event.time<<endl;
                                                            echo.setStatusECHO(true);
                                                            echo.addToServeTime(event.time-timeCur);
                                                            BCRT1.InsertInBCRT(event);
                                                        }
                                                        else if(echo.getStatusECHO()){
                                                            echo.AddToWaitECHO(event);
                                                            //cout<<"Customer"<<event.cId<<" was added to the wait queue of ECG facility at time "<<timeCur<<endl;
                                                        }
                                                    }
                                                }else if(sequence==5||sequence==6||sequence==11||sequence==12){
                                                    if(!event.Treadmill){
                                                        if(!tm.getStatusTM()){
                                                            event.Treadmill = true;
                                                            event.time = getServiceTime(event.time,TMMT);
                                                            event.semiServiced = true;
                                                            //cout<<"Customer"<<event.cId<<" was sent for ECG  successfully at "<<timeCur<<" will be serviced at "<<event.time<<endl;
                                                            tm.setStatusTM(true);
                                                            tm.addToServeTime(event.time-timeCur);
                                                            BCRT1.InsertInBCRT(event);
                                                        }
                                                        else if(tm.getStatusTM()){
                                                            tm.AddToWaitTM(event);
                                                            //cout<<"Customer"<<event.cId<<" was added to the wait queue of ECG facility at time "<<timeCur<<endl;
                                                        }
                                                    }
                                                }
                                        }
                                        break;
                                    case 2:
                                        {
                                            if(!mri.getStatusMRI()){
                                                event.serviced = true;
                                                event.time = getServiceTime(event.time,event.meanService);
                                                //cout<<"Customer"<<event.cId<<" was sent for MRI successfully at "<<timeCur<<" will be serviced at "<<event.time<<endl;
                                                mri.addToServeTime(event.time-timeCur);
                                                mri.setStatusMRI(true);
                                                BCRT1.InsertInBCRT(event);
                                            }
                                            else if(mri.getStatusMRI()){
                                                mri.AddToWaitMRI(event);
                                                //cout<<"Customer"<<event.cId<<" was added to the wait queue of MRI facility at time "<<timeCur<<endl;
                                            }
                                        }
                                        break;
                                    case 3:
                                        {
                                            if(!ct.getStatusCT()){
                                                event.serviced = true;
                                                event.time = getServiceTime(event.time,event.meanService);
                                                ct.addToServeTime(event.time-timeCur);
                                                //cout<<"Customer"<<event.cId<<" was sent for CT successfully at "<<timeCur<<" will be serviced at "<<event.time<<endl;
                                                ct.setStatusCT(true);
                                                BCRT1.InsertInBCRT(event);
                                            }
                                            else if(ct.getStatusCT()){
                                                ct.AddToWaitCT(event);
                                                //cout<<"Customer"<<event.cId<<" was added to the wait queue of CT facility at time "<<timeCur<<endl;
                                            }
                                        }
                                        break;
                                    case 4:
                                        {
                                            if(!ecg.getStatusECG()){
                                                event.serviced = true;
                                                event.time = getServiceTime(event.time,event.meanService);
                                                //cout<<"Customer"<<event.cId<<" was sent for ECG successfully at "<<timeCur<<" will be serviced at "<<event.time<<endl;
                                                ecg.setStatusECG(true);
                                                ecg.addToServeTime(event.time-timeCur);
                                                BCRT1.InsertInBCRT(event);
                                            }
                                            else if(ecg.getStatusECG()){
                                                ecg.AddToWaitECG(event);
                                                //cout<<"Customer"<<event.cId<<" was added to the wait queue of ECG facility at time "<<timeCur<<endl;
                                            }
                                        }
                                        break;
                                    case 5:
                                        {
                                            if(!echo.getStatusECHO()){
                                                event.serviced = true;
                                                event.time = getServiceTime(event.time,event.meanService);
                                                echo.addToServeTime(event.time-timeCur);
                                                //cout<<"Customer"<<event.cId<<" was sent for ECHO successfully at "<<timeCur<<" will be serviced at "<<event.time<<endl;
                                                echo.setStatusECHO(true);
                                                BCRT1.InsertInBCRT(event);
                                            }
                                            else if(echo.getStatusECHO()){
                                                echo.AddToWaitECHO(event);
                                                //cout<<"Customer"<<event.cId<<" was added to the wait queue of ECHO facility at time "<<timeCur<<endl;
                                            }
                                        }
                                        break;
                                    case 6:
                                        {
                                                if(sequence==1||sequence==2||sequence==3||sequence==4||sequence==5||sequence==6){
                                                    if(!event.ECG){
                                                        if(!ecg.getStatusECG()){
                                                            event.ECG = true;
                                                            event.time = getServiceTime(event.time,ECGMT);
                                                            event.semiServiced = true;
                                                            //cout<<"Customer"<<event.cId<<" was sent for ECG  successfully at "<<timeCur<<" will be serviced at "<<event.time<<endl;
                                                            ecg.setStatusECG(true);
                                                            ecg.addToServeTime(event.time-timeCur);
                                                            BCRT1.InsertInBCRT(event);
                                                        }
                                                        else if(ecg.getStatusECG()){
                                                            ecg.AddToWaitECG(event);
                                                            //cout<<"Customer"<<event.cId<<" was added to the wait queue of ECG facility at time "<<timeCur<<endl;
                                                        }
                                                    }
                                                }else if(sequence==7||sequence==8||sequence==9||sequence==10||sequence==11||sequence==12){
                                                    if(!event.Echo){
                                                        if(!echo.getStatusECHO()){
                                                            event.Echo = true;
                                                            event.time = getServiceTime(event.time,ECHOMT);
                                                            event.semiServiced = true;
                                                            //cout<<"Customer"<<event.cId<<" was sent for ECG  successfully at "<<timeCur<<" will be serviced at "<<event.time<<endl;
                                                            echo.setStatusECHO(true);
                                                            echo.addToServeTime(event.time-timeCur);
                                                            BCRT1.InsertInBCRT(event);
                                                        }
                                                        else if(echo.getStatusECHO()){
                                                            echo.AddToWaitECHO(event);
                                                            //cout<<"Customer"<<event.cId<<" was added to the wait queue of ECG facility at time "<<timeCur<<endl;
                                                        }
                                                    }
                                                }

                                        }
                                        break;
                                }
                            }
                            break;
                        case 3:
                            {
                                switch(packType){
                                case 1:
                                    {
                                        if(sequence==1||sequence==7){
                                            if(event.ECG&&event.Echo&&event.Treadmill){
                                                if(tm.getCloseTime()<timeCur)
                                                    tm.updateCloseTime(timeCur);
                                                event.completionTime = event.time;
                                                double totalTime = event.completionTime- event.arrivalTime;
                                                BCRT1.addTotalTimeP(totalTime,packType-1);
                                                //cout<<"Customer"<<event.cId<<" was serviced successfully at time "<<event.time<<endl;
                                                if(!tm.isEmptyTM()){
                                                    custEvent eventNext = tm.PickToServeTM();
                                                    tm.RemoveFromWaitTM();
                                                    tm.addToWaitTime(timeCur-eventNext.time);
                                                    eventNext.time = getServiceTime(timeCur,TMMT);
                                                    eventNext.Treadmill = true;
                                                    tm.addToServeTime(eventNext.time-timeCur);
                                                    //cout<<"Customer"<<eventNext.cId<<" was sent for treadmill successfully at "<<timeCur<<" will be serviced at "<<eventNext.time<<endl;
                                                    tm.setStatusTM(true);
                                                    BCRT1.InsertInBCRT(eventNext);
                                                }
                                                //If there is no event in registration queue then set status registration counter to free.
                                                else
                                                {
                                                    tm.setStatusTM(false);
                                                }
                                            }
                                            else if(event.ECG&&event.Echo&&!event.Treadmill){
                                                if(!tm.getStatusTM()){
                                                    event.Treadmill= true;
                                                    event.time = getServiceTime(event.time,TMMT);
                                                    tm.addToServeTime(event.time-timeCur);
                                                    //cout<<"Customer"<<event.cId<<" was sent for Treadmill successfully at "<<timeCur<<" will be serviced at "<<event.time<<endl;
                                                    tm.setStatusTM(true);
                                                    BCRT1.InsertInBCRT(event);
                                                }else if(tm.getStatusTM()){
                                                    tm.AddToWaitTM(event);
                                                    //cout<<"Customer"<<event.cId<<" was added to the wait queue of TM facility at time "<<timeCur<<endl;
                                                }
                                                if(!echo.isEmptyECHO()){
                                                    custEvent eventNext = echo.PickToServeECHO();
                                                    echo.RemoveFromWaitECHO();
                                                    if(eventNext.package==1||eventNext.package==6){
                                                        eventNext.semiServiced = true;
                                                        eventNext.Echo = true;
                                                    }
                                                    else
                                                        eventNext.serviced = true;
                                                    echo.addToWaitTime(timeCur-eventNext.time);
                                                    eventNext.time = getServiceTime(timeCur,ECHOMT);
                                                    echo.addToServeTime(eventNext.time-timeCur);
                                                    //cout<<"Customer"<<eventNext.cId<<" was sent for echo successfully at "<<timeCur<<" will be serviced at "<<eventNext.time<<endl;
                                                    echo.setStatusECHO(true);
                                                    BCRT1.InsertInBCRT(eventNext);
                                                }
                                                //If there is no event in registration queue then set status registration counter to free.
                                                else
                                                {
                                                    echo.setStatusECHO(false);
                                                }
                                            }
                                            else if(event.ECG&&!event.Echo&&!event.Treadmill){
                                                if(!echo.getStatusECHO()){
                                                    event.Echo = true;
                                                    event.time = getServiceTime(event.time,ECHOMT);
                                                    echo.addToServeTime(event.time-timeCur);
                                                    //cout<<"Customer"<<event.cId<<" was sent for echo after ECG successfully at "<<timeCur<<" will be serviced at "<<event.time<<endl;
                                                    echo.setStatusECHO(true);
                                                    BCRT1.InsertInBCRT(event);
                                                }else if(echo.getStatusECHO()){
                                                    echo.AddToWaitECHO(event);
                                                    //cout<<"Customer"<<event.cId<<" was added to the wait queue of echo after ECG facility at time "<<timeCur<<endl;
                                                }
                                                if(!ecg.isEmptyECG()){
                                                    custEvent eventNext = ecg.PickToServeECG();
                                                    ecg.RemoveFromWaitECG();
                                                    if(eventNext.package==1||eventNext.package==6){
                                                        eventNext.semiServiced = true;
                                                        eventNext.ECG = true;
                                                    }
                                                    else
                                                        eventNext.serviced = true;
                                                    ecg.addToWaitTime(timeCur-eventNext.time);
                                                    eventNext.time = getServiceTime(timeCur,ECGMT);
                                                    ecg.addToServeTime(eventNext.time-timeCur);
                                                    //cout<<"Customer"<<eventNext.cId<<" was sent for ECG successfully at "<<timeCur<<" will be serviced at "<<eventNext.time<<endl;
                                                    ecg.setStatusECG(true);
                                                    BCRT1.InsertInBCRT(eventNext);
                                                }
                                                //If there is no event in registration queue then set status registration counter to free.
                                                else
                                                {
                                                    ecg.setStatusECG(false);
                                                }
                                            }
                                        }else if(sequence==2||sequence==8){
                                            if(event.ECG&&event.Treadmill&&event.Echo){
                                                if(echo.getCloseTime()<timeCur)
                                                    echo.updateCloseTime(timeCur);
                                                event.completionTime = event.time;
                                                double totalTime = event.completionTime- event.arrivalTime;
                                                BCRT1.addTotalTimeP(totalTime,packType-1);
                                                //cout<<"Customer"<<event.cId<<" was serviced successfully at time "<<event.time<<endl;
                                                if(!echo.isEmptyECHO()){
                                                    custEvent eventNext = echo.PickToServeECHO();
                                                    echo.RemoveFromWaitECHO();
                                                    echo.addToWaitTime(timeCur-eventNext.time);
                                                    eventNext.time = getServiceTime(timeCur,ECHOMT);
                                                    eventNext.Echo = true;
                                                    echo.addToServeTime(eventNext.time-timeCur);
                                                    //cout<<"Customer"<<eventNext.cId<<" was sent for treadmill successfully at "<<timeCur<<" will be serviced at "<<eventNext.time<<endl;
                                                    echo.setStatusECHO(true);
                                                    BCRT1.InsertInBCRT(eventNext);
                                                }
                                                //If there is no event in registration queue then set status registration counter to free.
                                                else
                                                {
                                                    echo.setStatusECHO(false);
                                                }
                                            }
                                            else if(event.ECG&&event.Treadmill&&!event.Echo){
                                                if(!echo.getStatusECHO()){
                                                    event.Echo= true;
                                                    event.time = getServiceTime(event.time,ECHOMT);
                                                    echo.addToServeTime(event.time-timeCur);
                                                    //cout<<"Customer"<<event.cId<<" was sent for Treadmill successfully at "<<timeCur<<" will be serviced at "<<event.time<<endl;
                                                    echo.setStatusECHO(true);
                                                    BCRT1.InsertInBCRT(event);
                                                }else if(echo.getStatusECHO()){
                                                    echo.AddToWaitECHO(event);
                                                    //cout<<"Customer"<<event.cId<<" was added to the wait queue of TM facility at time "<<timeCur<<endl;
                                                }
                                                if(!tm.isEmptyTM()){
                                                    custEvent eventNext = tm.PickToServeTM();
                                                    tm.RemoveFromWaitTM();
                                                    if(eventNext.package==1||eventNext.package==6){
                                                        eventNext.semiServiced = true;
                                                        eventNext.Treadmill = true;
                                                    }
                                                    else
                                                        eventNext.serviced = true;
                                                    tm.addToWaitTime(timeCur-eventNext.time);
                                                    eventNext.time = getServiceTime(timeCur,TMMT);
                                                    tm.addToServeTime(eventNext.time-timeCur);
                                                    //cout<<"Customer"<<eventNext.cId<<" was sent for echo successfully at "<<timeCur<<" will be serviced at "<<eventNext.time<<endl;
                                                    tm.setStatusTM(true);
                                                    BCRT1.InsertInBCRT(eventNext);
                                                }
                                                //If there is no event in registration queue then set status registration counter to free.
                                                else
                                                {
                                                    tm.setStatusTM(false);
                                                }
                                            }
                                            else if(event.ECG&&!event.Treadmill&&!event.Echo){
                                                if(!tm.getStatusTM()){
                                                    event.Treadmill = true;
                                                    event.time = getServiceTime(event.time,TMMT);
                                                    tm.addToServeTime(event.time-timeCur);
                                                    //cout<<"Customer"<<event.cId<<" was sent for echo after ECG successfully at "<<timeCur<<" will be serviced at "<<event.time<<endl;
                                                    tm.setStatusTM(true);
                                                    BCRT1.InsertInBCRT(event);
                                                }else if(tm.getStatusTM()){
                                                    tm.AddToWaitTM(event);
                                                    //cout<<"Customer"<<event.cId<<" was added to the wait queue of echo after ECG facility at time "<<timeCur<<endl;
                                                }
                                                if(!ecg.isEmptyECG()){
                                                    custEvent eventNext = ecg.PickToServeECG();
                                                    ecg.RemoveFromWaitECG();
                                                    if(eventNext.package==1||eventNext.package==6){
                                                        eventNext.semiServiced = true;
                                                        eventNext.ECG = true;
                                                    }
                                                    else
                                                        eventNext.serviced = true;
                                                    ecg.addToWaitTime(timeCur-eventNext.time);
                                                    eventNext.time = getServiceTime(timeCur,ECGMT);
                                                    ecg.addToServeTime(eventNext.time-timeCur);
                                                    //cout<<"Customer"<<eventNext.cId<<" was sent for ECG successfully at "<<timeCur<<" will be serviced at "<<eventNext.time<<endl;
                                                    ecg.setStatusECG(true);
                                                    BCRT1.InsertInBCRT(eventNext);
                                                }
                                                //If there is no event in registration queue then set status registration counter to free.
                                                else
                                                {
                                                    ecg.setStatusECG(false);
                                                }
                                            }
                                        }else if(sequence==3||sequence==9){
                                            if(event.Echo&&event.ECG&&event.Treadmill){
                                                if(tm.getCloseTime()<timeCur)
                                                    tm.updateCloseTime(timeCur);
                                                event.completionTime = event.time;
                                                double totalTime = event.completionTime- event.arrivalTime;
                                                BCRT1.addTotalTimeP(totalTime,packType-1);
                                                //cout<<"Customer"<<event.cId<<" was serviced successfully at time "<<event.time<<endl;
                                                if(!tm.isEmptyTM()){
                                                    custEvent eventNext = tm.PickToServeTM();
                                                    tm.RemoveFromWaitTM();
                                                    tm.addToWaitTime(timeCur-eventNext.time);
                                                    eventNext.time = getServiceTime(timeCur,TMMT);
                                                    eventNext.Treadmill = true;
                                                    tm.addToServeTime(eventNext.time-timeCur);
                                                    //cout<<"Customer"<<eventNext.cId<<" was sent for treadmill successfully at "<<timeCur<<" will be serviced at "<<eventNext.time<<endl;
                                                    tm.setStatusTM(true);
                                                    BCRT1.InsertInBCRT(eventNext);
                                                }
                                                //If there is no event in registration queue then set status registration counter to free.
                                                else
                                                {
                                                    tm.setStatusTM(false);
                                                }
                                            }
                                            else if(event.Echo&&event.ECG&&!event.Treadmill){
                                                if(!tm.getStatusTM()){
                                                    event.Treadmill= true;
                                                    event.time = getServiceTime(event.time,TMMT);
                                                    tm.addToServeTime(event.time-timeCur);
                                                    //cout<<"Customer"<<event.cId<<" was sent for Treadmill successfully at "<<timeCur<<" will be serviced at "<<event.time<<endl;
                                                    tm.setStatusTM(true);
                                                    BCRT1.InsertInBCRT(event);
                                                }else if(tm.getStatusTM()){
                                                    tm.AddToWaitTM(event);
                                                    //cout<<"Customer"<<event.cId<<" was added to the wait queue of TM facility at time "<<timeCur<<endl;
                                                }
                                                if(!ecg.isEmptyECG()){
                                                    custEvent eventNext = ecg.PickToServeECG();
                                                    ecg.RemoveFromWaitECG();
                                                    if(eventNext.package==1||eventNext.package==6){
                                                        eventNext.semiServiced = true;
                                                        eventNext.ECG = true;
                                                    }
                                                    else
                                                        eventNext.serviced = true;
                                                    ecg.addToWaitTime(timeCur-eventNext.time);
                                                    eventNext.time = getServiceTime(timeCur,ECGMT);
                                                    ecg.addToServeTime(eventNext.time-timeCur);
                                                    //cout<<"Customer"<<eventNext.cId<<" was sent for echo successfully at "<<timeCur<<" will be serviced at "<<eventNext.time<<endl;
                                                    ecg.setStatusECG(true);
                                                    BCRT1.InsertInBCRT(eventNext);
                                                }
                                                //If there is no event in registration queue then set status registration counter to free.
                                                else
                                                {
                                                    ecg.setStatusECG(false);
                                                }
                                            }
                                            else if(event.Echo&&!event.ECG&&!event.Treadmill){
                                                if(!ecg.getStatusECG()){
                                                    event.ECG = true;
                                                    event.time = getServiceTime(event.time,ECGMT);
                                                    ecg.addToServeTime(event.time-timeCur);
                                                    //cout<<"Customer"<<event.cId<<" was sent for echo after ECG successfully at "<<timeCur<<" will be serviced at "<<event.time<<endl;
                                                    ecg.setStatusECG(true);
                                                    BCRT1.InsertInBCRT(event);
                                                }else if(ecg.getStatusECG()){
                                                    ecg.AddToWaitECG(event);
                                                    //cout<<"Customer"<<event.cId<<" was added to the wait queue of echo after ECG facility at time "<<timeCur<<endl;
                                                }
                                                if(!echo.isEmptyECHO()){
                                                    custEvent eventNext = echo.PickToServeECHO();
                                                    echo.RemoveFromWaitECHO();
                                                    if(eventNext.package==1||eventNext.package==6){
                                                        eventNext.semiServiced = true;
                                                        eventNext.Echo = true;
                                                    }
                                                    else
                                                        eventNext.serviced = true;
                                                    echo.addToWaitTime(timeCur-eventNext.time);
                                                    eventNext.time = getServiceTime(timeCur,ECHOMT);
                                                    echo.addToServeTime(eventNext.time-timeCur);
                                                    //cout<<"Customer"<<eventNext.cId<<" was sent for ECG successfully at "<<timeCur<<" will be serviced at "<<eventNext.time<<endl;
                                                    echo.setStatusECHO(true);
                                                    BCRT1.InsertInBCRT(eventNext);
                                                }
                                                //If there is no event in registration queue then set status registration counter to free.
                                                else
                                                {
                                                    echo.setStatusECHO(false);
                                                }
                                            }
                                        }else if(sequence==4||sequence==10){
                                            if(event.Echo&&event.Treadmill&&event.ECG){
                                                if(ecg.getCloseTime()<timeCur)
                                                    ecg.updateCloseTime(timeCur);
                                                event.completionTime = event.time;
                                                double totalTime = event.completionTime- event.arrivalTime;
                                                BCRT1.addTotalTimeP(totalTime,packType-1);
                                                //cout<<"Customer"<<event.cId<<" was serviced successfully at time "<<event.time<<endl;
                                                if(!ecg.isEmptyECG()){
                                                    custEvent eventNext = ecg.PickToServeECG();
                                                    ecg.RemoveFromWaitECG();
                                                    ecg.addToWaitTime(timeCur-eventNext.time);
                                                    eventNext.time = getServiceTime(timeCur,ECGMT);
                                                    eventNext.ECG = true;
                                                    ecg.addToServeTime(eventNext.time-timeCur);
                                                    //cout<<"Customer"<<eventNext.cId<<" was sent for treadmill successfully at "<<timeCur<<" will be serviced at "<<eventNext.time<<endl;
                                                    ecg.setStatusECG(true);
                                                    BCRT1.InsertInBCRT(eventNext);
                                                }
                                                //If there is no event in registration queue then set status registration counter to free.
                                                else
                                                {
                                                    ecg.setStatusECG(false);
                                                }
                                            }
                                            else if(event.Echo&&event.Treadmill&&!event.ECG){
                                                if(!ecg.getStatusECG()){
                                                    event.ECG= true;
                                                    event.time = getServiceTime(event.time,ECGMT);
                                                    ecg.addToServeTime(event.time-timeCur);
                                                    //cout<<"Customer"<<event.cId<<" was sent for Treadmill successfully at "<<timeCur<<" will be serviced at "<<event.time<<endl;
                                                    ecg.setStatusECG(true);
                                                    BCRT1.InsertInBCRT(event);
                                                }else if(ecg.getStatusECG()){
                                                    ecg.AddToWaitECG(event);
                                                    //cout<<"Customer"<<event.cId<<" was added to the wait queue of TM facility at time "<<timeCur<<endl;
                                                }
                                                if(!tm.isEmptyTM()){
                                                    custEvent eventNext = tm.PickToServeTM();
                                                    tm.RemoveFromWaitTM();
                                                    if(eventNext.package==1||eventNext.package==6){
                                                        eventNext.semiServiced = true;
                                                        eventNext.Treadmill = true;
                                                    }
                                                    else
                                                        eventNext.serviced = true;
                                                    tm.addToWaitTime(timeCur-eventNext.time);
                                                    eventNext.time = getServiceTime(timeCur,TMMT);
                                                    tm.addToServeTime(eventNext.time-timeCur);
                                                    //cout<<"Customer"<<eventNext.cId<<" was sent for echo successfully at "<<timeCur<<" will be serviced at "<<eventNext.time<<endl;
                                                    tm.setStatusTM(true);
                                                    BCRT1.InsertInBCRT(eventNext);
                                                }
                                                //If there is no event in registration queue then set status registration counter to free.
                                                else
                                                {
                                                    tm.setStatusTM(false);
                                                }
                                            }
                                            else if(event.Echo&&!event.Treadmill&&!event.ECG){
                                                if(!tm.getStatusTM()){
                                                    event.Treadmill = true;
                                                    event.time = getServiceTime(event.time,TMMT);
                                                    tm.addToServeTime(event.time-timeCur);
                                                    //cout<<"Customer"<<event.cId<<" was sent for echo after ECG successfully at "<<timeCur<<" will be serviced at "<<event.time<<endl;
                                                    tm.setStatusTM(true);
                                                    BCRT1.InsertInBCRT(event);
                                                }else if(tm.getStatusTM()){
                                                    tm.AddToWaitTM(event);
                                                    //cout<<"Customer"<<event.cId<<" was added to the wait queue of echo after ECG facility at time "<<timeCur<<endl;
                                                }
                                                if(!echo.isEmptyECHO()){
                                                    custEvent eventNext = echo.PickToServeECHO();
                                                    echo.RemoveFromWaitECHO();
                                                    if(eventNext.package==1||eventNext.package==6){
                                                        eventNext.semiServiced = true;
                                                        eventNext.Echo = true;
                                                    }
                                                    else
                                                        eventNext.serviced = true;
                                                    echo.addToWaitTime(timeCur-eventNext.time);
                                                    eventNext.time = getServiceTime(timeCur,ECHOMT);
                                                    echo.addToServeTime(eventNext.time-timeCur);
                                                    //cout<<"Customer"<<eventNext.cId<<" was sent for ECG successfully at "<<timeCur<<" will be serviced at "<<eventNext.time<<endl;
                                                    echo.setStatusECHO(true);
                                                    BCRT1.InsertInBCRT(eventNext);
                                                }
                                                //If there is no event in registration queue then set status registration counter to free.
                                                else
                                                {
                                                    echo.setStatusECHO(false);
                                                }
                                            }
                                        }else if(sequence==5||sequence==11){
                                            if(event.Treadmill&&event.Echo&&event.ECG){
                                                if(ecg.getCloseTime()<timeCur)
                                                    ecg.updateCloseTime(timeCur);
                                                event.completionTime = event.time;
                                                double totalTime = event.completionTime- event.arrivalTime;
                                                BCRT1.addTotalTimeP(totalTime,packType-1);
                                                //cout<<"Customer"<<event.cId<<" was serviced successfully at time "<<event.time<<endl;
                                                if(!ecg.isEmptyECG()){
                                                    custEvent eventNext = ecg.PickToServeECG();
                                                    ecg.RemoveFromWaitECG();
                                                    ecg.addToWaitTime(timeCur-eventNext.time);
                                                    eventNext.time = getServiceTime(timeCur,ECGMT);
                                                    eventNext.ECG = true;
                                                    ecg.addToServeTime(eventNext.time-timeCur);
                                                    //cout<<"Customer"<<eventNext.cId<<" was sent for treadmill successfully at "<<timeCur<<" will be serviced at "<<eventNext.time<<endl;
                                                    ecg.setStatusECG(true);
                                                    BCRT1.InsertInBCRT(eventNext);
                                                }
                                                //If there is no event in registration queue then set status registration counter to free.
                                                else
                                                {
                                                    ecg.setStatusECG(false);
                                                }
                                            }
                                            else if(event.Treadmill&&event.Echo&&!event.ECG){
                                                if(!ecg.getStatusECG()){
                                                    event.ECG= true;
                                                    event.time = getServiceTime(event.time,ECGMT);
                                                    ecg.addToServeTime(event.time-timeCur);
                                                    //cout<<"Customer"<<event.cId<<" was sent for Treadmill successfully at "<<timeCur<<" will be serviced at "<<event.time<<endl;
                                                    ecg.setStatusECG(true);
                                                    BCRT1.InsertInBCRT(event);
                                                }else if(ecg.getStatusECG()){
                                                    ecg.AddToWaitECG(event);
                                                    //cout<<"Customer"<<event.cId<<" was added to the wait queue of TM facility at time "<<timeCur<<endl;
                                                }
                                                if(!echo.isEmptyECHO()){
                                                    custEvent eventNext = echo.PickToServeECHO();
                                                    echo.RemoveFromWaitECHO();
                                                    if(eventNext.package==1||eventNext.package==6){
                                                        eventNext.semiServiced = true;
                                                        eventNext.Echo = true;
                                                    }
                                                    else
                                                        eventNext.serviced = true;
                                                    echo.addToWaitTime(timeCur-eventNext.time);
                                                    eventNext.time = getServiceTime(timeCur,ECHOMT);
                                                    echo.addToServeTime(eventNext.time-timeCur);
                                                    //cout<<"Customer"<<eventNext.cId<<" was sent for echo successfully at "<<timeCur<<" will be serviced at "<<eventNext.time<<endl;
                                                    echo.setStatusECHO(true);
                                                    BCRT1.InsertInBCRT(eventNext);
                                                }
                                                //If there is no event in registration queue then set status registration counter to free.
                                                else
                                                {
                                                    echo.setStatusECHO(false);
                                                }
                                            }
                                            else if(event.Treadmill&&!event.Echo&&!event.ECG){
                                                if(!echo.getStatusECHO()){
                                                    event.Echo = true;
                                                    event.time = getServiceTime(event.time,ECHOMT);
                                                    echo.addToServeTime(event.time-timeCur);
                                                    //cout<<"Customer"<<event.cId<<" was sent for echo after ECG successfully at "<<timeCur<<" will be serviced at "<<event.time<<endl;
                                                    echo.setStatusECHO(true);
                                                    BCRT1.InsertInBCRT(event);
                                                }else if(echo.getStatusECHO()){
                                                    echo.AddToWaitECHO(event);
                                                    //cout<<"Customer"<<event.cId<<" was added to the wait queue of echo after ECG facility at time "<<timeCur<<endl;
                                                }
                                                if(!tm.isEmptyTM()){
                                                    custEvent eventNext = tm.PickToServeTM();
                                                    tm.RemoveFromWaitTM();
                                                    if(eventNext.package==1||eventNext.package==6){
                                                        eventNext.semiServiced = true;
                                                        eventNext.Treadmill = true;
                                                    }
                                                    else
                                                        eventNext.serviced = true;
                                                    tm.addToWaitTime(timeCur-eventNext.time);
                                                    eventNext.time = getServiceTime(timeCur,TMMT);
                                                    tm.addToServeTime(eventNext.time-timeCur);
                                                    //cout<<"Customer"<<eventNext.cId<<" was sent for ECG successfully at "<<timeCur<<" will be serviced at "<<eventNext.time<<endl;
                                                    tm.setStatusTM(true);
                                                    BCRT1.InsertInBCRT(eventNext);
                                                }
                                                //If there is no event in registration queue then set status registration counter to free.
                                                else
                                                {
                                                    tm.setStatusTM(false);
                                                }
                                            }
                                        }else if(sequence==6||sequence==12){
                                            if(event.Treadmill&&event.ECG&&event.Echo){
                                                if(echo.getCloseTime()<timeCur)
                                                    echo.updateCloseTime(timeCur);
                                                event.completionTime = event.time;
                                                double totalTime = event.completionTime- event.arrivalTime;
                                                BCRT1.addTotalTimeP(totalTime,packType-1);
                                                //cout<<"Customer"<<event.cId<<" was serviced successfully at time "<<event.time<<endl;
                                                if(!echo.isEmptyECHO()){
                                                    custEvent eventNext = echo.PickToServeECHO();
                                                    echo.RemoveFromWaitECHO();
                                                    echo.addToWaitTime(timeCur-eventNext.time);
                                                    eventNext.time = getServiceTime(timeCur,ECHOMT);
                                                    eventNext.Echo = true;
                                                    echo.addToServeTime(eventNext.time-timeCur);
                                                    //cout<<"Customer"<<eventNext.cId<<" was sent for treadmill successfully at "<<timeCur<<" will be serviced at "<<eventNext.time<<endl;
                                                    echo.setStatusECHO(true);
                                                    BCRT1.InsertInBCRT(eventNext);
                                                }
                                                //If there is no event in registration queue then set status registration counter to free.
                                                else
                                                {
                                                    echo.setStatusECHO(false);
                                                }
                                            }
                                            else if(event.Treadmill&&event.ECG&&!event.Echo){
                                                if(!echo.getStatusECHO()){
                                                    event.Echo= true;
                                                    event.time = getServiceTime(event.time,ECHOMT);
                                                    echo.addToServeTime(event.time-timeCur);
                                                    //cout<<"Customer"<<event.cId<<" was sent for Treadmill successfully at "<<timeCur<<" will be serviced at "<<event.time<<endl;
                                                    echo.setStatusECHO(true);
                                                    BCRT1.InsertInBCRT(event);
                                                }else if(echo.getStatusECHO()){
                                                    echo.AddToWaitECHO(event);
                                                    //cout<<"Customer"<<event.cId<<" was added to the wait queue of TM facility at time "<<timeCur<<endl;
                                                }
                                                if(!ecg.isEmptyECG()){
                                                    custEvent eventNext = ecg.PickToServeECG();
                                                    ecg.RemoveFromWaitECG();
                                                    if(eventNext.package==1||eventNext.package==6){
                                                        eventNext.semiServiced = true;
                                                        eventNext.ECG = true;
                                                    }
                                                    else
                                                        eventNext.serviced = true;
                                                    ecg.addToWaitTime(timeCur-eventNext.time);
                                                    eventNext.time = getServiceTime(timeCur,ECGMT);
                                                    ecg.addToServeTime(eventNext.time-timeCur);
                                                    //cout<<"Customer"<<eventNext.cId<<" was sent for echo successfully at "<<timeCur<<" will be serviced at "<<eventNext.time<<endl;
                                                    ecg.setStatusECG(true);
                                                    BCRT1.InsertInBCRT(eventNext);
                                                }
                                                //If there is no event in registration queue then set status registration counter to free.
                                                else
                                                {
                                                    ecg.setStatusECG(false);
                                                }
                                            }
                                            else if(event.Treadmill&&!event.ECG&&!event.Echo){
                                                if(!ecg.getStatusECG()){
                                                    event.ECG = true;
                                                    event.time = getServiceTime(event.time,ECGMT);
                                                    ecg.addToServeTime(event.time-timeCur);
                                                    //cout<<"Customer"<<event.cId<<" was sent for echo after ECG successfully at "<<timeCur<<" will be serviced at "<<event.time<<endl;
                                                    ecg.setStatusECG(true);
                                                    BCRT1.InsertInBCRT(event);
                                                }else if(ecg.getStatusECG()){
                                                    ecg.AddToWaitECG(event);
                                                    //cout<<"Customer"<<event.cId<<" was added to the wait queue of echo after ECG facility at time "<<timeCur<<endl;
                                                }
                                                if(!tm.isEmptyTM()){
                                                    custEvent eventNext = tm.PickToServeTM();
                                                    tm.RemoveFromWaitTM();
                                                    if(eventNext.package==1||eventNext.package==6){
                                                        eventNext.semiServiced = true;
                                                        eventNext.Treadmill = true;
                                                    }
                                                    else
                                                        eventNext.serviced = true;
                                                    tm.addToWaitTime(timeCur-eventNext.time);
                                                    eventNext.time = getServiceTime(timeCur,TMMT);
                                                    tm.addToServeTime(eventNext.time-timeCur);
                                                    //cout<<"Customer"<<eventNext.cId<<" was sent for ECG successfully at "<<timeCur<<" will be serviced at "<<eventNext.time<<endl;
                                                    tm.setStatusTM(true);
                                                    BCRT1.InsertInBCRT(eventNext);
                                                }
                                                //If there is no event in registration queue then set status registration counter to free.
                                                else
                                                {
                                                    tm.setStatusTM(false);
                                                }
                                            }
                                        }
                                    }
                                    break;
                                case 6:
                                    {
                                        if(sequence==1||sequence==2||sequence==3||sequence==4||sequence==5||sequence==6){
                                            if(event.ECG&&event.Echo){
                                                if(echo.getCloseTime()<timeCur)
                                                    echo.updateCloseTime(timeCur);
                                                event.completionTime = event.time;
                                                double totalTime = event.completionTime- event.arrivalTime;
                                                BCRT1.addTotalTimeP(totalTime,packType-1);
                                                //cout<<"Customer"<<event.cId<<" was serviced successfully at time "<<event.time<<endl;
                                                if(!echo.isEmptyECHO()){
                                                    custEvent eventNext = echo.PickToServeECHO();
                                                    echo.RemoveFromWaitECHO();
                                                    if(eventNext.package==1||eventNext.package==6){
                                                        eventNext.semiServiced = true;
                                                        eventNext.Echo = true;
                                                    }
                                                    else
                                                        eventNext.serviced = true;
                                                    echo.addToWaitTime(timeCur-eventNext.time);
                                                    eventNext.time = getServiceTime(timeCur,ECHOMT);
                                                    echo.addToServeTime(eventNext.time-timeCur);
                                                    //cout<<"Customer"<<eventNext.cId<<" was sent for ECG successfully at "<<timeCur<<" will be serviced at "<<eventNext.time<<endl;
                                                    echo.setStatusECHO(true);
                                                    BCRT1.InsertInBCRT(eventNext);
                                                }
                                                //If there is no event in registration queue then set status registration counter to free.
                                                else
                                                {
                                                    echo.setStatusECHO(false);
                                                }
                                            }
                                            else if(event.ECG&&!event.Echo){
                                                if(!echo.getStatusECHO()){
                                                    event.Echo= true;
                                                    event.time = getServiceTime(event.time,ECHOMT);
                                                    echo.addToServeTime(event.time-timeCur);
                                                    //cout<<"Customer"<<event.cId<<" was sent for ECG successfully at "<<timeCur<<" will be serviced at "<<event.time<<endl;
                                                    echo.setStatusECHO(true);
                                                    BCRT1.InsertInBCRT(event);
                                                }else if(echo.getStatusECHO()){
                                                    echo.AddToWaitECHO(event);
                                                    //cout<<"Customer"<<event.cId<<" was added to the wait queue of ECG facility at time "<<timeCur<<endl;
                                                }
                                                if(!ecg.isEmptyECG()){
                                                    custEvent eventNext = ecg.PickToServeECG();
                                                    ecg.RemoveFromWaitECG();
                                                    if(eventNext.package==1||eventNext.package==6){
                                                        eventNext.semiServiced = true;
                                                        eventNext.ECG = true;
                                                    }
                                                    else
                                                        eventNext.serviced = true;
                                                    ecg.addToWaitTime(timeCur-eventNext.time);
                                                    eventNext.time = getServiceTime(timeCur,ECGMT);
                                                    ecg.addToServeTime(eventNext.time-timeCur);
                                                    //cout<<"Customer"<<eventNext.cId<<" was sent for echo successfully at "<<timeCur<<" will be serviced at "<<eventNext.time<<endl;
                                                    ecg.setStatusECG(true);
                                                    BCRT1.InsertInBCRT(eventNext);
                                                }
                                                //If there is no event in registration queue then set status registration counter to free.
                                                else
                                                {
                                                    ecg.setStatusECG(false);
                                                }
                                            }
                                        }else if(sequence==7||sequence==8||sequence==9||sequence==10||sequence==11||sequence==12){
                                                if(event.Echo&&event.ECG){
                                                    if(ecg.getCloseTime()<timeCur)
                                                        ecg.updateCloseTime(timeCur);
                                                    event.completionTime = event.time;
                                                    double totalTime = event.completionTime- event.arrivalTime;
                                                    BCRT1.addTotalTimeP(totalTime,packType-1);
                                                    //cout<<"Customer"<<event.cId<<" was serviced successfully at time "<<event.time<<endl;
                                                    if(!ecg.isEmptyECG()){
                                                        custEvent eventNext = ecg.PickToServeECG();
                                                        ecg.RemoveFromWaitECG();
                                                        if(eventNext.package==1||eventNext.package==6){
                                                            eventNext.semiServiced = true;
                                                            eventNext.ECG = true;
                                                        }
                                                        else
                                                            eventNext.serviced = true;
                                                        ecg.addToWaitTime(timeCur-eventNext.time);
                                                        eventNext.time = getServiceTime(timeCur,ECGMT);
                                                        ecg.addToServeTime(eventNext.time-timeCur);
                                                        //cout<<"Customer"<<eventNext.cId<<" was sent for ECG successfully at "<<timeCur<<" will be serviced at "<<eventNext.time<<endl;
                                                        ecg.setStatusECG(true);
                                                        BCRT1.InsertInBCRT(eventNext);
                                                    }
                                                    //If there is no event in registration queue then set status registration counter to free.
                                                    else
                                                    {
                                                        ecg.setStatusECG(false);
                                                    }
                                                }
                                                else if(event.Echo&&!event.ECG){
                                                    if(!ecg.getStatusECG()){
                                                        event.ECG= true;
                                                        event.time = getServiceTime(event.time,ECGMT);
                                                        ecg.addToServeTime(event.time-timeCur);
                                                        //cout<<"Customer"<<event.cId<<" was sent for ECG successfully at "<<timeCur<<" will be serviced at "<<event.time<<endl;
                                                        ecg.setStatusECG(true);
                                                        BCRT1.InsertInBCRT(event);
                                                    }else if(ecg.getStatusECG()){
                                                        ecg.AddToWaitECG(event);
                                                        //cout<<"Customer"<<event.cId<<" was added to the wait queue of ECG facility at time "<<timeCur<<endl;
                                                    }
                                                    if(!echo.isEmptyECHO()){
                                                        custEvent eventNext = echo.PickToServeECHO();
                                                        echo.RemoveFromWaitECHO();
                                                        if(eventNext.package==1||eventNext.package==6){
                                                            eventNext.semiServiced = true;
                                                            eventNext.Echo = true;
                                                        }
                                                        else
                                                            eventNext.serviced = true;
                                                        echo.addToWaitTime(timeCur-eventNext.time);
                                                        eventNext.time = getServiceTime(timeCur,ECHOMT);
                                                        echo.addToServeTime(eventNext.time-timeCur);
                                                        //cout<<"Customer"<<eventNext.cId<<" was sent for echo successfully at "<<timeCur<<" will be serviced at "<<eventNext.time<<endl;
                                                        echo.setStatusECHO(true);
                                                        BCRT1.InsertInBCRT(eventNext);
                                                    }
                                                    //If there is no event in registration queue then set status registration counter to free.
                                                    else
                                                    {
                                                        echo.setStatusECHO(false);
                                                    }
                                                }
                                            }
                                    }
                                    break;
                                }
                            }
                            break;
                        }

                }
                break;
            }

        }

        //Fill array to compute Average total time
        for(int i = 0;i<6;i++){
            AvgTotalTime[i] += BCRT1.getTotalTime(i);
        }

        //Computation of utilization for each service
        double util[6];
        util[0] = (reg.getServeTime()/reg.getCloseTime()>1)?1:reg.getServeTime()/reg.getCloseTime();
        util[1] = (ecg.getServeTime()/ecg.getCloseTime()>1)?1:ecg.getServeTime()/ecg.getCloseTime();
        util[2] = (echo.getServeTime()/echo.getCloseTime()>1)?1:echo.getServeTime()/echo.getCloseTime();
        util[3] = (tm.getServeTime()/tm.getCloseTime()>1)?1:tm.getServeTime()/tm.getCloseTime();
        util[4] = ((mri.getServeTime()/(mri.getCloseTime()-120))>1)?1:mri.getServeTime()/(mri.getCloseTime()-120);
        util[5] = (ct.getServeTime()/ct.getCloseTime()>1)?1:ct.getServeTime()/ct.getCloseTime();
        for(int i = 0;i<6;i++){
            AvgUtil[i]+=util[i];
        }

        //Computation of closing time of each service
        AvgCloseTime[0]+=reg.getCloseTime();
        AvgCloseTime[1]+=ecg.getCloseTime();
        AvgCloseTime[2]+=echo.getCloseTime();
        AvgCloseTime[3]+=tm.getCloseTime();
        AvgCloseTime[4]+=mri.getCloseTime();
        AvgCloseTime[5]+=ct.getCloseTime();

        //Computation of wait time for each service to get service with lowest throughput
        double wait[6];
        wait[0] = reg.getWaitTime();
        wait[1] = ecg.getWaitTime();
        wait[2] = echo.getWaitTime();
        wait[3] = tm.getWaitTime();
        wait[4] = mri.getWaitTime();
        wait[5] = ct.getWaitTime();
        for(int i = 0;i<6;i++){
            AvgWait[i]+=wait[i];
        }

        //Code to print values in each iteration :

        /*cout<<"**************************************************************************************"<<endl;
        cout<<"The output of simulation for DAY"<<i+1<<" are as follows : "<<endl;
        cout<<endl<<endl;
        cout<<"See if each customer is serviced fully or not."<<endl;
        cout<<"--------------------------------------------------------------------------------------"<<endl;
        cout<<"The queue for registration is : "<<endl;
        reg.printREGQueue();
        cout<<"The queue for ECG is : "<<endl;
        ecg.printECGQueue();
        cout<<"The queue for CT is : "<<endl;
        ct.printCTQueue();
        cout<<"The queue for MRI is : "<<endl;
        mri.printMRIQueue();
        cout<<"The queue for ECHO is : "<<endl;
        echo.printECHOQueue();
        cout<<"The queue for Treadmill is : "<<endl;
        tm.printTMQueue();
        cout<<"--------------------------------------------------------------------------------------"<<endl;
        cout<<"The total number of customers in each package "<<endl;
        cout<<"--------------------------------------------------------------------------------------"<<endl;
        BCRT1.printNumCustomerPack();
        cout<<"--------------------------------------------------------------------------------------"<<endl;
        cout<<"The total time taken by customers for each package are : "<<endl;
        cout<<"--------------------------------------------------------------------------------------"<<endl;
        BCRT1.printTotalTimePack();
        cout<<"--------------------------------------------------------------------------------------"<<endl;
        cout<<"A) The average total time spent by patients for each package are : "<<endl;
        cout<<"--------------------------------------------------------------------------------------"<<endl;
        BCRT1.printAverageTotalTimePack();
        cout<<"--------------------------------------------------------------------------------------"<<endl;
        cout<<"B) The facilities with minimum and maximum average utilization are : "<<endl;
        cout<<"--------------------------------------------------------------------------------------"<<endl;

        int minUtil = findMin(util,6);
        int maxUtil = findMax(util,6);
        switch(minUtil){
        case 0 :
            {
                cout<<"The minimum utilization came for Registration : "<<endl;
                cout<<"The utilization for Registration is : "<<util[0]<<endl;
            }
            break;
        case 1 :
            {
                cout<<"The minimum utilization came for ECG : "<<endl;
                cout<<"The utilization for ECG is : "<<util[1]<<endl;
            }
            break;
        case 2 :
            {
                cout<<"The minimum utilization came for ECHO : "<<endl;
                cout<<"The utilization for ECHO is : "<<util[2]<<endl;
            }
            break;
        case 3 :
            {
                cout<<"The minimum utilization came for TM : "<<endl;
                cout<<"The utilization for TM is : "<<util[3]<<endl;
            }
            break;
        case 4 :
            {
                cout<<"The minimum utilization came for MRI : "<<endl;
                cout<<"The utilization for MRI is : "<<util[4]<<endl;
            }
            break;
        case 5 :
            {
                cout<<"The minimum utilization came for CT : "<<endl;
                cout<<"The utilization for CT is : "<<util[5]<<endl;
            }
            break;
        }
        cout<<endl;
        switch(maxUtil){
        case 0 :
            {
                cout<<"The maximum utilization came for Registration : "<<endl;
                cout<<"The utilization for Registration is : "<<util[0]<<endl;
            }
            break;
        case 1 :
            {
                cout<<"The maximum utilization came for ECG : "<<endl;
                cout<<"The utilization for ECG is : "<<util[1]<<endl;
            }
            break;
        case 2 :
            {
                cout<<"The maximum utilization came for ECHO : "<<endl;
                cout<<"The utilization for ECHO is : "<<util[2]<<endl;
            }
            break;
        case 3 :
            {
                cout<<"The maximum utilization came for TM : "<<endl;
                cout<<"The utilization for TM is : "<<util[3]<<endl;
            }
            break;
        case 4 :
            {
                cout<<"The maximum utilization came for MRI : "<<endl;
                cout<<"The utilization for MRI is : "<<util[4]<<endl;
            }
            break;
        case 5 :
            {
                cout<<"The maximum utilization came for CT : "<<endl;
                cout<<"The utilization for CT is : "<<util[5]<<endl;
            }
            break;
        }

        cout<<"--------------------------------------------------------------------------------------"<<endl;
        cout<<"D) The final closing time of each facility is : "<<endl;
        cout<<"--------------------------------------------------------------------------------------"<<endl;
        cout<<"The final closing time of Registration is : "<<reg.getCloseTime()<<endl;
        cout<<"The final closing time of ECG is : "<<ecg.getCloseTime()<<endl;
        cout<<"The final closing time of ECHO is : "<<echo.getCloseTime()<<endl;
        cout<<"The final closing time of Treadmill is : "<<tm.getCloseTime()<<endl;
        cout<<"The final closing time of CT is : "<<ct.getCloseTime()<<endl;
        cout<<"The final closing time of MRI is : "<<mri.getCloseTime()<<endl;
        cout<<"--------------------------------------------------------------------------------------"<<endl;
        cout<<"E) The facility to be duplicated : "<<endl;
        cout<<"--------------------------------------------------------------------------------------"<<endl;
        int maxWait = findMax(wait,6);
        switch(maxWait){
        case 0 :
            {
                cout<<"The maximum wait time came for Registration : "<<endl;
                cout<<"The wait time for Registration is : "<<wait[0]<<endl;
                cout<<"Therefore throughput will increase if we duplicate Registration facility "<<endl;
            }
            break;
        case 1 :
            {
                cout<<"The maximum wait time came for ECG : "<<endl;
                cout<<"The wait time for ECG is : "<<wait[1]<<endl;
                cout<<"Therefore throughput will increase if we duplicate ECG facility "<<endl;
            }
            break;
        case 2 :
            {
                cout<<"The maximum wait time came for ECHO : "<<endl;
                cout<<"The wait time for ECHO is : "<<wait[2]<<endl;
                cout<<"Therefore throughput will increase if we duplicate ECHO facility "<<endl;
            }
            break;
        case 3 :
            {
                cout<<"The maximum wait time came for Treadmill : "<<endl;
                cout<<"The wait time for Treadmill is : "<<wait[3]<<endl;
                cout<<"Therefore throughput will increase if we duplicate Treadmill facility "<<endl;
            }
            break;
        case 4 :
            {
                cout<<"The maximum wait time came for MRI : "<<endl;
                cout<<"The wait time for MRI is : "<<wait[4]<<endl;
                cout<<"Therefore throughput will increase if we duplicate MRI facility "<<endl;
            }
            break;
        case 5 :
            {
                cout<<"The maximum wait time came for CT : "<<endl;
                cout<<"The wait time for CT is : "<<wait[5]<<endl;
                cout<<"Therefore throughput will increase if we duplicate CT facility "<<endl;
            }
            break;
        }
        cout<<"--------------------------------------------------------------------------------------"<<endl;
        cout<<"**************************************************************************************"<<endl;*/
    }
    double SumAvgTotal = 0.0;
    for(int i =0;i<6;i++){
        SumAvgTotal+=AvgTotalTime[i]/countDays;
    }
    cout<<"--------------------------------------------------------------------------------------"<<endl;
    cout<<"A) The average total time spent by patients for each package are : "<<endl;
    cout<<"--------------------------------------------------------------------------------------"<<endl;
    for(int i =0;i<6;i++){
        cout<<"The average total time taken by customers in package"<<i+1<<" is : "<<AvgTotalTime[i]/countDays<<endl;
    }
    cout<<"                                                    --------------------"<<endl;
    cout<<"The sum of average total time taken by customers is :       "<<SumAvgTotal<<endl;
    cout<<"--------------------------------------------------------------------------------------"<<endl;
    cout<<"B) The facilities with minimum and maximum average utilization are : "<<endl;
    cout<<"--------------------------------------------------------------------------------------"<<endl;
    int minUtil = findMin(AvgUtil,6);
    int maxUtil = findMax(AvgUtil,6);
    switch(minUtil){
    case 0 :
        {
            cout<<"The minimum utilization came for Registration : "<<endl;
            cout<<"The utilization for Registration is : "<<AvgUtil[0]/countDays<<endl;
        }
        break;
    case 1 :
        {
            cout<<"The minimum utilization came for ECG : "<<endl;
            cout<<"The utilization for ECG is : "<<AvgUtil[1]/countDays<<endl;
        }
        break;
    case 2 :
        {
            cout<<"The minimum utilization came for ECHO : "<<endl;
            cout<<"The utilization for ECHO is : "<<AvgUtil[2]/countDays<<endl;
        }
        break;
    case 3 :
        {
            cout<<"The minimum utilization came for TM : "<<endl;
            cout<<"The utilization for TM is : "<<AvgUtil[3]/countDays<<endl;
        }
        break;
    case 4 :
        {
            cout<<"The minimum utilization came for MRI : "<<endl;
            cout<<"The utilization for MRI is : "<<AvgUtil[4]/countDays<<endl;
        }
        break;
    case 5 :
        {
            cout<<"The minimum utilization came for CT : "<<endl;
            cout<<"The utilization for CT is : "<<AvgUtil[5]/countDays<<endl;
        }
        break;
    }
    cout<<endl;
    switch(maxUtil){
    case 0 :
        {
            cout<<"The maximum utilization came for Registration : "<<endl;
            cout<<"The utilization for Registration is : "<<AvgUtil[0]/countDays<<endl;
        }
        break;
    case 1 :
        {
            cout<<"The maximum utilization came for ECG : "<<endl;
            cout<<"The utilization for ECG is : "<<AvgUtil[1]/countDays<<endl;
        }
        break;
    case 2 :
        {
            cout<<"The maximum utilization came for ECHO : "<<endl;
            cout<<"The utilization for ECHO is : "<<AvgUtil[2]/countDays<<endl;
        }
        break;
    case 3 :
        {
            cout<<"The maximum utilization came for TM : "<<endl;
            cout<<"The utilization for TM is : "<<AvgUtil[3]/countDays<<endl;
        }
        break;
    case 4 :
        {
            cout<<"The maximum utilization came for MRI : "<<endl;
            cout<<"The utilization for MRI is : "<<AvgUtil[4]/countDays<<endl;
        }
        break;
    case 5 :
        {
            cout<<"The maximum utilization came for CT : "<<endl;
            cout<<"The utilization for CT is : "<<AvgUtil[5]/countDays<<endl;
        }
        break;
    }
    cout<<"--------------------------------------------------------------------------------------"<<endl;
    cout<<"D) The final average closing time of each facility is : "<<endl;
    cout<<"--------------------------------------------------------------------------------------"<<endl;
    cout<<"The final average closing time of Registration is : "<<AvgCloseTime[0]/countDays<<endl;
    cout<<"The final average closing time of ECG is : "<<AvgCloseTime[1]/countDays<<endl;
    cout<<"The final average closing time of ECHO is : "<<AvgCloseTime[2]/countDays<<endl;
    cout<<"The final average closing time of Treadmill is : "<<AvgCloseTime[3]/countDays<<endl;
    cout<<"The final average closing time of MRI is : "<<AvgCloseTime[4]/countDays<<endl;
    cout<<"The final average closing time of CT is : "<<AvgCloseTime[5]/countDays<<endl;
    cout<<"--------------------------------------------------------------------------------------"<<endl;
    cout<<"E) The facility to be duplicated : "<<endl;
    cout<<"--------------------------------------------------------------------------------------"<<endl;
    int maxWait = findMax(AvgWait,6);
    switch(maxWait){
    case 0 :
        {
            cout<<"The maximum wait time came for Registration : "<<endl;
            cout<<"The wait time for Registration is : "<<AvgWait[0]/countDays<<endl;
            cout<<"Therefore throughput will increase if we duplicate Registration facility "<<endl;
        }
        break;
    case 1 :
        {
            cout<<"The maximum wait time came for ECG : "<<endl;
            cout<<"The wait time for ECG is : "<<AvgWait[1]/countDays<<endl;
            cout<<"Therefore throughput will increase if we duplicate ECG facility "<<endl;
        }
        break;
    case 2 :
        {
            cout<<"The maximum wait time came for ECHO : "<<endl;
            cout<<"The wait time for ECHO is : "<<AvgWait[2]/countDays<<endl;
            cout<<"Therefore throughput will increase if we duplicate ECHO facility "<<endl;
        }
        break;
    case 3 :
        {
            cout<<"The maximum wait time came for Treadmill : "<<endl;
            cout<<"The wait time for Treadmill is : "<<AvgWait[3]/countDays<<endl;
            cout<<"Therefore throughput will increase if we duplicate Treadmill facility "<<endl;
        }
        break;
    case 4 :
        {
            cout<<"The maximum wait time came for MRI : "<<endl;
            cout<<"The wait time for MRI is : "<<AvgWait[4]/countDays<<endl;
            cout<<"Therefore throughput will increase if we duplicate MRI facility "<<endl;
        }
        break;
    case 5 :
        {
            cout<<"The maximum wait time came for CT : "<<endl;
            cout<<"The wait time for CT is : "<<AvgWait[5]/countDays<<endl;
            cout<<"Therefore throughput will increase if we duplicate CT facility "<<endl;
        }
        break;
    }
    cout<<"--------------------------------------------------------------------------------------"<<endl;
    cout<<"**************************************************************************************"<<endl;
    return 0;
}
int findMin(double *util,int n){
    int indexMin;
    double minUtil = 1000;
    for(int i = 0;i<6;i++){
        if(util[i]<minUtil){
            minUtil = util[i];
            indexMin = i;
        }
    }
    return indexMin;
}

int findMax(double *util,int n){
    int indexMax;
    double maxUtil = 0.0;
    for(int i = 0;i<6;i++){
        if(util[i]>maxUtil){
            maxUtil = util[i];
            indexMax = i;
        }
    }
    return indexMax;
}

double getServiceTime(double tInit,double l){
    srand( (unsigned)time(  NULL ) );
    double r =  ((double) rand()/(double)RAND_MAX);
    if(r<0.1)
        r+=0.1;
    if(r>0.9)
        r-=0.1;
    double logr = log(r);
    double t = -logr*l;
    return tInit + t;
}

double generateInterArrival(double tInit,double l){
    srand( (unsigned)time( NULL) );
    double r =  ((double) rand()/(double)RAND_MAX);
    if(r<0.1)
        r+=0.1;
    if(r>0.9)
        r-=0.1;
    double logr = log(r);
    double t = -logr*l;
    return tInit+t;
}

void InitArrival(custEvent *Init,double *L,int InitLength){
    for(int i =0;i<InitLength;i++){
        Init[i].cId = i+1;
        Init[i].package = i+1;
        if(Init[i].package==1){
            Init[i].interArrival = L[i];
            Init[i].meanService = ECGMT+ECHOMT+TMMT;
            Init[i].ECG = false;
            Init[i].Echo = false;
            Init[i].Treadmill = false;
            Init[i].time = generateInterArrival(0,Init[i].interArrival);
        }
        else if(Init[i].package==2){
            Init[i].interArrival = L[i];
            Init[i].meanService = MRIMT;
            Init[i].time = generateInterArrival(120.0,Init[i].interArrival);
        }
        else if(Init[i].package==3){
            Init[i].interArrival = L[i-1];
            Init[i].meanService = CTMT;
            Init[i].time = generateInterArrival(120.0,Init[i].interArrival);
        }
        else if(Init[i].package==4){
            Init[i].interArrival = L[i-1];
            Init[i].interArrival2 = L[i];
            Init[i].meanService = ECGMT;
            Init[i].time = generateInterArrival(0,Init[i].interArrival);
        }
        else if(Init[i].package==5){
            Init[i].interArrival = L[i-1];
            Init[i].meanService = ECHOMT;
            Init[i].time = generateInterArrival(0,Init[i].interArrival);
        }
        else if(Init[i].package==6){
            Init[i].interArrival = L[i-1];
            Init[i].meanService = ECHOMT+ECGMT;
            Init[i].Echo = false;
            Init[i].ECG = false;
            Init[i].time = generateInterArrival(0,Init[i].interArrival);
        }
    }
}
