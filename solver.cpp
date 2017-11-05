/*
* Is the brain of the application. Inputs a command line argument that describes a given unsolved Sudoku table.
* Handles the conversion of that input into usable formats, and then solve the Sudoku, and then returns the solved sudoku.
*/

// For stream operations
#include <iostream>

/*
*   Note to future self and anyone who is curious as to why I didn't do this:
#define SUDOKU_SIZE 9;
*
*   Well I happened to realize this was pointless right after I typed that statement. First and foremost, the bitflags won't work
*   Secondly, any number other than 9 for the size of a sudoku would mess this whole thing up pretty bad, and I don't expect to ever solve
*   anything like a 6X6 or whatever.
*/


// For streams
using namespace std;

// Each individual sqaure in a sudoku is going to be modelled as an element object
class element
{
private:
    short number;           // Stores the value that is supposed to be in the square. If value is zero, this means that the value hasn't been determined yet.
    int possibilities;      // Stores 9 flags in one integer using bitwise operators.
                            // possibilities is 0b000000000 if number is already finalized, else the respective digit from the right is 1 if it is a possibility, and 0 if not
                            // Example: if 1,2 and 9 are likely candidates for number, then possibilities is 0b100000011
public:
    // Constructor
    // Requirements: ANY number between 0 and 9, both included, but none other. Any other input can and most probably will cause the program to fail
    element(short input)
    {
        // Checks if the input for this square is a given clue
        if (input)
        {
            possibilities = 0b000000000;    // If yes, set it to be a permanent square
        }
        else
        {
            possibilities = 0b111111111;    // If not, set the sqaure open to the solver's solving algorithm
        }
        
        number = input;     // Sets the actual number
    }
    
    // Returns true if a value can be finalized
    bool finalize()
    {
        if (possibilities == 0)
        {
            return false;
        }
        
        // Iterates through all values of i for the correct bitshifts
        for (short i = 0; i < 9; i++)
        {
            // Checks all flags using a bitshift
            if (possibilities == (1<<i))
            {
                number = i + 1;     // Sets the final value for number if all else has been eliminated.
                                    // REMEMBER!! i is a bitshift variable, so ONE HAS to be added to get the right value for number
                possibilities = 0b000000000;  // Sets the square to be permanent, to prevent future edits
                return true;        // Returns true to show the succesful filling of this square.
            }
            else
            {
                return false;       // Returns false to say that a finalized value was not set.
            }
        }
    }
    
    // Inputs a set of bitflags that can be crossed off the list of possibilities. Updates possibilities to reflect this
    bool update(short input)
    {
        int check = possibilities;
        possibilities &= (0b111111111)^input;       // Sets relevant flag bit to 0 if it was a 1 earlier.
                                                    // REMEMBER!! input is the number, so bitshift is one less than input
        if (possibilities == check)
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    // Returns the number inside the square
    short getNumber()
    {
        return number;
    }
};

// Class that represents sudoku partitions like the big 3X3 square, or a row or column.
class partition
{
private:
    element* array[9];  // Stores pointers to the 9 squares that are a part of this partition
    int existing;       // Has bitflags set to 1 if the number already exists in this particular partition

public:
    // Constructor. Requirements: An array of pointers to this partition's squares
    partition(element* squares[9])
    {
        // Fills up array with the pointers to the squares
        for (int i = 0; i < 9; i++)
        {
            array[i] = squares[i];
        }
        // Sets existing up for check()
        existing = 0b000000000;
    }
    // Checks to see what all numbers have already been set in this partition and updates all squares
    bool solve()
    {
        bool returnVal = true;
        
        // Checks what numbers are in each square, and updates existing as required. Next, solves whatever it can.
        for (short i = 0; i < 9; i++)
        {
            if (array[i]->getNumber())
            {
                existing |= (1<<(array[i]->getNumber() - 1));
            }
        }
        
        // Updates every square
        for (short i = 0; i < 9; i++)
        {
            array[i]->update(existing);
        }
        
        // Solves
        for (short i = 0; i < 9; i++)
        {
            array[i]->finalize();
        }
    }
};

int main()
{
    cout<<"Hello World\n";
    cout<<((0b101)^(0b110))<<endl;
    return 0;
}