#include <iostream>
#include <fstream>


using namespace std;

struct user_Node{
    int uId;
    user_Node *nextUser;
};

struct content_Node{
    int cId;
    content_Node *nextContent;
};

struct user_Content_Node{
    user_Node *userList;
    int users;
    int uId;
    content_Node *contentList;
    int contents;
};

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

    // function to insert an element
    // at the rear of the queue
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

        // shift all the elements from index 2 till rear
        // to the left by one
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

    bool isEmpty(){
        if (frontQ == rearQ)
            return true;
        return false;
    }
};

//Prototyping
int getUserCount(string input);
void topKMinDist(user_Content_Node *G, int start, int * nodes, int *distances,int k,int numUsers,int numContents);
int ParseInputToGraph(string ip,user_Content_Node *G,int numUsers);
void printUserList(user_Content_Node *G,int numUsers);
void printContentList(user_Content_Node *G,int numUsers);



//Takes input file name from command line arg
int main(int argc,char *argv[])
{
    //To store name of input file
    string ip;

    //To store the source node id
    int start;

    //To store the number of contents to be recommended
    int k;

    //if no command line arg given
    if(argc==1){
	cout<<"Enter the name of ip file with extension .txt , the start node and value of k"<<endl;
	cin>>ip>>start>>k;
    }

    //If command line given
    else{
    	ip = argv[1];
        start = atoi(argv[2]);
        k = atoi(argv[3]);
	}

    //Retrieve number of users to construct adjacency list for user-content graph G
    int numUsers = getUserCount(ip);

    //Construct adjacency list of user-content Graph G
    user_Content_Node *G = new user_Content_Node[numUsers];

    //Parse the give .txt file to array pointer to of user struct and retrieve total number of contents
    int numContents = ParseInputToGraph(ip,G,numUsers);

    int nodes[k];
    int distances[k];

    //Run the function to print the list of k contents ranked according to their distance from given user.
    topKMinDist(G,start,nodes,distances,k,numUsers,numContents);

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

//Function to enlist/recommend k contents for the given user
void topKMinDist(user_Content_Node *G, int start, int *nodes, int *distances,int k,int numUsers,int numContents){

    //Array to store that the content has been already put in recommended list or not
    bool pickedContents[numContents];
    for(int i =0;i<numContents;i++)
        pickedContents[i] = false;

    //If the k entered is invalid
    content_Node *tempSC = G[start].contentList;
    if(k>(numContents-G[start].contents)){
        cout<<"The number of contents to be recommended for given user are greater than set of available contents : Invalid K "<<endl;
        return;
    }

    //Make all the contents which are already shared by the start user as true  they are not to be recommended
    while(tempSC){
        int startContent = tempSC->cId;
        pickedContents[startContent] = true;
        tempSC= tempSC->nextContent;
    }

    //Store the number of contents to be recommended (For printing purpose)
    int numRecommend = k;

    //Array to store level/distance of each node
    int level[numUsers];

    //Array to store that node is visited/marked or not
    bool marked[numUsers];

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

        //For all nodes other than the source node calculate and fill 'nodes' and 'distances' array
        if(curUser!=start){

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
                    if(pickedContents[curContent]==false){

                        //Update the content recommendation array
                        nodes[nodeIndex++] = curContent;

                        //Update the distance of that content from user array
                        distances[distanceIndex++] = level[curUser]+1;

                        //Mark that content as picked so that it should be skipped if seen next time
                        pickedContents[curContent] = true;

                        //Decrement th number of contents to be picked
                        k--;
                    }

                    //Move to the next content of current user
                    tempC = tempC->nextContent;
                }
            }

            //If at any point the k contents are found then break from BFS loop
            if(k<=0)
                break;
        }
    }

    // display all contents and their distance from source user
    cout << "Contents"
         << "  "
         << "Distances" << endl;
    for (int i = 0; i<numRecommend; i++){
        if(nodes[i]>=0&&nodes[i]<numContents&&distances[i]<numUsers+1)
            cout << " " <<nodes[i]<< "   -->   " << distances[i] << endl;
    }

}
/*void printUserList(user_Content_Node *G,int numUsers){
    for(int i =0;i<numUsers;i++){
        cout<<"This is for user "<<i<<endl;
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
        cout<<"This is for user "<<i<<endl;
        content_Node *temp = G[i].contentList;
        while(temp){
            cout<<temp->cId<<",";
            temp = temp->nextContent;
        }
        cout<<endl;
    }
}*/

