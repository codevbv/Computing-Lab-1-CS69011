#include <iostream>
#include <fstream>
#include <sstream>


using namespace std;

//The user node struct
struct user_Node{
    int uId;
    user_Node *nextUser;
};

//The content node struct
struct content_Node{
    int cId;
    content_Node *nextContent;
};

//The input graph nodes struct
struct user_Content_Node{
    user_Node *userList;
    int users;
    int uId;
    content_Node *contentList;
    int contents;
};

//Open list for BFS
struct OpenList{
    int frontQ, rearQ, capacityQ;
    int* Queue;
    OpenList(int c)
    {
        frontQ = rearQ = 0;
        capacityQ = c;
        if(c>50){
            int q[c];
            Queue = q;
        }
        else{
            Queue = new int;
        }
    }

    ~OpenList()
    {
        delete[] Queue;
    }

    // function to insert an element at the rear of the queue
    void OpenListAdd(int data)
    {
        // check queue is full or not
        if (capacityQ == rearQ) {
            cout<<"\nOpen list is  full\n";
            return;
        }

        // insert element at the rear
        else {
            Queue[rearQ] = data;
            rearQ++;
        }
        return;
    }

    // Function to delete an element from the front of openList
    void OpenListRemove()
    {
        // if queue is empty
        if (frontQ == rearQ) {
            cout<<"\nQueue is  empty"<<endl;
            return;
        }

        // shift all the elements from index 2 till rear to the left by one
        else {
            for (int i = 0; i <rearQ - 1; i++) {
                Queue[i] = Queue[i + 1];
            }

            // decrement rear
            rearQ--;
        }
        return;
    }

    // Return first element of open list
    int OpenListFront()
    {
        if (frontQ == rearQ) {
            cout<<"\nQueue is Empty"<<endl;
            return -1;
        }
        return Queue[frontQ];
    }
    //Function to check if open list is empty or not
    bool isEmpty(){
        if (frontQ == rearQ)
            return true;
        return false;
    }

};

//Prototyping
int getUserCount(string input);
int ParseInputToGraph(string ip,user_Content_Node *G,int numUsers);
int topKMinDist(user_Content_Node *G, int start, int *nodes, int *distances,int k,int numUsers,int numContents);
int connectedUserNodes(user_Content_Node *G,int start,int *nodes,int *distances,int k,int L,int totContents,int numUsers);
void InitUserOdd(user_Content_Node *G,int curContent,bool *usersOdd,int numUsers);
void printUserList(user_Content_Node *G,int numUsers);
void printContentList(user_Content_Node *G,int numUsers);
bool isAdjacent(int i,int *adjacent,int numAdj);



//Takes input file name from command line arg
int main(int argc,char *argv[])
{
    string ip;
    int start;
    int k,L ;
    if(argc==1){
	cout<<"Enter the name of ip file with extension .txt , The source node,the value of k and the value of L"<<endl;
	cin>>ip>>start>>k>>L;
    }
    else{
    	ip = argv[1];
    	start = atoi(argv[2]);
    	k = atoi(argv[3]);
    	L = atoi(argv[4]);
    }

    //Retrieve number of users to construct adjacency list for user-content graph G
    int numUsers = getUserCount(ip);

    //Construct adjacency list of user-content Graph G
    user_Content_Node *G = new user_Content_Node[numUsers];

    //Parse the give .txt file to array pointer to of user struct and retrieve total number of contents
    int numContents = ParseInputToGraph(ip,G,numUsers);

    int nodes[numContents];
    int distances[numContents];

    //Run the function to print the list of k contents ranked according to their distance from given user.
    int totContents = topKMinDist(G,start,nodes,distances,k,numUsers,numContents);

    connectedUserNodes(G,start,nodes,distances,k,L,totContents,numUsers);

    return 0;
}

//Function to parse the number of users from input file
int getUserCount(string input){
    //Open the input file with given name
    ifstream ipFile;
    ipFile.open(input);
    int x = 0;
    while(ipFile.peek()!='\n'){
        if(ipFile.peek()=='\r')
            break;
        x = x*10 + (ipFile.get()-48);
    }
    return x;
}

