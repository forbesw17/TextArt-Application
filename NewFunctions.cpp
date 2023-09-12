#include <iostream>
#include <windows.h>
#include <conio.h>
#include "Definitions.h"
using namespace std;


Point::Point(DrawPoint p)
{
	row = (int)round(p.row);
	col = (int)round(p.col);
}

// https://math.stackexchange.com/questions/39390/determining-end-coordinates-of-line-with-the-specified-length-and-angle
DrawPoint findEndPoint(DrawPoint start, int len, int angle)
{
	DrawPoint end;
	end.col = start.col + len * cos(degree2radian(angle));
	end.row = start.row + len * sin(degree2radian(angle));
	return end;
}


// Use this to draw characters into the canvas, with the option of performing animation
void drawHelper(char canvas[][MAXCOLS], Point p, char ch, bool animate)
{
	// Pause time between steps (in milliseconds)
	const int TIME = 50;

	// Make sure point is within bounds
	if (p.row >= 0 && p.row < MAXROWS && p.col >= 0 && p.col < MAXCOLS)
	{
		// Draw character into the canvas
		canvas[p.row][p.col] = ch;

		// If animation is enabled, draw to screen at same time
		if (animate)
		{
			gotoxy(p.row, p.col);
			printf("%c", ch);
			Sleep(TIME);
		}
	}
}


// Fills gaps in a row caused by mismatch between match calculations and screen coordinates
// (i.e. the resolution of our 'canvas' isn't very good)
void drawLineFillRow(char canvas[][MAXCOLS], int col, int startRow, int endRow, char ch, bool animate)
{
	// determine if we're counting up or down
	if (startRow <= endRow)
		for (int r = startRow; r <= endRow; r++)
		{
			Point point(r, col);
			drawHelper(canvas, point, ch, animate);
		}
	else
		for (int r = startRow; r >= endRow; r--)
		{
			Point point(r, col);
			drawHelper(canvas, point, ch, animate);
		}
}


// Draw a single line from start point to end point
void drawLine(char canvas[][MAXCOLS], DrawPoint start, DrawPoint end, bool animate)
{
	char ch;

	Point scrStart(start);
	Point scrEnd(end);

	// vertical line
	if (scrStart.col == scrEnd.col)
	{
		ch = '|';

		drawLineFillRow(canvas, scrStart.col, scrStart.row, scrEnd.row, ch, animate);
	}
	// non-vertical line
	else
	{
		int row = -1, prevRow;

		// determine the slope of the line
		double slope = (start.row - end.row) / (start.col - end.col);

		// choose appropriate characters based on 'steepness' and direction of slope
		if (slope > 1.8)  ch = '|';
		else if (slope > 0.08)  ch = '`';
		else if (slope > -0.08)  ch = '-';
		else if (slope > -1.8) ch = '\'';
		else ch = '|';

		// determine if columns are counting up or down
		if (scrStart.col <= scrEnd.col)
		{
			// for each column from start to end, calculate row values
			for (int col = scrStart.col; col <= scrEnd.col; col++)
			{
				prevRow = row;
				row = (int)round(slope * (col - start.col) + start.row);

				// draw from previous row to current row (to fill in row gaps)
				if (prevRow > -1)
				{
					drawLineFillRow(canvas, col, prevRow, row, ch, animate);
				}
			}
		}
		else
		{
			// for each column from start to end, calculate row values
			for (int col = scrStart.col; col >= scrEnd.col; col--)
			{
				prevRow = row;
				row = (int)round(slope * (col - start.col) + start.row);

				// draw from previous row to current row (to fill in row gaps)
				if (prevRow > -1)
				{
					drawLineFillRow(canvas, col, prevRow, row, ch, animate);
				}
			}
		}
	}
}


// Draws a single box around a center point
void drawBox(char canvas[][MAXCOLS], Point center, int height, bool animate)
{
	int sizeHalf = height / 2;
	int ratio = (int)round(MAXCOLS / (double)MAXROWS * sizeHalf);

	// Calculate where the four corners of the box should be
	DrawPoint points[4];
	points[0] = DrawPoint(center.row - sizeHalf, center.col - ratio);
	points[1] = DrawPoint(center.row - sizeHalf, center.col + ratio);
	points[2] = DrawPoint(center.row + sizeHalf, center.col + ratio);
	points[3] = DrawPoint(center.row + sizeHalf, center.col - ratio);

	// Draw the four lines of the box
	for (int x = 0; x < 3; x++)
	{
		drawLine(canvas, points[x], points[x + 1], animate);
	}
	drawLine(canvas, points[3], points[0], animate);

	// Replace the corners with a better looking character
	for (int x = 0; x < 4; x++)
	{
		drawHelper(canvas, points[x], '+', animate);
	}
}


