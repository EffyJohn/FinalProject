/*
* Is the brain of the application. Inputs a command line argument that describes a given unsolved Sudoku table.
* Handles the conversion of that input into usable formats, and then solve the Sudoku
*/
#include <iostream>

using namespace std;

class element
{
private:
    short number;
    int possibilities;
};

int main()
{
    cout<<"Hello World\n";
    cout<<sizeof(int)<<endl;
    return 0;
}