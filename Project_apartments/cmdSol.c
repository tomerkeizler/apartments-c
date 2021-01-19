/*
Commands sol
*/

#include "cmdHeader.h"


/////////////////////////////////
////// text file functions //////
/////////////////////////////////


// loads commands from a text file
// returns how many commands have been loaded from the file
// ---------------------------------------------------------------------
void loadCommands(OldComList* comList, LastComQueue* comQueue)
{
	FILE* f = fopen("coms.txt", "r");
	if (f == NULL) // there is no such file
		return 0;

	int numComsInFile = 0; // count how many commands are in the file
	char* com = getLine(f); // get the first command from the file

	// store the commands one by one in a list
	OldComList* textFileComs = makeEmptyComList();
	while (com)
	{
		insertCommandToEndList(textFileComs, com);
		numComsInFile++;
		com = getLine(f); // get the next command from the file
	}
	fclose(f);

	// save all the commands in the two data sets of the program:
	// 1) the list of old commands
	// 2) the queue of new commands
	ComListNode* current = textFileComs->tail;
	while (current)
	{
		saveCom(comList, comQueue, current->com);
		current = current->prev;
	}
}


// gets a single command from the text file
// stops reading at the end of the line
// ---------------------------------------------------------------------
char* getLine(FILE* f)
{
	int phySize = 1, logSize = 0; // for a dynamic memory allocation
	char* str = (char*)malloc(sizeof(char));

	char c = fgetc(f); // get the first letter of the command
	if (c == -1)
		return NULL;

	// read another char while didn't arrive to end of the line
	while (c != '\n')
	{
		if (phySize == logSize)
		{
			phySize *= 2;
			str = (char*)realloc(str, sizeof(char) * phySize);
		}
		str[logSize] = c;
		logSize++;
		c = fgetc(f); // get the next letter of the command
	}

	// final size of the command
	str = (char*)realloc(str, sizeof(char) * logSize + 1);
	str[logSize] = '\0';

	return str;
}


// gets the number of commands that have been loaded from the file
// saves the new commands that are not in the file yet
// ---------------------------------------------------------------------
void saveCommands(OldComList* comList, LastComQueue* comQueue)
{
	// calculate the number of commands in both the list and the queue
	// stop if there are no commands to write to the text file
	if (comList->size + comQueue->size == 0)
		return;

	FILE* f = fopen("coms.txt", "w");
	if (f == NULL)
	{
		printf("Failed to open the file!");
		return;
	}

	// write commands from the commands queue to the text file
	for (int i = comQueue->back; i >= comQueue->front; i--)
	{
		fputs(comQueue->short_term_history[i], f);
		fputc('\n', f);
	}

	// if there is anything to write from the commands list
	if (comList->size > 0)
	{
		// write commands from the commands list to the text file
		ComListNode* currentCom = comList->tail;
		while (currentCom)
		{
			fputs(currentCom->com, f);
			fputc('\n', f);
			currentCom = currentCom->prev;
		}
	}

	fclose(f);
}


///////////////////////////////
////// general fucntions //////
///////////////////////////////