// Menu for the drawing tools
void menuTwo(Node*& current, List& undoList, List& redoList, List& clips, bool& animate)
{
	//arrays for draw menu
	const int MENUSIZE = 66;
	char drawMenu[MENUSIZE] = "<F>ill / <L>ine / <B>ox / <N>ested Boxes / <T>ree / <M>ain Menu: ";
	int menuBoolPos = 11;

	//variable to hold user choice
	char choice;

	do
	{
		//display the canvas
		displayCanvas(current->item);

		//clear line to prep for menu line 1
		clearLine(MAXROWS + 1, MAXCOLS + 5);

		//start printing menu line 1 here
		cout << "<A>nimate: ";

		//check for animation value to determine which character to print
		if (animate == true)
		{
			cout << 'Y';
		}
		else
		{
			cout << 'N';
		}

		//continuation of menu line 1
		cout << " / <U>ndo: " << undoList.count;

		//continuation of menu line 1 we can show the redo option if we have any redo available
		if (redoList.count > 0)
		{
			cout << " / Red<O>: " << redoList.count;
		}

		//continuation of menu line 1
		cout << " / Cl<I>p: " << clips.count;

		//continuation of menu line 1, we can show the play option if there are 2 or more clips
		if (clips.count >= 2)
		{
			cout << " / <P>lay: ";
		}

		//clear line 3 to make sure some of our other menu prints do not linger
		clearLine(MAXROWS + 3, MAXCOLS + 5);

		//clear line to prep for menu line 2
		clearLine(MAXROWS + 2, MAXCOLS + 5);

		//print menu line 2
		for (int i = 0; i < MENUSIZE; i++)
		{
			cout << drawMenu[i];
		}

		//get user choice
		cin >> choice;
		//this will ignore the enter key press so it does not go into the key buffer

		cin.ignore((numeric_limits<streamsize>::max)(), '\n');

		//change the char choice to uppercase
		choice = toupper(choice);

		switch (choice)
		{

			//animation option
		case 'A':
		{
			//set animate equal to the opposite of current state
			animate = !animate;
			break;
		}

		//fill option
		case 'F':
		{
			//variables to hold new and old characters
			char newCh, oldCh;

			//point variable to use getPoint
			Point pt;

			//clear line and ask user to enter character they want to replace old character
			clearLine(MAXROWS + 2, MAXCOLS + 5);
			clearLine(MAXROWS + 1, MAXCOLS + 5);
			cout << "Enter character to fill with from current location / <ESC> to cancel";

			//newCh is the key entered
			newCh = getPoint(pt);
			//oldCh is the char at the row and col key was pressed
			oldCh = current->item[pt.row][pt.col];

			//if they try to fill with the same character
			if (newCh == oldCh)
			{

			}
			else if (newCh != ESC)
			{
				//add new node to undo list that is a copy of current and delete redo list
				addUndoState(undoList, redoList, current);

				fillRecursive(current->item, pt.row, pt.col, oldCh, newCh, animate);
			}
			break;
		}

		//line option
		case 'L':
		{
			//point variable to use getPoint
			char charStart;
			Point start, end;

			//clear line and ask user to enter character they want to replace old character
			clearLine(MAXROWS + 2, MAXCOLS + 5);
			clearLine(MAXROWS + 1, MAXCOLS + 5);
			cout << "Type any letter to choose start point / <ESC> to cancel";

			//get the start point and store the character returned into variable
			charStart = getPoint(start);

			//use variable to check for esc
			if (charStart != ESC)
			{
				//print the character if they did not press escape
				cout << charStart;
			}
			else
			{
				break;
			}

			//clear line and ask user to enter character they want to replace old character
			clearLine(MAXROWS + 2, MAXCOLS + 5);
			clearLine(MAXROWS + 1, MAXCOLS + 5);
			cout << "Type any letter to choose end point / <ESC> to cancel";


			//if the user did not enter ESC then we use function
			if (getPoint(end) != ESC)
			{
				//add new node to undo list that is a copy of current and delete redo list
				addUndoState(undoList, redoList, current);

				//use the parameters with the functions
				drawLine(current->item, start, end, animate);

			}
			break;
		}
		//box option

		case 'B':
		{
			//point variable to hold center
			Point center;

			//height of the box
			int height;

			//used to check char typed by user
			char realCenter;

			//ask uer to enter the size they want the box
			clearLine(MAXROWS + 2, MAXCOLS + 5);
			clearLine(MAXROWS + 1, MAXCOLS + 5);
			cout << "Enter size: ";
			cin >> height;

			//give user instructions on how to choose box center
			clearLine(MAXROWS + 2, MAXCOLS + 5);
			clearLine(MAXROWS + 1, MAXCOLS + 5);
			cout << "Type any letter to choose box center, or <C> for screen center / <ESC> to cancel";

			//use realCenter to store the value of character
			realCenter = getPoint(center);
			realCenter = toupper(realCenter);

			//check if they entered C, if so set the center to our chosen default
			if (realCenter == 'C')
			{
				//add new node to undo list that is a copy of current and delete redo list
				addUndoState(undoList, redoList, current);

				center.col = MAXCOLS / 2;
				center.row = MAXROWS / 2;
				drawBox(current->item, center, height, animate);
			}
			//if the user did not enter ESC then we use function
			else if (realCenter != ESC)
			{
				//add new node to undo list that is a copy of current and delete redo list
				addUndoState(undoList, redoList, current);

				drawBox(current->item, center, height, animate);
			}
			break;
		}

		//nested box option
		case 'N':
		{
			//point variable to hold center
			Point center;

			//height of the box
			int height;
			//used to check char typed by user
			char realCenter;

			//ask uer to enter the size they want the box
			clearLine(MAXROWS + 2, MAXCOLS + 5);
			clearLine(MAXROWS + 1, MAXCOLS + 5);
			cout << "Enter size of largest box: ";
			cin >> height;

			//give user instructions on how to choose box center
			clearLine(MAXROWS + 2, MAXCOLS + 5);
			clearLine(MAXROWS + 1, MAXCOLS + 5);
			cout << "Type any letter to choose box center, or <C> for screen center / <ESC> to cancel";

			//use realCenter to store the value of character
			realCenter = getPoint(center);
			realCenter = toupper(realCenter);

			//check if they entered C, if so set the center to our chosen default
			if (realCenter == 'C')
			{
				//add new node to undo list that is a copy of current and delete redo list
				addUndoState(undoList, redoList, current);

				center.col = MAXCOLS / 2;
				center.row = MAXROWS / 2;
				drawBoxesRecursive(current->item, center, height, animate);
			}
			//if the user did not enter ESC then we use function
			else if (realCenter != ESC)
			{
				//add new node to undo list that is a copy of current and delete redo list
				addUndoState(undoList, redoList, current);

				drawBoxesRecursive(current->item, center, height, animate);
			}
			break;
		}

		//tree option
		case 'T':
		{
			//starting angle
			const int startAngle = 270;

			//point variable to hold center
			Point center;
			//height of the box
			int height;
			//user branch angle choice
			int branchAngle;
			//used to check char typed by user
			char realCenter;

			//ask user to enter the size they want the box
			clearLine(MAXROWS + 2, MAXCOLS + 5);
			clearLine(MAXROWS + 1, MAXCOLS + 5);
			cout << "Enter approximate tree height: ";
			cin >> height;

			//ask user to enter the branch angle they want the box
			clearLine(MAXROWS + 2, MAXCOLS + 5);
			clearLine(MAXROWS + 1, MAXCOLS + 5);
			cout << "Enter branch angle: ";
			cin >> branchAngle;

			//give user instructions on how to choose box center
			clearLine(MAXROWS + 1, MAXCOLS + 5);
			cout << "Type any letter to choose box center, or <C> for screen center / <ESC> to cancel";

			//use realCenter to store the value of character
			realCenter = getPoint(center);
			realCenter = toupper(realCenter);

			//check if they entered C, if so set the center to our chosen default
			if (realCenter == 'C')
			{
				//add new node to undo list that is a copy of current and delete redo list
				addUndoState(undoList, redoList, current);

				center.col = MAXCOLS / 2;
				center.row = MAXROWS - 1;
				treeRecursive(current->item, center, height, startAngle, branchAngle, animate);
			}
			//if the user did not enter ESC then we use function
			else if (realCenter != ESC)
			{
				//add new node to undo list that is a copy of current and delete redo list
				addUndoState(undoList, redoList, current);

				treeRecursive(current->item, center, height, startAngle, branchAngle, animate);
			}
			break;
		}
		case 'U':
			//check to see if the there is anything that can be undone, if so, do it
			if (undoList.count > 0)
			{
				restore(undoList, redoList, current);
			}
			break;

		case 'O':
			//check to see if the there is anything that can be undone, if so, do it
			if (redoList.count > 0)
			{
				restore(redoList, undoList, current);
			}
			break;

		case 'P':
			//check if there are enough clips
			if (clips.count >= 2)
			{
				clearLine(MAXROWS + 2, MAXCOLS + 5);
				play(clips);
				break;
			}

		case 'I':
		{
			//create a new node that is a copy of the canvas
			Node* newNode = newCanvas(current);

			//check to see if the list is empty
			if (clips.head == NULL)
			{
				//if empty we can set the head to be the new node
				clips.head = newNode;
			}
			else
			{
				//traverse to the last node
				Node* temp = clips.head;

				while (temp->next != NULL)
				{
					temp = temp->next;
				}

				//make the last node point to the new node
				temp->next = newNode;
			}

			//increment clip list count
			clips.count++;

			break;
		}

		}
	}
	//continues while user has not pressed m
	while (choice != 'M');
}


