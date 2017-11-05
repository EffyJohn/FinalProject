/*
* Is the brain of the application. Inputs a command line argument that describes a given unsolved Sudoku table.
* Handles the conversion of that input into usable formats, and then solve the Sudoku, and then returns the solved sudoku.
*/

// For stream operations
#include <iostream>

// For streams
using namespace std;

// Each individual sqaure in a sudoku is going to be modelled as an element object
class element
{
private:
    short number;           // Stores the value that is supposed to be in the square. If value is zero, this means that the valu hasn't been determined yet.
    int possibilities;      // Stores 9 flags in one integer using bitwise operators.
                            // possibilities is 0b000000000 if number is already finalized, else the respective digit from the left is 1 if it is a possibility, and 0 if not
                            // Example: if both 1,2 and 9 are likely candidates for number, then possibilities is 0b110000001
};

int main()
{
    cout<<"Hello World\n";
    cout<<sizeof(int)<<endl;
    return 0;
}