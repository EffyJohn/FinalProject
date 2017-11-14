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


// Simply for testing purposes
void print(short* answer)
{
    for (short i = 0; i < 81; i++)
    {
        if (!(i%9))
        {
            cout << endl;
        }
        cout << answer[i];
    }
    
    cout << endl;
}


// Each individual sqaure in a sudoku is going to be modelled as an element object
class element
{
private:
    short number;           // Stores the value that is supposed to be in the square. If value is zero, this means that the value hasn't been determined yet.
    int possibilities;      // Stores 9 flags in one integer using bitwise operators.
                            // possibilities is 0b000000000 if number is already finalized, else the respective digit from the right is 1 if it is a possibility, and 0 if not
                            // Example: if 1,2 and 9 are likely candidates for number, then possibilities is 0b100000011
    short row;              // Stores the row number of this element, a number from 0-8 
    short column;           // Stores the column number of this element, a number from 0-8
    short box;              // Stores the box number of this element, a number from 0-8

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
            possibilities = 0b111111111;    // If not, set the square open to the solver's solving algorithm
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
        }
        
        // For testing only
        if (possibilities == 1 || possibilities == 2 || possibilities == 4 || possibilities == 8 || possibilities == 16 || possibilities == 32 || possibilities == 64 || possibilities == 128 || possibilities == 256)
        {
            cout << possibilities << endl;
        }
        
        
        return false;
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
    
    // See the box variable to understand
    short getBox()
    {
        return box;
    }
    
    // See the row variable to understand
    short getRow()
    {
        return row;
    }
    
    // See the column variable to understand
    short getColumn()
    {
        return column;
    }
    
    // See the box variable to understand
    short setBox(short data)
    {
        box = data;
    }
    
    // See the row variable to understand
    short setRow(short data)
    {
        row = data;
    }
    
    // See the column variable to understand
    short setColumn(short data)
    {
        column = data;
    }
};

// Used in the second step of solving, for higher order puzzles. Simply stores the pointer
// to one element, and has a flag variable named value to store a boolean value that is needed
class flag
{
private:
    element* data;      // Stores the relevant square's pointer
    bool  value;        // Stores the flag value

public:
    flag()
    {
        data = NULL;
        value = false;
    }
    
    void setData(element* input)
    {
        data = input;
    }
    
    element* getData()
    {
        return data;
    }
    
    void setFlag(bool input)
    {
        value = input;
    }
    
    bool getFlag()
    {
        return value;
    }
};