// Get a single point from screen, with character entered at that point
char getPoint(Point& pt)
{
	char input;
	int row = 0, col = 0;

	// Move cursor to row,col and then get
	// a single character from the keyboard
	gotoxy(row, col);

	do
	{
		//get user input 
		input = _getch();

		//check if it is a printable ascii
		if (input >= 32 && input <= 126)
		{
			//change our point row and col fields to the row and col selected
			pt.row = row;
			pt.col = col;

			//return the character pressed at the current location
			return input;
		}
		//check for special characters
		if (input == SPECIAL)
		{
			//receive second part of special character 
			input = _getch();
			switch (input)
			{
			case RIGHTARROW:
			{
				col += 1;
				if (col >= MAXCOLS)
				{
					col--;
				}
				gotoxy(row, col);
				break;
			}

			case LEFTARROW:
			{
				col -= 1;
				if (col < 0)
				{
					col++;
				}
				gotoxy(row, col);
				break;
			}

			case UPARROW:
			{
				row -= 1;
				if (row < 0)
				{
					row++;
				}
				gotoxy(row, col);
				break;
			}

			case DOWNARROW:
			{
				row += 1;
				if (row >= MAXROWS)
				{
					row--;
				}
				gotoxy(row, col);
				break;
			}
			}
		}
		//check for the F keys 
		else if (input == '\0')
		{
			//we get the char but do not store it  
			_getch();
		}
	}
	//continue while the escape button has not been pressed    
	while (input != ESC);

	//if the user presses escape, we have to return it for checking in menu
	return ESC;
}