//Function to store input file in form of user-content Graph G and retrieve total number of contents
int ParseInputToGraph(string ip,user_Content_Node *G,int numUsers){
    //Open the input file with given name
    ifstream ipFile;
    ipFile.open(ip);


    //Skip count of users as already retrieved
    while(ipFile.peek()!='\n'){
        if(ipFile.peek()=='\r')
            break;
        ipFile.get();
    }

    //Skip new line or carriage return
    ipFile.get();

    //For each user fill the adjacent user list
    for(int i = 0 ;!ipFile.eof()&&i<numUsers;i++){

        //Fill user ID for each user
        int result = 0;
        while(ipFile.peek()!=':'&&!ipFile.eof()){
             result = result*10 + (ipFile.get()-48);
        }
        G[result].uId = result;

        //Skip ':'
        ipFile.get();

        //If user not connected to any user than assign null to its userList pointer
        if(ipFile.peek()=='\n'||ipFile.peek()=='\r'){
            ipFile.get();
            G[result].userList = nullptr;
            G[result].users = 0;
            continue;
        }

        //Initialize the pointer to adjacent users list
        user_Node *temp = new user_Node;
        G[result].userList = temp;

        //Variable to keep track of number of neighbors of particular user
        int numNeighbor = 0;

        //Traverse input till new line character to fill all the adjacent users in user list
        while(!ipFile.eof()&&(ipFile.peek()!='\n'||ipFile.peek()!='\r')){

            if(ipFile.peek()==',')
                ipFile.get();
            else{
                //Convert Characters to integer for each adjacent user and check for valid user entry
                int x =0;
                while(ipFile.peek()!=','&&ipFile.peek()!='\n'&&!ipFile.eof()){
                    if(ipFile.peek()=='\n'||ipFile.peek()=='\r')
                        break;
                    x = x*10 + (ipFile.get()-48);
                }
                if(x>=0&&x<=numUsers){
                    temp->uId = x;
                    numNeighbor++;
                }
                else{
                    cout<<"Invalid entry for Path vertex "<<endl;
                    return 0;
                }

                //If new line not reached then make a new user entry in the user list
                if(ipFile.peek()!='\n'&&ipFile.peek()!='\r'){
                    temp->nextUser = new user_Node;
                    temp = temp->nextUser;
                }

                //If new line reached the terminate user list with null pointer and skip new line character
                else{
                    ipFile.get();
                    temp->nextUser = nullptr;
                    break;
                }
            }
        }

        //Update the number of neighboring users for current user i
        G[result].users = numNeighbor;
    }

    //Retrieve the number of contents
    int numContents = 0;
    while(ipFile.peek()!='\n'){
        if(ipFile.peek()=='\r')
            break;
        numContents = numContents*10 + (ipFile.get()-48);
    }

    //Skip new line
    ipFile.get();

    //For each user fill the shared content list
    for(int i = 0 ;!ipFile.eof()&&i<numUsers;i++){

        //Fill user ID for each user
        int result = 0;
        while(ipFile.peek()!=':'&&!ipFile.eof()){
             result = result*10 + (ipFile.get()-48);
        }

        //Skip ':'
        ipFile.get();

        //If user not sharing any content than assign null to its contentList pointer
        if(ipFile.peek()=='\n'||ipFile.peek()=='\r'||ipFile.eof()){
            ipFile.get();
            G[result].contentList = nullptr;
            G[result].contents = 0;
            continue;
        }

        //Initialize the pointer to shared contents list
        content_Node *temp = new content_Node;
        G[result].contentList = temp;

        //Variable to keep track of number of contents per user.
        int numShared = 0;

        //Traverse input till new line character to fill all the shared contents in content list
        while(!ipFile.eof()&&(ipFile.peek()!='\n'||ipFile.peek()!='\r')){

            if(ipFile.peek()==',')
                ipFile.get();
            else{
                //Convert Characters to integer for each content and check for valid content entry
                int x =0;
                while(ipFile.peek()!=','&&ipFile.peek()!='\n'&&!ipFile.eof()){
                    if(ipFile.peek()=='\n'||ipFile.peek()=='\r')
                        break;
                    x = x*10 + (ipFile.get()-48);
                }
                if(x>=0&&x<=numContents){
                    temp->cId = x;
                    numShared++;
                }
                else{
                    cout<<"Invalid entry for content "<<endl;
                    return 0;
                }

                //If new line not reached then make a new content entry in the content list
                if(ipFile.peek()!='\n'&&ipFile.peek()!='\r'&&ipFile.peek()==','){
                    temp->nextContent = new content_Node;
                    temp = temp->nextContent;
                }

                //If new line reached the terminate user list with null pointer and skip new line character
                else{
                    ipFile.get();
                    temp->nextContent = nullptr;
                    break;
                }
            }
        }

        //Update the count of total shared contents for user i
        G[result].contents = numShared;
    }

    //Return total number of contents
    return numContents;
}