// Class that represents sudoku partitions like the big 3X3 square, or a row or column.
class partition
{
private:
    element* array[9];  // Stores pointers to the 9 squares that are a part of this partition
    int existing;       // Has bitflags set to 1 if the number already exists in this particular partition
    short index;        // Stores a number to denote which row/column/box this is

public:
    // Constructor. Requirements: An array of pointers to this partition's squares, an index to fill this->index, and a char that has c (column), r (row), or b (box)
    partition(element* squares[9], short partitionIndex, char indicator)
    {
        index = partitionIndex;
        // Fills up array with the pointers to the squares
        for (short i = 0; i < 9; i++)
        {
            array[i] = squares[i];
            
            // Sets required indices for each square, to ensure that leg 2 of program works.
            if (indicator == 'c')
            {
                array[i]->setColumn(index);
            }
            else if (indicator == 'r')
            {
                array[i]->setRow(index);
            }
            else if (indicator == 'b')
            {
                array[i]->setBox(index);
            }
        }
        // Sets existing up for solve()
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
    
    element** returnArray()
    {
        return array;
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
        for (short i = 0; i < 81; i++)
        {
            squares[i%9][i/9] = new element(array[i]);
        }
        
        // Creates all the rows using the constructor
        for (short i = 0; i < 9; i++)
        {
            element* temp[9];
            
            for (short j = 0; j < 9; j++)
            {
                temp[j] = squares[i][j];
            }
            
            rows[i] = new partition(temp,i,'r');
        }
        
        // Creates all the columns using the constructor
        for (short i = 0; i < 9; i++)
        {
            element* temp[9];
            
            for (short j = 0; j < 9; j++)
            {
                temp[j] = squares[j][i];
            }
            
            columns[i] = new partition(temp,i,'c');
        }
        
        /*
        * I'm a bit loose with how rows and columns are defined here. Technically, rows are horizontal, and columns are vertical.
        * In truth, this doesn't matter at all for solving, I just need to represent both. The names are irrelevant.
        */
        
        // Outer loop iterates through the columns of boxes
        for (short i = 0; i < 3; i++)
        {
            // Inner loop iterates through the rows of boxes
            for (short j = 0; j < 3; j++)
            {
                element* temp[9];
                // i*3 and j*3 give box start coordinates
                // Outer loop iterates through each column of the box
                for (short k = 0; k < 3; k++)
                {
                    // Inner loop iterates through each row of the box
                    for (short l = 0; l < 3; l++)
                    {
                        temp[3*k+l] = squares[i*3 + k][j*3 + l];
                    }
                }
                
                boxes[3*i + j] = new partition(temp,3*i + j,'b');
            }
        }
    }
    
    // Deletes ALL dynamic memory created to represent things in the sudoku.
    ~sudoku()
    {
        // Deletes partitions first, to avoid any mishaps.
        for (short i = 0; i < 9; i++)
        {
            delete(rows[i]);
            delete(columns[i]);
            delete(boxes[i]);
        }
        
        // Deletes each individual element.
        for (short i = 0; i < 81; i++)
        {
            delete(squares[i%9][i/9]);
        }
    }
    
    // Solves the entire sudoku, by calling (partition object).solve() for every partition.
    // Returns true if even one of these functions returns true. Else returns false to show that the sudoku has been solved.
    // Look at the solve functions for the partition class to understand how this works.
    bool solve()
    {
        bool returnVal = false;
        
        for (short i = 0; i < 9; i++)
        {
            returnVal |= rows[i]->solve();
            returnVal |= columns[i]->solve();
            returnVal |= boxes[i]->solve();
            
            // ONLY FOR TESTING
            print(this->solution());
        }
        
        // Runs a higher order check on boxes
        for (short i = 0; i < 9; i++)   // Iterates through each box
        {
            flag temp[9];
            for (short j = 0; j < 9; j++, temp[i].setData((boxes[i]->returnArray())[j]));   // Initializes the flag objects
            
            for (short j = 0; j < 9; j++)   // Checks for a possible elimination for each number from 1-9
            {
                
            }
        }
        
        return returnVal;
    }
    
    // Temporary measure for testing, but I might keep it for the final version.
    short* solution()
    {
        short* returnVal = new short[81];
        
        for (short i = 0; i < 81; i++)
        {
            returnVal[i] = squares[i%9][i/9]->getNumber();
        }
        
        return returnVal;
    }
};



// I forgot to mention in the previous commits. main(), as of now, is simply for testing purposes
// It will only be properly finished once I complete the classes entirely
int main()
{
    // Difficult:
    //short array[] = {2,0,0,3,0,0,0,0,0,8,0,4,0,6,2,0,0,3,0,1,3,8,0,0,2,0,0,0,0,0,0,2,0,3,9,0,5,0,7,0,0,0,6,2,1,0,3,2,0,0,6,0,0,0,0,2,0,0,0,9,1,4,0,6,0,1,2,5,0,8,0,9,0,0,0,0,0,1,0,0,2};
    
    // Easy:
    short array[] = {0,0,0,2,6,0,7,0,1,6,8,0,0,7,0,0,9,0,1,9,0,0,0,4,5,0,0,8,2,0,1,0,0,0,4,0,0,0,4,6,0,2,9,0,0,0,5,0,0,0,3,0,2,8,0,0,9,3,0,0,0,7,4,0,4,0,0,5,0,0,3,6,7,0,3,0,1,8,0,0,0};
    
    // Intermediate:
    //short array[] = {0,2,0,6,0,8,0,0,0,5,8,0,0,0,9,7,0,0,0,0,0,0,4,0,0,0,0,3,7,0,0,0,0,5,0,0,6,0,0,0,0,0,0,0,4,0,0,8,0,0,0,0,1,3,0,0,0,0,2,0,0,0,0,0,0,9,8,0,0,0,3,6,0,0,0,3,0,6,0,9,0};
    
    sudoku s(array);
    
    while(s.solve());
    
    short* answer = s.solution();
    
    for (short i = 0; i < 81; i++)
    {
        if (!(i%9))
        {
            cout << endl;
        }
        cout << answer[i];
    }
    cout << endl;
    
    cout<<"Hello World\n";
    cout<<((0b101)^(0b110))<<endl;
    delete(answer);
    
    return 0;
}