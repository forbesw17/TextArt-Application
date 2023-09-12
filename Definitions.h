#pragma once

const int MAXROWS = 22;
const int MAXCOLS = 80;
const int BUFFERSIZE = 20;
const int FILENAMESIZE = 255;

// ASCII codes for special keys; for editing
const char ESC = 27;
const char LEFTARROW = 75;
const char UPARROW = 72;
const char RIGHTARROW = 77;
const char DOWNARROW = 80;
const char SPECIAL = (char)224;


// Type definition to define the data type for the data items in a linked list
typedef char ListItemType[MAXROWS][MAXCOLS];

// The node structure for a linked list
struct Node
{
	ListItemType item;
	Node* next;
};

// A list structure containing a linked list and an integer, representing
// the number of items currently stored in the linked list
struct List
{
	Node* head = NULL;
	int count = 0;
};

struct DrawPoint;

/*
* A point used to represent a row/column on the screen or in the canvas array
*/
struct Point
{
	int row, col;

	Point() { row = 0; col = 0; }
	Point(int r, int c) { row = r; col = c; }
	Point(DrawPoint p);
};

/*
* A point used for the purpose of mathematical calculations (for drawing)
*/
struct DrawPoint
{
	double row, col;

	DrawPoint() { row = 0; col = 0; }
	DrawPoint(int r, int c) { row = r; col = c; }
	DrawPoint(Point p) { row = p.row;  col = p.col; }
};

//--------------------Functions To Modify---------------------------------------------------------------

/*
* Opens the specified filename for reading; assumed to be a TXT file.
* Filename is assumed to be in the form: "SavedFiles\example.txt"
* If the file can be opened for reading, this function loads the
* file's contents into current canvas, and then returns TRUE.
* If the file cannot be opened for reading, returns FALSE.
* If the file cannot be opened, canvas is left unchanged.
*/
// Get rid of this:
bool loadCanvas(char canvas[][MAXCOLS], char filename[]);
// Use this instead:
// bool loadCanvas(char canvas[][MAXCOLS], char filename[]);

/*
* Opens the specified filename for writing; assumed to be a TXT file.
* If the file can be opened for writing, this function writes the
* current canvas contents into the file, and then returns TRUE.
* If the file cannot be opened for writing, returns FALSE.
*/
// Get rid of this:
bool saveCanvas(char canvas[][MAXCOLS], char filename[]);
// Use this instead:
// bool saveCanvas(char canvas[][MAXCOLS], char filename[]);

/*
* Secondary menu used for choosing the new drawing functions.
* Menu repeats until the user enters 'M' to return to the main menu.
* current is a Node representing the main drawing canvas
* undoList is a List of nodes, holding all of the undo states
* redoList is a List of nodes, holding all of the redo states
* clips is a List of nodes, representing the current animation clip
* animate - true: animate / false: no animation
*   animate will be updated to reflect the menu option chosen by the user
*/
// Get rid of this:
void menuTwo(Node*& current, List& undoList, List& redoList, List& clips, bool& animate);
// Use this instead:
// void menuTwo(Node*& current, List& undoList, List& redoList, List& clips, bool& animate);


//--------------------New Functions---------------------------------------------------------------------

/*
* Creates and returns a new node, which contains a single blank (initialized) canvas
*/
Node* newCanvas();

/*
* Creates and returns a new node, which contains a single canvas, where the canvas
* contains a copy of the one which is inside oldNode
*/
Node* newCanvas(Node* oldNode);

/*
* Adds a node to the front of a linked list
* listToUpdate is a structure containing the linked list to which the node is to be added
* nodeToAdd is the node which should be inserted at the front of the linked list
*/
void addNode(List& listToUpdate, Node* nodeToAdd);

/*
* Removes a node from the front of a linked list
* listToUpdate is a structure containing the linked list from which the node is to be removed
* Returns the node which was removed from the list
* Returns NULL if the list is empty
*/
Node* removeNode(List& listToTUpdate);

/*
* Deletes all of the nodes in a linked list
* listToUpdate is a structure containing the linked list to be deleted
*/
void deleteList(List& listToDelete);

/*
* Adds a new undo state to the front of undoList, containing a
* a copy of the current canvas
* undoList the list to which the new undo state is to be added
* redoList is the list containing the redo states
* current is a node reprsenting the current drawing canvas
*/
void addUndoState(List& undoList, List& redoList, Node*& current);

/*
* Undo or Redo operation
* Adds current node to the front of the redoList, then removes a node
* from the front of the undoList and sets this as the current node
*/
void restore(List& undoList, List& redoList, Node*& current);

/*
* Plays the current animation in the drawing window repeatedly until ESC is held
* The current canvas is not changed
* clips is a structure containing the linked list holding the animation clips
* in reverse order (the item at the end of the linked list is assumed to be
* the first clip in the animation)
* The animation can only be played if there are at least 2 clips in the animation
* This function uses playRecursive to actually play the clips
*/
void play(List& clips);

/*
* Recursive function which plays all of the clips found in the linked list
* pointed to by head.  Count represents the number of clips found in the
* list currently pointed to by head, and is used to display the clip number
* at the bottom of the screen.
*/
void playRecursive(Node* head, int count);

/*
* Erases all clips found in the clips list, and then loads a new
* set of clips into the list, from several saved files.
* Filename is assumed to be in the form: "SavedFiles\example"
* The function will form filenames like:
*   SavedFiles\example-1.txt, SavedFiles\example-2.txt, SavedFiles\example-3.txt, etc.
* Each file will be opened and its contents loaded into a new
* node in the clips list. The item at the front of the list will
* be the last file (the one with the highest number).
*
* If the first file can be opened for reading, this function assumes the
* rest can be also, and loads them into the clips list, then returns TRUE.
* If the first file cannot be opened for reading, returns FALSE.
* The current canvas is not affected by this function.
*/
bool loadClips(List& clips, char filename[]);