//Function to find all contents whose distance is less than k from the given user
int topKMinDist(user_Content_Node *G, int start, int *nodes, int *distances,int k,int numUsers,int numContents){

    //Array to store that the content has been already put in recommended list or not
    bool pickedContents[numContents];
    for(int i =0;i<numContents;i++)
        pickedContents[i] = false;


    //Array to store level/distance of each node
    int level[numUsers];

    //Array to store that node is visited/marked or not
    bool marked[numUsers];

    for(int i =0;i<numUsers;i++){
        level[i] = 0;
        marked[i] = false;
    }
    //Create a open list
    OpenList open(numUsers);

    //Add source element to open list
    open.OpenListAdd(start);

    //Initialize level of source node to 0
    level[start] = 0;

    //Mark source as visited
    marked[start] = true;

    //Index to fill the recommended contents array i.e 'nodes'
    int nodeIndex = 0;

    //Index to fill the distance from user to content array i.e 'distances'
    int distanceIndex = 0;

    //Boolean to keep track that only those contents should be picked whose distance is less than k
    bool kPicked = false;

    //Do until open list is empty
    while (!open.isEmpty()) {

        //Get the first element of open list
        int curUser  = open.OpenListFront();

        //Remove first element of open list
        open.OpenListRemove();

        //Traverse neighbors of curUser
        user_Node *temp = G[curUser].userList;

        while(temp){
            //Friend is neighbor of curUser
            int Friend = temp->uId;


            //Friend is not marked already
            if(!marked[Friend]) {

                //Add Friend in open list
                open.OpenListAdd(Friend);

                //Level of Friend is Level of curUser+1
                level[Friend] = level[curUser] + 1;

                //Mark Friend as visited
                marked[Friend] = true;
            }

            //Go to the next neighbor
            temp = temp->nextUser;
        }


        //Number of contents shared by the current user
        int numContent = G[curUser].contents;

        //If content shared are zero than skip that user
        if(numContent!=0){

        //Initialize pointer to contentList of current user
        content_Node *tempC = G[curUser].contentList;

        //Check and fill content table till all contents of current user are not seen
        while(tempC){

            //Retrieve value of current content
            int curContent = tempC->cId;

            //If content already picked than skip this part
            if(pickedContents[curContent]==false&&(level[curUser]+1<=k)){

                //Update the content recommendation array
                nodes[nodeIndex++] = curContent;

                //Update the distance of that content from user array
                distances[distanceIndex++] = level[curUser]+1;

                //Mark that content as picked so that it should be skipped if seen next time
                pickedContents[curContent] = true;

                }

            //Move to the next content of current user
            tempC = tempC->nextContent;
            }
        }

        //If content is at distance greater than equal to k then no need to take that content
        if(level[curUser]+1>k){

            break;
        }
    }
    /*cout << "Contents"
         << "  "
         << "Distances" << endl;
    for (int i = 0; i<nodeIndex; i++){
        if(nodes[i]!=0||distances[i]!=0)
            cout << " " <<nodes[i]<< "   -->   " << distances[i] << endl;
    }*/
    return nodeIndex;
}