// gets a string with a command and its details
// and calls to a function to perform the command
// ---------------------------------------------------------------------
void executeCom(AptList* aptList, OldComList* comList, LastComQueue* comQueue, char* com)
{
	// array of possible commands
	char* commands[NUM_OF_CMDS] = { "get-an-apt -Enter", "buy-an-apt", "delete-an-apt -Enter", "get-an-apt", "add-an-apt", "!!", "!", "short_history", "history", "exit" };

	// search for one of the possible commands in the array
	int i = -1, found = 0;
	char* comLocation;
	while (i + 1 < NUM_OF_CMDS && !found)
		if (comLocation = strstr(com, commands[++i]))
			found = 1;

	// array of pointers to similar commands functions
	void(*funcByNum[])(AptList*, int) = { printLatestApts, buyApt, deleteApt };
	void(*funcByString[])(AptList*, char*) = { getApt, addApt };

	if (found) // if recognzied a known command
	{
		switch (i) // perform the command
		{
		case 0: // get-an-apt -Enter <num>
		case 1: // buy-an-apt <num>
		case 2: // delete-an-apt -Enter <num>
			funcByNum[i](aptList, atoi(comLocation + strlen(commands[i]))); // send the number
			break;

		case 3: // get-an-apt "parameters..."
		case 4: // add-an-apt "details..."
			funcByString[i - 3](aptList, comLocation + strlen(commands[i]) + 1); // send the details
			break;

		case 5: // !!
			execLastCom(aptList, comList, comQueue);
			break;

		case 6: // !... (2 options)
		{
			// 1) !<num>^str1^str2
			if (strstr(com, "^")) // the sign ^ appears
			{
				char* token = strtok(com, "^");
				int commandNum = atoi(token + 1); // the requested command number
				char* str1 = strtok(NULL, "^"); // a string to replace
				char* str2 = strtok(NULL, "^"); // a replacement string
				execComStrReplace(aptList, comList, comQueue, commandNum, str1, str2);
			}

			// 2) !<num>
			else  // the sign ^ does not appear
				execComByNum(aptList, comList, comQueue, atoi(com + 1)); // send a command  number
			break;
		}

		case 7: // short_history
			printShortHistory(comList->size, comQueue);
			break;

		case 8: // history
			PrintAllHistory(comList, comQueue);
			break;

		case 9: // exit
			printf("Good Bye!\n");
			break;
		}
	}

	// only save apartment commands:
	// "get-an-apt", "add-an-apt", "buy-an-apt", "delete-an-apt"
	if (i >= 0 && i <= 4)
		saveCom(comList, comQueue, com);
}


// saves a new command in the commands queue
// ---------------------------------------------------------------------
void saveCom(OldComList* comList, LastComQueue* comQueue, char* comToSave)
{
	if (isFullQueue(comQueue)) // the queue is full
	{
		// tansfer the oldest command from the queue to the commands list
		insertCommandToEndList(comList, dequeueCommand(comQueue));

		// shift the queue one step forward
		moveQueueForward(comQueue);
	}
	// insert the new command to the queue
	enqueueCommand(comQueue, comToSave);
}


// prints a single command to the screen
// ---------------------------------------------------------------------
void printCom(int num, char* com)
{
	printf("%d: %s\n", num, com);
}


// gets a number of a command and returns its content
// ---------------------------------------------------------------------
char* getComByNum(OldComList* comList, LastComQueue* comQueue, int num)
{
	char* requestedCom;
	if (num > comList->size) // the command is in the queue
		requestedCom = getQueueComByNum(comQueue, num - comList->size);
	else // the command is in the list
		requestedCom = getListComByNum(comList, num);
	return requestedCom;
}


// gets a command cmd, a pattern to replace in it, and a replacement
// returns the new command after all replacemnts
// ---------------------------------------------------------------------
char* replaceCom(const char *cmd, const char *pattern, const char *replace)
{
	// there is nothing to replace with an empty pattern
	if (strlen(pattern) == 0)
		return cmd;

	int count; // number of occurences of pattern in cmd
	char* rest = cmd; // address of the rest of cmd to search for patterns in
	char* next = strstr(rest, pattern); // address of the next pattern in cmd

	// count the number of occurences of the pattern in cmd
	// run the loop as long as a next pattern is found
	for (count = 0; next; count++)
	{
		rest = next + strlen(pattern);
		next = strstr(rest, pattern);
	}

	// calculate the size of the new command after replacements
	int newSize = strlen(cmd) + count * (strlen(replace) - strlen(pattern));
	char* res = malloc(sizeof(char) * newSize + 1); // the new command

	int location = 0; // for iterating over the new command
	next = strstr(cmd, pattern); // address of the next pattern in cmd
	
	while (next) { // run as long as a next pattern is found in cmd

		// copy the part of cmd before the pattern
		memcpy(res + location, cmd, next - cmd);
		location += next - cmd;

		// skip the pattern in cmd
		cmd = next + strlen(pattern);

		// copy the replacement instead of pattern
		strcpy(res + location, replace);
		location += strlen(replace);

		// address of the next pattern in cmd
		next = strstr(cmd, pattern);
	}

	// copy the remaining part of cmd
	strcpy(res + location, cmd);

	return res;
}


