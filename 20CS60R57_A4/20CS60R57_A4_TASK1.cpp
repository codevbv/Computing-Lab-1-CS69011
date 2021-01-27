#include <iostream>
#include<fstream>
#include <glpk.h>

using namespace std;

void getNumUnit(string input,int *arr);

int main(int argc , char* argv[])
{
    string ip;
    if(argc==1){
	cout<<"Enter the name of ip file with extension .txt "<<endl;
	cin>>ip;
    }
    else{
    	ip = argv[1];
    }
    //Fill dimensions i.e n and m
    int dimension[2];
    getNumUnit(ip,dimension);
    int n = dimension[0];
    int m = dimension[1];

    //initialize input arrays
    int u[n],d[m],cost[n][m],Coeff[n][m];
    int len = 2 + n + m + n*m;
    int arr[len];

    //Open file
    ifstream ipFile;
    ipFile.open(ip);

    //Fill all integers in file in a single array
    for(int a = 0; a <len; a++)
        ipFile >> arr[a];

    int i =2;
    //Fill Unit array
    for(int j = 0;j<n;j++)
        u[j] = arr[i++];

    //Fill demand array
    for(int j=0;j<m;j++)
        d[j] = arr[i++];

    //Fill cost array
    for(int p=0;p<n;p++){
        for(int q=0;q<m;q++){
            cost[p][q] = arr[i++];
            Coeff[p][q] = cost[p][q]*d[q];
        }
    }

    // object in which we store our problem statement
    glp_prob *lp;
    lp = glp_create_prob(); // create a problem object , in which all data is stored
    glp_set_prob_name(lp,"A4_Task1");

    // set objective function
    glp_set_obj_dir(lp, GLP_MIN); // set objective direction(min,max)

    // create n*m columns ( variables )
    int numCols = n*m;
    glp_add_cols(lp,numCols);
    char nameCol[20];
    for(int i=0;i<n;i++){
        for(int j =0;j<m;j++){
            sprintf(nameCol, "x%d%d",i+1,j+1);
            glp_set_col_name(lp,i*m+j+1,nameCol);
            glp_set_col_bnds(lp,i*m+j+1, GLP_DB, 0.0, 1.0);
            glp_set_obj_coef(lp,i*m+j+1,Coeff[i][j]);
        }
    }

    // set row bounds
    int numRows = n+m;
    char nameRow[20];
    glp_add_rows(lp,numRows);
    for(int i = 0;i<n;i++){
        sprintf(nameRow, "Unit%d",i+1);
        glp_set_row_bnds(lp,i+1, GLP_DB, 0.0,u[i]);
    }
    for(int j =n+1;j<=numRows;j++){
        sprintf(nameRow, "BattleGround%d",j);
        glp_set_row_bnds(lp,j, GLP_LO,1.0,0.0);
    }

    // set row coefficients
    int sizeMatrix = numRows*numCols+1;
    int ia[sizeMatrix], ja[sizeMatrix];
    double ar[sizeMatrix];
    for(int i =0;i<numRows;i++){
        for(int j =0;j<numCols;j++){
            ia[i*numCols+j+1] = i+1;
            ja[i*numCols+j+1] = j+1;
            ar[i*numCols+j+1] = 0.0;
            if(i<n){
                int from = i*m;
                int to = from + m;
                if(j>=from&&j<to)
                    ar[i*numCols+j+1] = d[j-from];
            }else if(i>=n&&i<numRows){
                int calc = i-n+1;
                if((j+1)%m==calc%m)
                    ar[i*numCols+j+1] = 1.0;
            }
        }
    }

    // load the matrix
    glp_load_matrix(lp,sizeMatrix-1,ia,ja,ar);

    // call the solver
    glp_simplex(lp, NULL);

    //Print input

    //The values of n,m are
    cout<<"The inputs n and m are: "<<endl
    cout<<n<<"      "<<m<<endl;

    //Print input vectors

    //Fill Unit array
    cout<<"The u vector as given in input is : "<<endl;
    for(int j = 0;j<n;j++)
        cout<<u[j]<<"   ";
    cout<<endl;

    //Fill demand array
    cout<<"The d vector as given in input is : "<<endl;
    for(int j=0;j<m;j++)
        cout<<d[j]<<"   ";
    cout<<endl;

    //Fill cost array
    cout<<"The cost vector as given in input is : "<<endl;
    for(int p=0;p<n;p++){
        for(int q=0;q<m;q++){
            cout<<cost[p][q]<<"   ";
        }
        cout<<endl;
    }
    cout<<endl;

    // get solution
    double z,x;
    z = glp_get_obj_val(lp);
    cout<<"The final optimal cost is : "<<z<<endl;
    cout<<"The variable values for which the optimal solution came is :"<<endl;
    for(int i =0;i<n;i++){
        for(int j = 0;j<m;j++){
            x = d[j]*glp_get_col_prim(lp,i*m+j+1);
            cout<<x<<"      ";
        }
        cout<<endl;
    }

    //Delete problem
    glp_delete_prob(lp);
    return 0;
}

void getNumUnit(string input,int *arr){
    //Open the input file with given name
    ifstream ipFile;
    ipFile.open(input);
    int x = 0;
    while(ipFile.peek()!=' ')
        x = x*10 + (ipFile.get()-48);
    arr[0] = x;
    x = 0;
    ipFile.get();
    while(ipFile.peek()!='\n')
        x = x*10 + (ipFile.get()-48);
    arr[1] = x;
    ipFile.get();
}
