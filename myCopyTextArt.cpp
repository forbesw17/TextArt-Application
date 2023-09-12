#include <iostream>
#include <fstream>
#include <cctype>
#include <windows.h>
#include <conio.h>
#include "Definitions.h"
using namespace std;



int main()
{
	//create a new node pointer to hold the current canvas 
	Node* current;

	//this will be our list for the undo option
	List undoList;

	//this will be our list for the redo option
	List redoList;

	//this will be our list for the clips
	List clipList;

	//initializse current to be a node
	current = newCanvas();

	int menuBoolPos = 11;

	//arrays to hold menu lines
	const int MENUSIZE = 76;
	char menuLine2[MENUSIZE] = "<E>dit / <M>ove / <R>eplace / <D>raw / <C>lear / <L>oad / <S>ave / <Q>uit: ";

	//choice variable for user input
	char choice;

	//animation support variable
	bool animate = false;

	//fill array with spaces and clear the screen
	system("cls");

	do
	{
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
		cout << " / Cl<I>p: " << clipList.count;

		//continuation of menu line 1, we can show the play option if there are 2 or more clips
		if (clipList.count >= 2)
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
			cout << menuLine2[i];
		}

		//ask for user choice
		cin >> choice;

		//this will ignore the enter key press so it does not go into the key buffer
		cin.ignore((numeric_limits<streamsize>::max)(), '\n');

		//change the char choice to uppercase
		choice = toupper(choice);

		switch (choice)
		{
		case 'A':
			//set animate equal to the opposite of current state
			animate = !animate;
			break;


		case 'E':
			//add new node to undo list that is a copy of current and delete redo list
			addUndoState(undoList, redoList, current);

			//clear current line and ask user for input
			clearLine(MAXROWS + 2, MAXCOLS + 5);
			clearLine(MAXROWS + 1, MAXCOLS + 5);
			cout << "Press <ESC> to stop editing";

			editCanvas(current->item);
			break;

		case 'M':
			//add new node to undo list that is a copy of current and delete redo list
			addUndoState(undoList, redoList, current);

			//create variables to get units for moving
			int rowValue, colValue;

			//clear current line and ask user for input
			clearLine(MAXROWS + 2, MAXCOLS + 5);
			clearLine(MAXROWS + 1, MAXCOLS + 5);
			cout << "Enter column units to move: ";
			cin >> colValue;
			cout << "Enter row units to move: ";
			cin >> rowValue;

			//pass the canvas and user inputs to move function
			moveCanvas(current->item, rowValue, colValue);
			break;

		case 'R':
			//add new node to undo list that is a copy of current and delete redo list
			addUndoState(undoList, redoList, current);

			//create variables to get characters for replacing
			char oldCh, newCh;

			//clear current line and ask user for input
			clearLine(MAXROWS + 2, MAXCOLS + 5);
			clearLine(MAXROWS + 1, MAXCOLS + 5);
			cout << "Enter character to replace: ";
			//gets what the user wants to replace, ignores enter key
			cin.get(oldCh);
			cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			cout << "Enter character to replace with: ";
			//gets the replacement character, ignores enter key
			cin.get(newCh);
			cin.ignore((numeric_limits<streamsize>::max)(), '\n');

			//pass the canvas and user inputs to replace function
			replace(current->item, oldCh, newCh);
			break;

		case 'D':
			//call the drawMenu function
			menuTwo(current, undoList, redoList, clipList, animate);
			break;

		case 'C':
			//add new node to undo list that is a copy of current and delete redo list
			addUndoState(undoList, redoList, current);

			//init canvas to clear canvas
			initCanvas(current->item);
			break;

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
			if (clipList.count >= 2)
			{
				clearLine(MAXROWS + 2, MAXCOLS + 5);
				play(clipList);
				break;
			}

		case 'I':
		{
			//create a new node that is a copy of the canvas
			Node* newNode = newCanvas(current);

			//check to see if the list is empty
			if (clipList.head == NULL)
			{
				//if empty we can set the head to be the new node
				clipList.head = newNode;
			}
			else
			{
				//traverse to the last node
				Node* temp = clipList.head;

				while (temp->next != NULL)
				{
					temp = temp->next;
				}

				//make the last node point to the new node
				temp->next = newNode;
			}

			//increment clip list count
			clipList.count++;

			break;
		}

		case 'L':
		{
			//ask user if they want to load a canvas or animation
			char canvasOrAnim;
			clearLine(MAXROWS + 2, MAXCOLS + 5);
			clearLine(MAXROWS + 1, MAXCOLS + 5);
			cout << "<C>anvas or <A>nimation ? ";

			cin >> canvasOrAnim;
			cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			canvasOrAnim = toupper(canvasOrAnim);

			//if they do not press C or A it will break us to main menu
			if (canvasOrAnim != 'C' && canvasOrAnim != 'A')
			{
				break;
			}

			//if they chose to load a canvas we pass to loadCanvas 
			if (canvasOrAnim == 'C')
			{
				//filename variable to hold the full filename
				char filename[FILENAMESIZE] = "SavedFiles\\.txt";
				int usedSize = 15;

				//maximum amount of characters a user can enter for their filename
				const int USERINPUTSIZE = FILENAMESIZE - 15;

				//ask the user for filename and retrieve it
				char userFilename[USERINPUTSIZE];
				clearLine(MAXROWS + 2, MAXCOLS + 5);
				clearLine(MAXROWS + 1, MAXCOLS + 5);
				cout << "Enter the filename (don't enter 'txt'): ";
				cin.getline(userFilename, USERINPUTSIZE);

				int pos = 11;

				//we create an insertion loop, goes until the highest possible userinput size or until we reach '\0'
				for (int i = 0; i < USERINPUTSIZE && userFilename[i] != '\0'; i++)
				{
					//this loop is for moving everything after the insertion position to the right by 1
					for (int j = usedSize; j > pos; j--)
					{
						filename[j] = filename[j - 1];
					}
					//insert userInput into filename at the position
					filename[pos] = userFilename[i];

					//since we insterted we increased the usedSize
					usedSize++;

					//increment position so that next insert is after last one
					pos++;
				}

				//add new node to undo list that is a copy of current and delete redo list
				addUndoState(undoList, redoList, current);

				//check if loadCanvas return false 
				if (loadCanvas(current->item, filename) == false)
				{
					//if so we say at cannot be found
					removeNode(undoList);
					cerr << "ERROR: File cannot be read.\n";
					system("pause");
					break;
				}
			}
			//if they chose to load animation pass loadClips
			else if (canvasOrAnim == 'A')
			{
				//filename variable to hold the full filename
				char filename[FILENAMESIZE] = "SavedFiles\\";
				int usedSize = 11;

				//maximum amount of characters a user can enter for their filename
				const int USERINPUTSIZE = FILENAMESIZE - 15;

				//ask the user for filename and retrieve it
				char userFilename[USERINPUTSIZE];
				clearLine(MAXROWS + 2, MAXCOLS + 5);
				clearLine(MAXROWS + 1, MAXCOLS + 5);
				cout << "Enter the filename (don't enter 'txt'): ";
				cin.getline(userFilename, USERINPUTSIZE);

				int pos = 11;

				//we create an insertion loop, goes until the highest possible userinput size or until we reach '\0'
				for (int i = 0; i < USERINPUTSIZE && userFilename[i] != '\0'; i++)
				{
					//this loop is for moving everything after the insertion position to the right by 1
					for (int j = usedSize; j > pos; j--)
					{
						filename[j] = filename[j - 1];
					}
					//insert userInput into filename at the position
					filename[pos] = userFilename[i];

					//since we insterted we increased the usedSize
					usedSize++;

					//increment position so that next insert is after last one
					pos++;
				}

				//check if load clips returned false
				if (loadClips(clipList, filename) == false)
				{
					//if so we say at cannot be found
					removeNode(undoList);
					cerr << "ERROR: File cannot be read.\n";
					system("pause");
					break;
				}
				//if load clips returns true
				else
				{
					//check if it loaded one file 

					if (clipList.head->next == NULL)
					{
						//if so, we have to remove the node since it counted one file and say we could not read it
						removeNode(clipList);
						removeNode(undoList);
						cerr << "ERROR: File cannot be read.\n";
						system("pause");
						break;
					}

					//if it loads multiple files then we can say clips loaded
					else
					{
						//add new node to undo list that is a copy of current and delete redo list
						clearLine(MAXROWS + 2, MAXCOLS + 5);
						cout << "Clips loaded!\n";
						system("pause");
					}
				}
			}
			break;
		}

		case 'S':
		{
			//ask user if they want to load a canvas or animation
			char canvasOrAnim;
			clearLine(MAXROWS + 2, MAXCOLS + 5);
			clearLine(MAXROWS + 1, MAXCOLS + 5);
			cout << "<C>anvas or <A>nimation ? ";

			cin >> canvasOrAnim;
			cin.ignore((numeric_limits<streamsize>::max)(), '\n');
			canvasOrAnim = toupper(canvasOrAnim);

			//if they do not press C or A it will break us to main menu
			if (canvasOrAnim != 'C' && canvasOrAnim != 'A')
			{
				break;
			}

			//if they chose to load a canvas we pass to loadCanvas 
			if (canvasOrAnim == 'C')
			{
				//filename variable to hold the full filename
				char filename[FILENAMESIZE] = "SavedFiles\\.txt";
				int usedSize = 15;

				//maximum amount of characters a user can enter for their filename
				const int USERINPUTSIZE = FILENAMESIZE - 15;

				//ask the user for filename and retrieve it
				char userFilename[USERINPUTSIZE];
				clearLine(MAXROWS + 2, MAXCOLS + 5);
				clearLine(MAXROWS + 1, MAXCOLS + 5);
				cout << "Enter the filename (don't enter 'txt'): ";
				cin.getline(userFilename, USERINPUTSIZE);

				int pos = 11;

				//we create an insertion loop, goes until the highest possible userinput size or until we reach '\0'
				for (int i = 0; i < USERINPUTSIZE && userFilename[i] != '\0'; i++)
				{
					//this loop is for moving everything after the insertion position to the right by 1
					for (int j = usedSize; j > pos; j--)
					{
						filename[j] = filename[j - 1];
					}
					//insert userInput into filename at the position
					filename[pos] = userFilename[i];

					//since we insterted we increased the usedSize
					usedSize++;

					//increment position so that next insert is after last one
					pos++;
				}

				//check if saveCanvas returns true
				if (saveCanvas(current->item, filename) == true)
				{
					//say that we saved the file
					cout << "File saved!\n";
					system("pause");
				}
				//if it returns false
				else
				{
					//say we cannot write
					clearLine(MAXROWS + 2, MAXCOLS + 5);
					cerr << "ERROR: File cannot be written.\n";
					system("pause");
					break;
				}
			}
			//if they chose to save animation pass saveClips
			else if (canvasOrAnim == 'A')
			{
				//filename variable to hold the full filename
				char filename[FILENAMESIZE] = "SavedFiles\\";
				int usedSize = 11;

				//maximum amount of characters a user can enter for their filename
				const int USERINPUTSIZE = FILENAMESIZE - 15;

				//ask the user for filename and retrieve it
				char userFilename[USERINPUTSIZE];
				clearLine(MAXROWS + 2, MAXCOLS + 5);
				clearLine(MAXROWS + 1, MAXCOLS + 5);
				cout << "Enter the filename (don't enter 'txt'): ";
				cin.getline(userFilename, USERINPUTSIZE);

				int pos = 11;

				//we create an insertion loop, goes until the highest possible userinput size or until we reach '\0'
				for (int i = 0; i < USERINPUTSIZE && userFilename[i] != '\0'; i++)
				{
					//this loop is for moving everything after the insertion position to the right by 1
					for (int j = usedSize; j > pos; j--)
					{
						filename[j] = filename[j - 1];
					}
					//insert userInput into filename at the position
					filename[pos] = userFilename[i];

					//since we insterted we increased the usedSize
					usedSize++;

					//increment position so that next insert is after last one
					pos++;
				}

				//check if saveClips returns true
				if (saveClips(clipList, filename) == true)
				{
					//say we saved the clips
					clearLine(MAXROWS + 2, MAXCOLS + 5);
					cout << "Clips saved!\n";
					system("pause");
				}
				//if it returns false
				else
				{
					//say we cannot write
					clearLine(MAXROWS + 2, MAXCOLS + 5);
					cerr << "ERROR: File cannot be written.\n";
					system("pause");
					break;
				}
			}
		}
		}

	} while (choice != 'Q');

	//delete current node* and lists to avoid memory leak
	delete current;
	deleteList(undoList);
	deleteList(redoList);
	deleteList(clipList);

	return 0;
}

