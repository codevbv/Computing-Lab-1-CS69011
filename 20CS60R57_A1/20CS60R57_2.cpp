#include <iostream>
#include <fstream>
#include <math.h>
#include <sstream>
#define P 1009
#define d 1000

using namespace std;

//Prototyping
void ParseInputToUserStruct(string ip);
void Swap_MinPathUser_With_FirstUser(int minIndex);
void CreateHashTableU1(int *u1Path,int u1PathLength,int window);
void InsertInHashTable(int indexInsert,string valInsert);
string FindKeyMatch(int userIndex,int window);
bool CheckHashTable(int index,string val);
int calcModPow(int val,int power);



//For data of each user entry
struct user{
    char uId[20];
    int uPath[1000];
    int pathLength=0;
};

//Array of pointers to each user struct
user *ArrUser[2];
int userCount = 0;

//Retrieves minimum path length from input paths
int minPathLength = 2000;

//Retrieves index of user struct with minimum path length
int ArrUserIndex_MinPath = 0;

//Node Hash table
struct RHTNode{
    string val;
};

//Defining Hash table
RHTNode *RHT[P];

//Function to reset hash table
void ResetRHT();

//Take input file name from command line arg
int main(int argc,char *argv[])
{
    string ip;
    if(argc==1){
	cout<<"Enter the name of ip file with extension .txt "<<endl;
	cin>>ip;
    }
    else
    	ip = argv[1];

    //Parse the give .txt file to array pointer to of user struct
    ParseInputToUserStruct(ip);

    //Place the user with minimum path at index 0
    Swap_MinPathUser_With_FirstUser(ArrUserIndex_MinPath);

    //Get details of index 0 user
    int u0PathLength = ArrUser[0]->pathLength;
    int *u0Path = ArrUser[0]->uPath;

    //Get details of index 1 user
    int userPathLength = ArrUser[1]->pathLength;

    //MaxSubPath
    string MaxSubPath;

    //Binary Search on window length
    int leastWindow = 1;
    int maxWindow = u0PathLength;
    if(maxWindow==0){
        cout<<"There is no common sub path between given users  "<<endl;
    }
    int window;
    while(leastWindow<=maxWindow){
        if((leastWindow == maxWindow)){
            window = leastWindow;
            ResetRHT();
            CreateHashTableU1(u0Path,u0PathLength,window);
            MaxSubPath = FindKeyMatch(1,window);
            if(MaxSubPath==""){
                window = window-1;
                if(window==0){
                    cout<<"There is no common sub path between given users  "<<endl;
                }
                ResetRHT();
                CreateHashTableU1(u0Path,u0PathLength,window);
                MaxSubPath = FindKeyMatch(1,window);
            }
            cout<<"Maximum common sub path is : "<<MaxSubPath<<endl;
            break;
        }else{
            window = (leastWindow+maxWindow)/2;
            ResetRHT();
            CreateHashTableU1(u0Path,u0PathLength,window);
            MaxSubPath = FindKeyMatch(1,window);
            if(MaxSubPath==""){
                if(window==leastWindow){
                    maxWindow = window;
                }else
                maxWindow = window-1;
            }else if(MaxSubPath!=""){
                if(window==maxWindow){
                    leastWindow = window;
                }else
                    leastWindow = window+1;
            }
        }
    }


    return 0;
}

//Function to parse input
void ParseInputToUserStruct(string ip){

    //Open the input file with given name
    ifstream ipFile;
    ipFile.open(ip);

    //Parse, Tokenize and store entry of each user.
    while(!ipFile.eof()&&userCount<2){

        //Create entry in struct for user
        ArrUser[userCount] = new user;

        //Index to store uId string
        int uIdIndex =0;

        //Set uId for user
        while(ipFile.peek()!=':'&&!ipFile.eof()){
            char ch = ipFile.get();
            ArrUser[userCount]->uId[uIdIndex++] = ch;
        }
        ArrUser[userCount]->uId[uIdIndex] = '\0';

        //Skip ':'
        ipFile.get();

        //Track current path length of user
        int length =0;

        //Tokenise and Parse till '\n' not reached
        while(ipFile.peek()!='\n'&&!ipFile.eof()){
            if(ipFile.peek()=='\r'){
                ipFile.get();
                continue;
            }
            if(ipFile.peek()==',')
                ipFile.get();
            else{
        //Convert Characters to integer for each vertex and check for valid vertex entries
                int x =0;
                while(ipFile.peek()!=','&&ipFile.peek()!='\n'&&!ipFile.eof()){
                    if(ipFile.peek()=='\n'||ipFile.peek()=='\r')
                        break;
                    x = x*10 + (ipFile.get()-48);
                }
                if(x>0){
                    ArrUser[userCount]->uPath[length] = x;
                    length++;
                }else{
                    cout<<"Invalid entry for Path vertex "<<endl;
                    return;
                }
            }
        }
        ArrUser[userCount]->pathLength = length;
        if(length<minPathLength){
            minPathLength = length;
            ArrUserIndex_MinPath = userCount;
        }

        userCount++;
        //Skip '\n' character
        ipFile.get();
    }
}

//Swap user with minimum path length from user at index 0
void Swap_MinPathUser_With_FirstUser(int minIndex){
    user *temp;
    temp = ArrUser[0];
    ArrUser[0] = ArrUser[minIndex];
    ArrUser[minIndex] = temp;
}