// gets a string and returns an exact copy of it
// ---------------------------------------------------------------------
char* duplicateStr(char* str)
{
	char* dup = (char*)malloc(sizeof(char)*strlen(str)+1);
	strcpy(dup, str);
	return dup;
}


// gets an input of a single command from the user
// stop reading at the end of the line
// ---------------------------------------------------------------------
char* getLineInput()
{
	int phySize = 1, logSize = 0; // for a dynamic memory allocation
	char* str = (char*)malloc(sizeof(char));

	char c = getchar(); // get the first letter of the command

	while (c != '\n') // read another char while didn't arrive to end of the line
	{
		if (phySize == logSize)
		{
			phySize *= 2;
			str = (char*)realloc(str, sizeof(char) * phySize);
		}
		str[logSize] = c;
		logSize++;
		c = getchar(); // get the next letter of the command
	}

	// final size of the command
	str = (char*)realloc(str, sizeof(char) * logSize + 1);
	str[logSize] = '\0';

	return str;
}


///////////////////////////////
////// restore functions //////
///////////////////////////////


// performs the command "!!"
// ---------------------------------------------------------------------
void execLastCom(AptList* aptList, OldComList* comList, LastComQueue* comQueue)
{
	char* lastCom = getLastQueueCom(comQueue);
	executeCom(aptList, comList, comQueue, duplicateStr(lastCom));
}


// performs the command "!<num>"
// ---------------------------------------------------------------------
void execComByNum(AptList* aptList, OldComList* comList, LastComQueue* comQueue, int num)
{
	char* requestedCom = getComByNum(comList, comQueue, num);
	executeCom(aptList, comList, comQueue, duplicateStr(requestedCom));
}


// performs the command "short_history"
// ---------------------------------------------------------------------
void printShortHistory(int sizeOldComs, LastComQueue* comQueue)
{
	// print commands from the queue
	for (int i = 1; i <= comQueue->size; i++)
		printCom(sizeOldComs + i, getQueueComByNum(comQueue, i));
}


// performs the command "history"
// ---------------------------------------------------------------------
void PrintAllHistory(OldComList* comList, LastComQueue* comQueue)
{
	// print commands from the list of old commands
	if (comList->size != 0)
	{
		int i = 1;
		ComListNode* current = comList->head;

		while (current)
		{
			printCom(i, current->com);
			current = current->next;
			i++;
		}
	}
	// print commands from the queue
	printShortHistory(comList->size, comQueue);
}


// performs the command "!<num>^str1^str2"
// ---------------------------------------------------------------------
void execComStrReplace(AptList* aptList, OldComList* comList, LastComQueue* comQueue, int num, char* str1, char* str2)
{
	// get the original command and create a new command based on it
	char* originCommand = getComByNum(comList, comQueue, num);
	char* newCommand = replaceCom(originCommand, str1, str2);
	
	// perform the new command
	executeCom(aptList, comList, comQueue, newCommand);
}


//////////////////////////////////////
////// Commands queue fucntions //////
//////////////////////////////////////


// creates an empty queue of commands
// ---------------------------------------------------------------------
LastComQueue* makeEmptyQueue()
{
	LastComQueue* comQueue = (LastComQueue*)malloc(sizeof(LastComQueue));
	for (int i = 0; i < N; i++)
		comQueue->short_term_history[i] = NULL;
	comQueue->front = comQueue->size = 0;
	comQueue->back = N - 1;
	return comQueue;
}


// frees the commands queue
// ---------------------------------------------------------------------
void freeQueue(LastComQueue* comQueue)
{
	for (int i = 0; i < N; i++)
		free(comQueue->short_term_history[i]);
	free(comQueue);
}


