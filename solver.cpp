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
    
    /*
    *   Realized right after I typed this that this was useless. The sudoku class is the one where the dynamic memory is allocated.
    *   I think it's best if the deletions happen there as well. No point writing a destructor that will make the pointers null either.
    *   Deleted memory is not a concern.
    // Destructor to deallocate all dynamic memory
    ~partition()
    {
        for (int i = 0; i < 9; i++)
        {
            delete(array[i]);   
        }
    }
    */
    
    // Checks to see what all numbers have already been set in this partition and updates all squares. Next, solves whatever it can.
    // If no functions run, returns false to signify that no more operations are possible. Returns true otherwise.
    bool solve()
    {
        // Is used to see if atleast one called function returns true
        bool returnVal = false;
        
        // Checks what numbers are in each square, and updates existing as required.
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
            returnVal |= array[i]->update(existing);
        }
        
        // Solves whatever possible
        for (short i = 0; i < 9; i++)
        {
            returnVal |= array[i]->finalize();
        }
        
        return returnVal;
    }
};

// Class that represents the actual sudoku. It is a composition class of 81 element objects, and 27 partition objects
class sudoku
{
private:
    partition* rows[9];     // Contains references to the squares that constitute the 9 rows of the sudoku
    partition* columns[9];  // Contains references to the squares that constitute the 9 columns of the sudoku
    partition* boxes[9];    // Contains references to the squares that constitute the 9 3X3 boxes of the sudoku
    element* squares[9][9]; // Contains references to ALL the squares that constitute the sudoku,
                            // as a two dimensional array that is in the shape of the real sudoku

public:
    // Constructor. Requirements: An array that has the 81 numbers that represent a sudoku
    sudoku(short array[81])
    {
        // Creates all the square elements, to store each square of the sudoku
        for (int i = 0; i < 81; i++)
        {
            squares[i%9][i/9] = new element(array[i]);
        }
        
        // Creates all the rows using the constructor
        for (int i = 0; i < 9; i++)
        {
            element* temp[9];
            
            for (int j = 0; j < 9; j++)
            {
                temp[j] = squares[i][j];
            }
            
            rows[i] = new partition(temp);
        }
        
        // Creates all the columns using the constructor
        for (int i = 0; i < 9; i++)
        {
            element* temp[9];
            
            for (int j = 0; j < 9; j++)
            {
                temp[j] = squares[j][i];
            }
            
            columns[i] = new partition(temp);
        }
        
        /*
        * I'm a bit loose with how rows and columns are defined here. Technically, rows are horizontal, and columns are vertical.
        * In truth, this doesn't matter at all for solving, I just need to represent both. The names are irrelevant.
        */
        
        // Outer loop iterates through the columns of boxes
        for (int i = 0; i < 3; i++)
        {
            // Inner loop iterates through the rows of boxes
            for (int j = 0; j < 3; j++)
            {
                element* temp[9];
                // i*3 and j*3 give box start coordinates
                // Outer loop iterates through each column of the box
                for (int k = 0; k < 3; k++)
                {
                    // Inner loop iterates through each row of the box
                    for (int l = 0; l < 3; l++)
                    {
                        temp[3*k+l] = squares[i*3 + k][j*3 + l];
                    }
                }
                
                boxes[3*i + j] = new partition(temp);
            }
        }
    }
    
    // Deletes ALL dynamic memory created to represent things in the sudoku.
    ~sudoku()
    {
        // Deletes partitions first, to avoid any mishaps.
        for (int i = 0; i < 9; i++)
        {
            delete(rows[i]);
            delete(columns[i]);
            delete(boxes[i]);
        }
        
        // Deletes each individual element.
        for (int i = 0; i < 81; i++)
        {
            delete(squares[i%9][i/9]);
        }
    }
};


// I forgot to mention in the previous commits. main(), as of now, is simply for testing purposes
// It will only be properly finished once I complete the classes entirely
int main()
{
    short array[81];
    
    for (int i = 0; i < 81; i++)
    {
        array[i] = i%9;
    }
    
    sudoku s(array);
    
    cout<<"Hello World\n";
    cout<<((0b101)^(0b110))<<endl;
    return 0;
}