int connectedUserNodes(user_Content_Node *G,int start,int *nodes,int *distances,int k,int L,int totContents,int numUsers){
    //Array to store the just adjacent users of source as they should not be recommended
    int numAdjacent = G[start].users;
    int startAdjacent[numAdjacent];
    user_Node *adj = G[start].userList;
    int adjIndex = 0;
    while(adj){
        startAdjacent[adjIndex++] = adj->uId;
        adj = adj->nextUser;
    }

    //Array to store final recommended users and there distance
    int connectedUsers[L];
    int distConnectedUsers[L];

    //Array to maintain the count of common contents among neighbors
    int kNeighborCount[numUsers];
    for(int i = 0;i<numUsers;i++){
        kNeighborCount[i] = 0;
    }

    //Array to store that content is common to user or not
    bool marked[numUsers];

    for(int i =0;i<numUsers;i++){
        marked[i] = false;
    }


    //For each content see that to which k neighbors it is common
    for(int i =0;i<totContents;i++){

        int kInit = k;

        //Current content
        int curContent = nodes[i];

        //Get the users which are direct linked with current conten
        InitUserOdd(G,curContent,marked,numUsers);
        int oldNodes = 0;
        int newNodes = 0;

        //Create a open list
        OpenList open(numUsers);

        //Add directly linked users to open list
        for(int i =0;i<numUsers;i++){
            if(marked[i]){
                open.OpenListAdd(i);
                oldNodes++;
                //level[i] = 1;
            }
        }

        kInit--;

        //BFS applied for k neighbors
        while(!open.isEmpty()&&kInit>0){

        //Get the first element of open list
        int curUser  = open.OpenListFront();


        //Remove first element of open list
        open.OpenListRemove();
        oldNodes--;

        //Traverse neighbors of curUser
        user_Node *temp = G[curUser].userList;

        while(temp){
            //Friend is neighbor of curUser
            int Friend = temp->uId;


            //Friend is not marked already
            if(!marked[Friend]) {
                newNodes++;
                //Add Friend in open list
                open.OpenListAdd(Friend);

                //Mark Friend as visited
                marked[Friend] = true;
            }

            //Go to the next neighbor
            temp = temp->nextUser;
        }

        if(oldNodes==0){
            oldNodes = newNodes;
            newNodes = 0;
            kInit--;
        }
    }

    //Update kNeighborCount array
    for(int i =0;i<numUsers;i++){
        if(marked[i]==true&&i!=start&&!isAdjacent(i,startAdjacent,numAdjacent)){
            kNeighborCount[i]+=1;
        }
    }

    /*cout<<"kNeighbor array for "<<curContent<<endl;
    for(int i =0;i<numUsers;i++){
        cout<<kNeighborCount[i]<<"      ";
    }*/

    //Re initialize the directly linked users for next content in list
    for(int i = 0;i<numUsers;i++){
        marked[i] = false;
        //level[i] = 0;
        }
    }
    for(int i =0;i<L;i++){
        int maxCount = 0;
        int maxCountIndex = 0;
        for(int j =0;j<numUsers;j++){
            if(kNeighborCount[j]>=maxCount){
                maxCount = kNeighborCount[j];
                maxCountIndex = j;
            }
        }
        connectedUsers[i] = maxCountIndex;
        distConnectedUsers[i] = maxCount;
        kNeighborCount[maxCountIndex] = -1;
    }

    int finalRecommendedUsers = 0;
    cout << "Users"
         << "  "
         << "Distances" << endl;
    for (int i = 0; i<L; i++){
        if(connectedUsers[i]!=0||distConnectedUsers[i]!=0){
            if(distConnectedUsers[i]!=0)
                cout << " " <<connectedUsers[i]<< "   -->   " << distConnectedUsers[i] << endl;
            finalRecommendedUsers++;
        }
    }

    return finalRecommendedUsers;

}




void InitUserOdd(user_Content_Node *G,int curContent,bool *marked,int numUsers){
    for(int i = 0;i<numUsers;i++){
        content_Node *tempC = G[i].contentList;
        while(tempC){
            if(tempC->cId==curContent){
                marked[i] = true;
                break;
            }
            tempC= tempC->nextContent;
        }
    }
}

bool isAdjacent(int i,int *adjacent,int numAdj){
    for(int j=0;j<numAdj;j++){
        if(i==adjacent[j])
            return true;
    }
    return false;
}

void printUserList(user_Content_Node *G,int numUsers){
    for(int i =0;i<numUsers;i++){
        cout<<"This is for user "<<G[i].uId<<endl;
        user_Node *temp = G[i].userList;
        while(temp){
            cout<<temp->uId<<",";
            temp = temp->nextUser;
        }
        cout<<endl;
    }
}
void printContentList(user_Content_Node *G,int numUsers){

    for(int i =0;i<numUsers;i++){
        cout<<"This is for user "<<G[i].uId<<endl;
        content_Node *temp = G[i].contentList;
        while(temp){
            cout<<temp->cId<<",";
            temp = temp->nextContent;
        }
        cout<<endl;
    }
}