// inserts a new command to the queue
// ---------------------------------------------------------------------
void enqueueCommand(LastComQueue* comQueue, char* comToAdd)
{
	comQueue->back = (comQueue->back + 1) % N; // updates the index of the newest command
	comQueue->short_term_history[comQueue->back] = comToAdd;
	comQueue->size++;
}


// removes a command from the queue and returns it
// ---------------------------------------------------------------------
char* dequeueCommand(LastComQueue* comQueue)
{
	if (isEmptyQueue(comQueue)) // nothing to remove if the queue is empty
		return NULL;

	char* comToDetele = comQueue->short_term_history[comQueue->front];
	comQueue->front = (comQueue->front + 1) % N; // updates the index of the oldest command
	comQueue->size--;
	return comToDetele;
}


// shifts the queue cells one step forward
// ---------------------------------------------------------------------
void moveQueueForward(LastComQueue* comQueue)
{
	if (comQueue->front == 0) // cannot move forward if the front cell is full
		return;

	for (int i = comQueue->front - 1; i < comQueue->back; i++)
		comQueue->short_term_history[i] = comQueue->short_term_history[i + 1];
	comQueue->short_term_history[comQueue->back] = NULL;

	// update the indexes of the newest and oldest command in the queue
	comQueue->back--;
	comQueue->front--;
}


// returns 1 if the queue is full, or 0 otherwise
// ---------------------------------------------------------------------
int isFullQueue(LastComQueue* comQueue)
{
	return (comQueue->size == N);
}


// returns 1 if the queue is empty, or 0 otherwise
// ---------------------------------------------------------------------
int isEmptyQueue(LastComQueue* comQueue)
{
	return (comQueue->size == 0);
}


// returns a command by its number in the queue
// ---------------------------------------------------------------------
char* getQueueComByNum(LastComQueue* comQueue, int num)
{
	return comQueue->short_term_history[comQueue->front + num - 1];
}


// returns the oldest command in the queue
// ---------------------------------------------------------------------
char* getLastQueueCom(LastComQueue* comQueue)
{
	return comQueue->short_term_history[comQueue->back];
}


/////////////////////////////////////
////// Commands list fucntions //////
/////////////////////////////////////


// creates an empty list of commands
// ---------------------------------------------------------------------
OldComList* makeEmptyComList()
{
	OldComList* comList = (OldComList*)malloc(sizeof(OldComList));
	comList->head = comList->tail = NULL;
	comList->size = 0;
	return comList;
}


// frees the commands list
// ---------------------------------------------------------------------
void freeComList(OldComList* comList)
{
	ComListNode* current = comList->head;
	ComListNode* next;

	while (current)
	{
		next = current->next;
		free(current->com);
		free(current);
		current = next;
	}
	comList->head = NULL;
	comList->tail = NULL;
	free(comList);
}


// inserts a new command to the list
// ---------------------------------------------------------------------
void insertCommandToEndList(OldComList* comList, char* comToAdd)
{
	ComListNode* newTail = (ComListNode*)malloc(sizeof(ComListNode));
	newTail->prev = NULL;
	newTail->next = NULL;
	newTail->com = comToAdd;
	comList->size++;

	if (isEmptyComList(*comList))
		comList->head = comList->tail = newTail;
	else
	{
		newTail->prev = comList->tail;
		comList->tail->next = newTail;
		comList->tail = newTail;
	}
}


// returns 1 if the list is empty, or 0 otherwise
// ---------------------------------------------------------------------
int isEmptyComList(OldComList comList)
{
	return (comList.head == NULL);
}


// returns a command by its number in the list
// ---------------------------------------------------------------------
char* getListComByNum(OldComList* comList, int num)
{
	if (comList->size < num)
		return NULL;

	int i = 1;
	ComListNode* current = comList->head;

	while (i < num)
	{
		current = current->next;
		i++;
	}
	return current->com;
}