// Recursively fill a section of the screen
void fillRecursive(char canvas[][MAXCOLS], int row, int col, char oldCh, char newCh, bool animate)
{
	//checks to make sure given position is in bounds of canvas and equal to oldCh
	if (canvas[row][col] == oldCh && row <= MAXROWS && row >= 0 && col <= MAXCOLS && col >= 0)
	{
		//checks for animation boolean and utilizes drawHelper if true
		Point point(row, col);
		drawHelper(canvas, point, newCh, animate);

		//function calls determine directions to fill
		fillRecursive(canvas, row + 1, col, oldCh, newCh, animate);
		fillRecursive(canvas, row - 1, col, oldCh, newCh, animate);
		fillRecursive(canvas, row, col + 1, oldCh, newCh, animate);
		fillRecursive(canvas, row, col - 1, oldCh, newCh, animate);
	}
}


// Recursively draw a tree
void treeRecursive(char canvas[][MAXCOLS], DrawPoint start, int height, int startAngle, int branchAngle, bool animate)
{
	//continues only if height >= 3 and checks to make sure the start is in bounds 
	if (height >= 3 && start.col <= MAXCOLS && start.col >= 0 && start.row <= MAXROWS && start.row >= 0)
	{
		//find the endpoint
		DrawPoint end = findEndPoint(start, height / 3, startAngle);

		//draws trunk and branches
		drawLine(canvas, start, end, animate);

		//rightbranch
		treeRecursive(canvas, end, height - 2, startAngle + branchAngle, branchAngle, animate);

		//leftBranch
		treeRecursive(canvas, end, height - 2, startAngle - branchAngle, branchAngle, animate);
	}
}


// Recursively draw nested boxes
void drawBoxesRecursive(char canvas[][MAXCOLS], Point center, int height, bool animate)
{
	//Draws box if height >=2, then calls itself with height - 2
	if (height >= 2)
	{
		drawBox(canvas, center, height, animate);
		drawBoxesRecursive(canvas, center, height - 2, animate);
	}
}