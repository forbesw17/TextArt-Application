#include <iostream>
#include <Windows.h>
#include "Definitions.h"
using namespace std;


Node* newCanvas()
{
	//create new node
	Node* head = new Node;

	//initilize the canvas inside the node to be empty
	initCanvas(head->item);

	//set the head next to null since this will be the last thing in the list
	head->next = NULL;

	return head;
}


Node* newCanvas(Node* oldNode)
{
	//create new node
	Node* head = new Node;

	//copy the old node canvas into the new node canvas
	copyCanvas(head->item, oldNode->item);

	//set the head next to null since this will be the last thing in the list
	head->next = NULL;

	return head;
}


void play(List& clips)
{
	// loops as long as the ESCAPE key is not currently being pressed
	while (!(GetKeyState(VK_ESCAPE) & 0x8000))
	{
		playRecursive(clips.head, clips.count);
	}
}


void playRecursive(Node* head, int count)
{
	//checks for null to know when to stop
	if (head == NULL || head->next == NULL)
	{
		return;
	}

	//recursively calls itself to display the next item in the list
	playRecursive(head->next, count - 1);

	//prints menu option
	clearLine(MAXROWS + 1, MAXCOLS + 5);
	printf("Hold <ESC> to stop \t Clip: %2d     ", count);

	displayCanvas(head->item);

	// Pause for 100 milliseconds to slow down animation
	Sleep(100);
}


void addUndoState(List& undoList, List& redoList, Node*& current)
{
	//we are adding a node to the undolist that is a copy of the current canvas
	addNode(undoList, newCanvas(current));

	//delete the redoList
	deleteList(redoList);
}


void restore(List& undoList, List& redoList, Node*& current)
{
	//add the current canvas to the redolist
	addNode(redoList, current);

	//set the current canvas equal to the node we remove from undoList
	current = removeNode(undoList);
}


void addNode(List& list, Node* nodeToAdd)
{
	//first we make our node we add point to the same thing the head points to
	nodeToAdd->next = list.head;

	//then we change the head to point to the node we are adding
	list.head = nodeToAdd;

	//we increment count of list to keep track of nodes
	list.count++;
}


Node* removeNode(List& list)
{
	//check if the head is null, if so we cannot remove any more nodes
	if (list.head == NULL)
	{
		return NULL;
	}

	//create a temp to store the old head
	Node* temp = list.head;

	//move the head pointer to the next node
	list.head = list.head->next;

	//decrement the count of list to keep track of nodes
	list.count--;

	//return the removed node
	return temp;
}


void deleteList(List& list)
{
	//continues until there are no nodes in the list
	while (list.head != NULL)
	{
		Node* temp = list.head->next;
		delete list.head;
		list.head = temp;
	}
}


bool loadClips(List& clips, char filename[])
{
	Node* nodeToAdd = newCanvas();;
	int filenameLen = strlen(filename);
	int fileNumber = 1;

	//deletes pre-existing list
	deleteList(clips);

	//sets clips count to zero
	clips.count = 0;

	//creates new node for storing canvas
	nodeToAdd = newCanvas();

	//concatenates file name to have -#.txt 
	snprintf(filename + filenameLen, FILENAMESIZE - (filenameLen + 2), "-%d.txt", fileNumber);

	if (loadCanvas(nodeToAdd->item, filename) == false)
	{
		return false;
	}

	//loop to go though files 2-end
	while (loadCanvas(nodeToAdd->item, filename) != false)
	{
		addNode(clips, nodeToAdd);

		nodeToAdd = newCanvas();

		snprintf(filename + filenameLen, FILENAMESIZE - (filenameLen + 2), "-%d.txt", fileNumber);

		//counter used to keep track of filenumber
		fileNumber++;
	}

	//removes node created for last failed loadCanvas()
	delete nodeToAdd;
	clips.count--;

	return true;
}


bool saveClips(List& clips, char filename[])
{
	int filenameLength = strlen(filename);
	int fileNumber = 1;

	for (Node* current = clips.head; current != NULL; current = current->next)
	{
		snprintf(filename + filenameLength, FILENAMESIZE - (filenameLength + 2), "-%d.txt", fileNumber);

		// try to save the current canvas to the file
		if (saveCanvas(current->item, filename) == false)
		{
			return false;
		}

		//increment file number if successful
		fileNumber++;
	}

	return true;
}