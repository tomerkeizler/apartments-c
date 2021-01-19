#include "aptHeader.h"
#include "cmdHeader.h"


void main()
{
	//------------------------------------------------------
	// ------------------ prompt for user ------------------
	//------------------------------------------------------

	printf("Please enter one of the following commands:\n");
	printf("add-an-apt, get-an-apt, buy-an-apt or delete-an-apt\n");
	printf("For reconstruction commands, please enter:\n");
	printf("!!, !num, history, short_history or !num^str1^str2\n");
	printf("To exit, enter exit.\n");

	//-------------------------------------------------------
	// -------------------- data loading --------------------
	//-------------------------------------------------------

	// load apartments from the binary file
	AptList* myAptList = createEmptyAptList();
	loadApts(myAptList);

	// load commands from the text file
	LastComQueue* myComQueue = makeEmptyQueue();
	OldComList* myComList = makeEmptyComList();
	loadCommands(myComList, myComQueue);

	//---------------------------------------------------------
	// ------------ getting commands from the user ------------
	//---------------------------------------------------------
	
	printf(">> ");
	char* input;
	while (input = getLineInput()) // get the next input command from the user
	{
		executeCom(myAptList, myComList, myComQueue, input); // perform the command
		if (strcmp(input, "exit") == 0)
			break; // exit the program
		printf(">> ");
	}
	
	//------------------------------------------------------
	// -------------------- data saving --------------------
	//------------------------------------------------------

	// save apartments in the binary file
	saveApts(myAptList);

	// save commands in the text file
	saveCommands(myComList, myComQueue);

	//--------------------------------------------------------
	// ----------- free dynamic memory allocations -----------
	//--------------------------------------------------------

	freeAptList(myAptList);
	freeComList(myComList);
	freeQueue(myComQueue);
}