/*
* Writes all of the clips from the clips list into multiple files.
* Filename is assumed to be in the form: "SavedFiles\example"
* The function will store each clip from the list into a separate file such as:
*   SavedFiles\example-1.txt, SavedFiles\example-2.txt, SavedFiles\example-3.txt, etc.
* The clip at the end of the list will be stored in the first file.
*
* If the files have been written successfully, this function returns TRUE.
* If the any file fails to be written, this function returns FALSE
*/
bool saveClips(List& clips, char filename[]);


//--------------------Old Functions---------------------------------------------------------------------


/*
* Allows user to choose a location on the screen moving the cursor around with arrow keys.
* Continues until the ESC key is pressed, or an ASCII printable character is entered.
* Returns: the character entered by the user
*   Point pt will be updated to reflect the point chosen by the user
*/
char getPoint(Point& pt);

/*
* Recursive function to fill a section of the canvas. Replaces all of adjacent oldCh characters in
* the canvas section with newCh.
* row and col is the row and column of the starting location where filling should begin
* oldCh is the character in the section to be replaced
* newCh is the character to replace with
* animate - true: animate / false: no animation
*/
void fillRecursive(char canvas[][MAXCOLS], int row, int col, char oldCh, char newCh, bool animate);

/*
* Stores character ch into canvas at location p
* p is the Point(row, col) where the character is to be stored
* ch is the character to store
* if animate is true animation is also performed by drawing the character
* to the screen at the same time it is being stored in the canvas
*/
void drawHelper(char canvas[][MAXCOLS], Point p, char ch, bool animate);

/*
* Draws a line between two points into the canvas.
* start is the point representing one end of the line
* end is the point reprsenting the other end of the line
* animate - true: animate the drawing / false: no animation
*/
void drawLine(char canvas[][MAXCOLS], DrawPoint start, DrawPoint end, bool animate);

/*
* Draws a box into the canvas, around a central point.
* center is the point representing the center of the box
* height is the height of the box (width is automatically proportionally chosen based on canvas size)
* animate - true: animate the drawing / false: no animation
*/
void drawBox(char canvas[][MAXCOLS], Point center, int height, bool  animate);

/*
* Recursive function to draw a series of nested boxes into the canvas, around a central point.
* center is the point representing the center of the smallest box
* height is the height of the largest box (width is automatically proportionally chosen based on canvas size)
* animate - true: animate the drawing / false: no animation
*/
void drawBoxesRecursive(char canvas[][MAXCOLS], Point center, int height, bool animate);

/*
* Recursive function to draw a fractal tree into the canvas.
* start is the starting point for the tree (the base of the trunk)
* height is the approximate height of the entire tree
* startAngle represents the direction in which to draw the trunk where
*     0 = east, 90 = south, 180 = west, 270 = north
* branchAngle represents the angle of the branch to the tree trunk
*     45 means each branch will be at a 45 degree angle from the tree trunk
* animate - true: animate the drawing / false: no animation
*/
void treeRecursive(char canvas[][MAXCOLS], DrawPoint start, int height, int startAngle, int branchAngle, bool animate);



/*
* Finds the end point of a line, given the line's starting point, length, and angle
* angle: 0 = east, 90 = south, 180 = west, 270 = north
* Returns: the end point for the line
*/
DrawPoint findEndPoint(DrawPoint start, int len, int angle);

/*
* Converts a from degrees to radians
* Returns: angle a in radians
*/
double inline degree2radian(int a) { return (a * 0.017453292519); }

/*
* Used by the drawLine function to help fill in row gaps in a line
* col is the column; startRow is the starting row; endRow is the ending row
* ch is the character to draw; animate - true: animate the drawing / false: no animation
*/
void drawLineFillRow(char canvas[][MAXCOLS], int col, int startRow, int endRow, char ch, bool animate);

/*
* Initializes canvas to contain all spaces.
*/
void initCanvas(char canvas[][MAXCOLS]);

/*
* Displays canvas contents on the screen, with a border
* around the right and bottom edges.
*/
void displayCanvas(char canvas[][MAXCOLS]);

/*
* Allows user to edit the canvas by moving the cursor around and
* entering characters. Editing continues until the ESC key is pressed.
*/
void editCanvas(char canvas[][MAXCOLS]);

/*
* Copies contents of the "from" canvas into the "to" canvas.
*/
void copyCanvas(char to[][MAXCOLS], char from[][MAXCOLS]);

/*
* Replaces all instances of a character in the canvas.
* oldCh is the character to be replaced.
* newCh character is the character to replace with.
*/
void replace(char canvas[][MAXCOLS], char oldCh, char newCh);

/*
* Shifts contents of the canvas by a specified number of rows and columns.
* rowValue is the number of rows by which to shift
*    positive numbers shift downward; negative numbers shift upward
* colValue is the number of rows by which to shift
*    positive numbers shift right; negative numbers shift left
*/
void moveCanvas(char canvas[][MAXCOLS], int rowValue, int colValue);

/*
* Clears a line on the output screen, then resets the cursor back to the
* beginning of this line.
* lineNum is the line number on the output screen to clear
* numOfChars is the number of characters to clear on this line
*/
void clearLine(int lineNum, int numOfChars);

/*
* Moves the cursor in the output window to a specified row and column.
* The next output produced by the program will begin at this position.
*/
void gotoxy(short row, short col);


//used to initalizes any 1D temp array
void initTempArray(char temp[]);