//sets the cursor position to a specified row and column
void gotoxy(short row, short col)
{
	COORD pos = { col, row };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

//clears a specified line by overwriting characters given the line number and number of characters to overwrite
void clearLine(int lineNum, int numOfChars)
{
	// Move cursor to the beginning of the specified line on the console
	gotoxy(lineNum, 0);

	// Write a specified number of spaces to overwrite characters
	for (int x = 0; x < numOfChars; x++)
		cout << " ";

	// Move cursor back to the beginning of the line
	gotoxy(lineNum, 0);
}

//looks through an array for any interation of oldCh, replaces it with the newCh
void replace(char canvas[][MAXCOLS], char oldCh, char newCh)
{
	//iterates through the 2d array treating it as a 1d array
	for (int i = 0; i < MAXCOLS * MAXROWS; i++)
	{
		//checks if the given position is equal to the to-be-replaced character
		if (canvas[0][i] == oldCh)
		{
			//replaces said character with the newCh
			canvas[0][i] = newCh;
		}
	}
}

//takes a given canvas array, uses the gotoxy function to move the cursor around the canvas
//ignores F-key input, checks for typable characters, and exits if ESC is pressed
void editCanvas(char canvas[][MAXCOLS])
{
	char input;
	int row = 0, col = 0;

	clearLine(MAXROWS + 1, MAXCOLS + 5);
	cout << "Press <ESC> to stop editing";

	// Move cursor to row,col and then get
	// a single character from the keyboard
	gotoxy(row, col);

	do
	{

		//get user input 
		input = _getch();

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
		//check for any typable
		else if (input >= 32 && input <= 126)
		{
			canvas[row][col] = input;
			cout << input;
			gotoxy(row, col);
		}

	}
	//continue while the escape button has not been pressed    
	while (input != ESC);
}

//shifts the entire canvas using a given amount of rows and columns
//doesn't store values that go past the boundaries of the canvas
void moveCanvas(char canvas[][MAXCOLS], int rowValue, int colValue) // Robert
{
	char temp[MAXROWS][MAXCOLS];

	//stores each postion of canvas into temp 
	copyCanvas(temp, canvas);

	//fill canvas with whitespace
	initCanvas(canvas);

	for (int i = 0; i < MAXROWS; i++)
	{
		for (int j = 0; j < MAXCOLS; j++)
		{
			//only stores value if it is in boundaries of canvas
			if (j + colValue < MAXCOLS && j + colValue >= 0 && i + rowValue < MAXROWS && i + rowValue >= 0)
			{
				canvas[i + rowValue][j + colValue] = temp[i][j];
			}
		}
	}
}

//fills the canvas array with blank spaces
void initCanvas(char canvas[][MAXCOLS])
{
	//loop through all positions in the array
	for (int i = 0; i < MAXCOLS * MAXROWS; i++)
	{
		//set position to hold a space
		canvas[0][i] = ' ';
	}
}

//clears the screen, then displays the updated canvas array to the screen
//prints the canvas borders
void displayCanvas(char canvas[][MAXCOLS])
{
	//move cursor to top left
	gotoxy(0, 0);

	//diplays canvas
	for (int x = 0; x < MAXROWS; x++)
	{
		for (int y = 0; y < MAXCOLS; y++)
		{
			cout << canvas[x][y];
		}
		//adds "|" at the end of each row then goes to \n
		cout << "|\n";
	}

	//prints horizonal lines for canvas
	for (int i = 0; i < MAXCOLS; i++)
	{
		cout << "-";
	}
}

//copies the contents of the canvas array to another array, used mainly in the undo function
void copyCanvas(char to[][MAXCOLS], char from[][MAXCOLS])
{
	//iterates through both arrays copying from[][] to to[][]
	for (int i = 0; i < MAXCOLS * MAXROWS; i++)
	{
		to[0][i] = from[0][i];
	}
}

//saves the canvas to a local text file
//allows user to enter the filename, pauses if the file cannot be connected to
bool saveCanvas(char canvas[][MAXCOLS], char filename[])
{
	//create ofstream variable and connect to the file
	ofstream outFile(filename);

	//test if connections to file worked, if not, print error message
	if (outFile.fail())
	{
		return false;
	}
	//if connection successful, canvas is copied into the file
	else
	{
		for (int x = 0; x < MAXROWS; x++)
		{
			for (int y = 0; y < MAXCOLS; y++)
			{
				outFile << canvas[x][y];
			}
			//add a new line after each row
			outFile << "\n";
		}

		//close the file
		outFile.close();

		//if file was loaded successfully we can return true
		return true;
	}
}

//initializes a temporary canvas array
void initTempArray(char temp[])
{
	//only needs to fill canvas size with white spaces
	for (int i = 0; i < MAXCOLS; i++)
	{
		temp[i] = ' ';
	}
}

//allows the user to load a file from local storage, copying it to the canvas array
//pauses if the file cannot be read
bool loadCanvas(char canvas[][MAXCOLS], char filename[])
{
	//1d array used to read each line of the file
	char temp[200];
	int rowCounter = 0;

	//Initializes temp array to hold all whitespaces
	initTempArray(temp);

	ifstream inFile(filename);

	if (inFile.fail())
	{
		//if file did not load correctly, we return false
		return false;
	}
	//if connection successful, file is copied into canvas
	else
	{
		//initalizes canvas before loading
		initCanvas(canvas);

		//add check to see if getline goes into error state
		inFile.getline(temp, (numeric_limits<streamsize>::max)());

		//checks to see if you've reached EOF or max number of rows
		while (!inFile.eof() && rowCounter < MAXROWS)
		{
			//stores chars from each position of temp into canvas
			for (int i = 0; i < MAXCOLS; i++)
			{
				if (temp[i] != '\0')
				{
					canvas[rowCounter][i] = temp[i];
				}
			}
			//clears temp array for next use
			initTempArray(temp);

			rowCounter++;

			inFile.getline(temp, (numeric_limits<streamsize>::max)());

		}

		//close the file
		inFile.close();

		//if file was loaded successfully we can return true
		return true;
	}
}