//Fill hash table for user at index 0 for th given window size
void CreateHashTableU1(int *u1Path,int u1PathLength,int window){
    //Calculate Index of first window
    int i =window-1;
    int hIndexPrev=0;
    while(i>=0){
        hIndexPrev+= (((u1Path[i])%P)*calcModPow(d,window-i-1))%P;
        hIndexPrev = hIndexPrev%P;
        i--;
    }
    hIndexPrev = hIndexPrev%P;

    //Calculate Path for first window
    string hValPrev;
    ostringstream str1;
    int j =0;
    while(j!=window){
        if(j==window-1)
            str1<<u1Path[j];
        else
            str1<<u1Path[j]<<",";
        j++;
    }
    hValPrev = str1.str();

    //Insert first window path at index
    InsertInHashTable(hIndexPrev,hValPrev);

    //For each subsequent window of given size
    for(int i =1;i<u1PathLength-window+1;i++){

        //Calculate Index for window i
        int hIndexSub = (hIndexPrev-((u1Path[i-1]%P)*calcModPow(d,window-1))%P);
        if(hIndexSub<0)
            hIndexSub+=P;
        int hIndex = ((((hIndexSub%P)*d)%P)+(u1Path[i+window-1]))%P;
        hIndexPrev = hIndex;

        //Calculate Path for window i
        string hVal;
        ostringstream str1;
        if(window==1)
            str1<<u1Path[i+window-1];
        else{
            str1<<hValPrev.substr(hValPrev.find(",")+1);
            str1<<","<<u1Path[i+window-1];
        }
        hVal = str1.str();
        hValPrev = hVal;

        //Insert Subsequent window path at index
        InsertInHashTable(hIndex,hVal);
    }
}

//Function to insert Path at given index in hash table
void InsertInHashTable(int indexInsert,string valueInsert){

    //Go clockwise through all psoitions once
    bool inserted = false;
    for(int i = 0;!inserted&&i<P;i++){
        if(RHT[(indexInsert+i)%P]==nullptr){
            RHT[(indexInsert+i)%P] = new RHTNode;
            RHT[(indexInsert+i)%P]->val = valueInsert;
            inserted = true;
        }else if(RHT[(indexInsert+i)%P]->val==valueInsert){
            inserted = true;
        }
    }
}

//Find if path is present in given index of hash table
string FindKeyMatch(int userIndex,int window){

    //Boolean to keep track that path matched or not if matched return
    bool matched = false;

    //Retrieve path length and path for given user
    int userPathLength = ArrUser[userIndex]->pathLength;
    int *userPath = ArrUser[userIndex]->uPath;

    //If window size greater than path length return null string
    if(window>userPathLength)
        return "";

    //Calculate Index of first window
    int i =window-1;
    int hIndexPrev=0;
    while(i>=0){
        hIndexPrev+= (((userPath[i])%P)*calcModPow(d,window-i-1))%P;
        i--;
    }
    hIndexPrev = hIndexPrev%P;

    //Calculate Path for first window
    string hValPrev;
    ostringstream str1;
    int j =0;
    while(j!=window){
        if(j==window-1)
            str1<<userPath[j];
        else
            str1<<userPath[j]<<",";
        j++;
    }
    hValPrev = str1.str();


    //Check if calculated path match the hashed path
    matched = CheckHashTable(hIndexPrev,hValPrev);

    //If matched return path
    if(matched)
        return hValPrev;

    //Else check for each subsequent windows of given size
    else{
        string hVal;
        int hIndex;
        for(int i =1;!matched&&i<userPathLength-window+1;i++){
            //Calculate Index for window i
            int hIndexSub = (hIndexPrev-((userPath[i-1]%P)*calcModPow(d,window-1))%P);
            if(hIndexSub<0)
                hIndexSub+=P;
            hIndex = ((((hIndexSub%P)*d)%P)+(userPath[i+window-1]))%P;
            hIndexPrev = hIndex;

            //Calculate Path for window i
            ostringstream str1;
            if(window==1)
                str1<<userPath[i+window-1];
            else{
                str1<<hValPrev.substr(hValPrev.find(",")+1);
                str1<<","<<userPath[i+window-1];
            }
            hVal = str1.str();
            hValPrev = hVal;

            //Check if calculated path matched or not
            matched = CheckHashTable(hIndex,hVal);

    }

    //If matched return path
    if(matched)
        return hVal;

    //else return null string
    else if(!matched)
        return "";
    }
}

//To check if given entry is existing in hash table or not
bool CheckHashTable(int indexCheck,string valCheck){
    bool found = false;
    for(int i = 0;!found&&i<P;i++){
        if(RHT[(indexCheck+i)%P]==nullptr){
            return false;
        }else if(RHT[(indexCheck+i)%P]->val==valCheck){
            return true;
        }
    }
}

//To reset hash table for every new window size
void ResetRHT(){
    for(int i =0;i<P;i++){
        RHT[i] = nullptr;
    }
}

//To calculated the modulo value of different powers of 1000 used in calculating index
int calcModPow(int val,int power){
    int result = 1;
    while(power!=0){
        result = (result*val)%P;
        power--;
    }
    return result;
}
