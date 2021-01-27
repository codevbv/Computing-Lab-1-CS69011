#include <iostream>
#include <fstream>

using namespace std;

//Prototyping
void ParseInputToUserStruct(string ip);
void MaxCommonSubPath_MCSP(int *user1,int *user2,int u1PathLength,int u2PathLength);


//For data of each user entry
struct user{
    char uId[20];
    int uPath[1000];
    int pathLength=0;
};

//Array of pointers to each user struct
user *ArrUser[2];
int userCount = 0;

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
    
    //Fill user struct by parsing ip file
    ParseInputToUserStruct(ip);
    
    //Retrieve Parameters for Maximal common sub path function from user struct
    int *user1Path = ArrUser[0]->uPath;                                                   
    int *user2Path = ArrUser[1]->uPath;                                                  
    int user1PathLength = ArrUser[0]->pathLength;                                         
    int user2PathLength = ArrUser[1]->pathLength;
      
    //Maximal common sub path                                       
    cout<<"The Maximal sub path  for "<<ArrUser[0]->uId<<" and "<<ArrUser[1]->uId<<" is : "<<endl;
    MaxCommonSubPath_MCSP(user1Path,user2Path,user1PathLength,user2PathLength);

    return 0;
}

void ParseInputToUserStruct(string ip){
    
    //Open the input file with given name
    ifstream ipFile;
    ipFile.open(ip);

    //Parse, Tokenize and store entry of each user.
    while(!ipFile.eof()&&userCount<2){
            
        //Create entry in struct for user
        ArrUser[userCount] = new user;
    
        //Maintain User count
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
        
        //Update final path length for user
        ArrUser[userCount]->pathLength = length;                                  
        userCount++;
        
        //Skip '\n' character
        ipFile.get();                                                            
    }
}

//DP function to find maximal common sub path among given users
void MaxCommonSubPath_MCSP(int *user1,int *user2,int u1PathLength,int u2PathLength){
    
    //Table to compute Maximal subPath legth
    int t_MCSP[u1PathLength+1][u2PathLength+1];
    
    //Length of Max sub path
    int Length_MCSP = 0;
    
    //Row and Column number of last vertex of maximum sub path
    int Row_MCSP,Col_MCSP;
    
    for(int i =0;i<u1PathLength+1;i++){
        for(int j =0;j<u2PathLength+1;j++){
            //If either string empty then fill 0
            if(i==0||j==0)
                t_MCSP[i][j] =0;
            else{
                //If Match then fill cell and record its row and column.
                if(user1[i-1]==user2[j-1]){
                    t_MCSP[i][j] = 1+t_MCSP[i-1][j-1];
                    if(Length_MCSP<t_MCSP[i][j]){
                        Length_MCSP = t_MCSP[i][j];
                        Row_MCSP = i;
                        Col_MCSP = j;
                    }
                }
                //If not matched start a fresh
                else{
                    t_MCSP[i][j] = 0;
                }
            }
        }
    }

    //If no common sub path found
    if(Length_MCSP==0){
        cout<<"There is no common sub path between given users "<<endl;
        return;
    }
    
    //Integer array to store maximal common sub path(MSP) vertices
    int MCSP[Length_MCSP];
    int PathIndex = Length_MCSP;
    while(t_MCSP[Row_MCSP][Col_MCSP]!=0){
        MCSP[--PathIndex] = user1[Row_MCSP-1];
        Row_MCSP--;
        Col_MCSP--;
    }
    
    //Print the Maximal Common Sub Path
    cout<<MCSP[PathIndex++];
    while(PathIndex!=Length_MCSP){
        cout<<","<<MCSP[PathIndex++];
    }
    cout<<"\n";

}
