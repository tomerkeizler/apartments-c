/*
aptSol.c

This file contains the implementations of the functions concerning the apartments.
The functions' prototypes are located in the file "aptHeader.h".
*/

#include "aptHeader.h"

AptList* createEmptyAptList()
{	//This function creates and returns a pointer to an empty apartment list.

	AptList* newAptList;

	newAptList = (AptList*)calloc(1, sizeof(AptList));

	return newAptList;
}

void buyApt(AptList* aptList, int aptCode)
{	//This function receives a pointer to an apartment list and a code for an apartment to be bought.
	//The function removes the apartment with that code from the list.

	AptListNode* nodeToRemove;

	nodeToRemove = aptList->head;

	while (nodeToRemove->apt->code != aptCode) //Find the node of the apartment to be removed
		nodeToRemove = nodeToRemove->next;

	removeAptListNode(aptList, nodeToRemove);
}

void removeAptListNode(AptList* aptList, AptListNode* nodeToRemove)
{	//This function receives a pointer to an apartment list and a pointer to a node to be removed from
	//the list. The function removes the node from the list.

	if (nodeToRemove->apt->code == aptList->lastCode)
		aptList->lastCode--;

	if (aptList->head == aptList->tail) //List is only one node (and it's the node to remove)
		aptList->head = aptList->tail = NULL;

	else if (aptList->tail == nodeToRemove)
	{	//List is more than one node, the node to remove is the tail
		aptList->tail = nodeToRemove->prev;
		aptList->tail->next = NULL;
	}

	else if (aptList->head == nodeToRemove)
	{	//List is more than one node, the node to remove is the head
		aptList->head = nodeToRemove->next;
		aptList->head->prev = NULL;
	}

	else
	{	//List is more than one node, the node to remove is neither the head nor the tail
		nodeToRemove->next->prev = nodeToRemove->prev;
		nodeToRemove->prev->next = nodeToRemove->next;
	}

	free(nodeToRemove->apt->address);
	free(nodeToRemove->apt);
	free(nodeToRemove);
}

void addApt(AptList* aptList, char* aptInfo)
{	//This fucntion receives a pointer to the current apartment list and a string with the info of a 
	//new apartment. The function creates a new apartment according to the info and inserts it to the
	//apartment list in it's right place (the list is sorted from the cheapest apartment to
	//the most expensive).

	char* address, *priceStr, *numRoomsStr, *dayStr, *monthStr, *yearStr, *aptInfoCopy;
	int price, numRooms;
	short int enterDay, enterMonth, enterYear;
	Date enterDate, regDate;
	AptListNode* nextApt, *prevApt;

	aptInfoCopy = (char*)malloc(strlen(aptInfo) + 1);
	strcpy(aptInfoCopy, aptInfo);

	nextApt = (AptListNode*)malloc(sizeof(AptListNode));
	prevApt = (AptListNode*)malloc(sizeof(AptListNode));
	//Get apt info from the input string
	address = strtok(aptInfoCopy, "\"");
	priceStr = strtok(NULL, " ");
	numRoomsStr = strtok(NULL, " ");
	dayStr = strtok(NULL, " ");
	monthStr = strtok(NULL, " ");
	yearStr = strtok(NULL, " ");

	//Conversions from strings
	sscanf(priceStr, "%d", &price);
	sscanf(numRoomsStr, "%d", &numRooms);
	sscanf(dayStr, "%hd", &enterDay);
	sscanf(monthStr, "%hd", &enterMonth);
	sscanf(yearStr, "%hd", &enterYear);
	enterYear += 2000;

	enterDate = makeDate(enterDay, enterMonth, enterYear);

	regDate = getCurrTime();

	findAptLocation(aptList, price, &nextApt, &prevApt);
	addAptToList(aptList, nextApt, prevApt, address, price, numRooms, enterDate, regDate, NO_CODE);
	free(aptInfoCopy);
}

Date makeDate(short int day, short int month, short int year)
{	//This function receives three numbers representing day, month and year and returns a Date struct
	//with the input data.

	Date newDate;

	newDate.day = day;
	newDate.month = month;
	newDate.year = year;

	return newDate;
}

Date getCurrTime()
{	//This function returns the current system's time in a Date struct, containing day, month and year.

	time_t rawtime;
	struct tm * timeinfo;
	char* timeStr, *token;
	short int month;
	Date currTime;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	timeStr = asctime(timeinfo);

	token = strtok(timeStr, " "); //ww, skip
	token = strtok(NULL, " "); //mm
	month = getMonthFromStr(token);
	currTime.month = month;
	token = strtok(NULL, " "); //dd
	sscanf(token, "%hd", &currTime.day);
	token = strtok(NULL, " "); //hh:mm:ss, skip
	token = strtok(NULL, " "); //yyyy
	sscanf(token, "%hd", &currTime.year);

	return currTime;
}

short int getMonthFromStr(char* month)
{	//This function receives a month as a string and returns it's numeral value.

	if (!strcmp(month, "Jan"))
		return 1;
	if (!strcmp(month, "Feb"))
		return 2;
	if (!strcmp(month, "Mar"))
		return 3;
	if (!strcmp(month, "Apr"))
		return 4;
	if (!strcmp(month, "May"))
		return 5;
	if (!strcmp(month, "Jun"))
		return 6;
	if (!strcmp(month, "Jul"))
		return 7;
	if (!strcmp(month, "Aug"))
		return 8;
	if (!strcmp(month, "Sep"))
		return 9;
	if (!strcmp(month, "Oct"))
		return 10;
	if (!strcmp(month, "Nov"))
		return 11;
	if (!strcmp(month, "Dec"))
		return 12;
}

void findAptLocation(AptList* aptList, int aptPrice, AptListNode** nextApt, AptListNode** prevApt)
{	//This function receives a pointer to the current apartment list, an apartment price and two output 
	//parameters. The function finds the location in which the new apartment is to be added according to it's
	//price, and updates the output parameters with it.

	AptListNode* currNode;
	int found = FALSE;

	//Case 1: empty list
	if (isEmptyList(aptList))
	{
		*nextApt = *prevApt = NULL;
		found = TRUE;
	}

	//Case 2: list with one node
	else if (aptList->head == aptList->tail)
	{
		if (aptList->head->apt->price < aptPrice)
		{
			*nextApt = NULL;
			*prevApt = aptList->head;
		}

		else
		{
			*prevApt = NULL;
			*nextApt = aptList->head;
		}

		found = TRUE;
	}

	//Case 3: list with more than one node
	else if (aptPrice <= aptList->head->apt->price) //New apt is the cheapest, so it's the new head
	{
		found = TRUE;
		*prevApt = NULL;
		*nextApt = aptList->head;
	}

	else //New apt isn't the cheapest, find it's location in the list
	{
		currNode = aptList->head;
		while (currNode && !found)
		{
			if (currNode->next)
			{
				if (currNode->apt->price <= aptPrice && currNode->next->apt->price >= aptPrice)
				{
					found = TRUE;
					*prevApt = currNode;
					*nextApt = currNode->next;
				}
			}

			else //we reached the tail, new apt is with the highest price
			{
				found = TRUE;
				*prevApt = currNode;
				*nextApt = NULL;
			}
			currNode = currNode->next;
		}
	}
}

int isEmptyList(AptList* aptList)
{	//This function receives a pointer to an apartment list and returns true if it's empty, false otherwise.

	return aptList->head == NULL;
}

void addAptToList(AptList* aptList, AptListNode* nextApt, AptListNode* prevApt, char* address,
	int price, int numRooms, Date enterDate, Date regDate, int aptCode)
{	//This function receives a pointer to the current apartment list, two pointers to the nodes indicating
	//the location of the new apartment to be added in the list and the apartment's info. The function
	//creates a new apartment according to the input info and adds it to the list.

	AptListNode* newAptNode;

	if (aptCode == NO_CODE)
	{
		aptCode = aptList->lastCode + 1;
		aptList->lastCode++;
	}

	newAptNode = createNewAptListNode(nextApt, prevApt);
	newAptNode->apt = createNewApt(aptCode, address, price, numRooms, enterDate, regDate);

	//Check if new apartment node is the new head or tail:
	if (!nextApt && !prevApt)
		aptList->head = aptList->tail = newAptNode;

	else if (!nextApt)
	{
		aptList->tail = newAptNode;
		newAptNode->prev->next = newAptNode;
	}

	else if (!prevApt)
	{
		aptList->head = newAptNode;
		newAptNode->next->prev = newAptNode;
	}

	else
	{
		newAptNode->next->prev = newAptNode;
		newAptNode->prev->next = newAptNode;
	}
}

AptListNode* createNewAptListNode(AptListNode* next, AptListNode* prev)
{	//This function receives two apt list node pointers indicating a new apt node's location in the list.
	//It creates a new apt node with the location info and returns it.

	AptListNode* newAptNode;

	newAptNode = (AptListNode*)malloc(sizeof(AptListNode));
	newAptNode->next = next;
	newAptNode->prev = prev;

	return newAptNode;
}

Apartment* createNewApt(int code, char* address, int price, int numRooms, Date enterDate, Date regDate)
{	//This function receives all the required info for an apartment, creates the apartment and returns it.

	Apartment* newApt;

	newApt = (Apartment*)malloc(sizeof(Apartment));
	newApt->address = (char*)malloc(strlen(address) + 1);
	strcpy(newApt->address, address);
	newApt->code = code;
	newApt->enterDate = enterDate;
	newApt->numRooms = numRooms;
	newApt->price = price;
	newApt->regDate = regDate;

	return newApt;
}

void printLatestApts(AptList* aptList, int numOfDays)
{	//This function receives a pointer to the current apartment list and a number of days, x.
	//The function prints the codes of the apartments that were added to the pool in the last x days.

	int* latestAptCodes;
	int size, i;

	latestAptCodes = getLatestAptCodes(aptList, numOfDays, &size);

	for (i = 0; i < size; i++)
		printf("Apt Code: %d\n", latestAptCodes[i]);

	free(latestAptCodes);
}

time_t dateToTimeT(int day, int month, int year) {
	//This function receives three numbers representing a date and returns their time_t equivalent.

	time_t rawtime;
	struct tm * timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	timeinfo->tm_year = year - 1900;
	timeinfo->tm_mon = month - 1;
	timeinfo->tm_mday = day;

	return mktime(timeinfo);
}

void deleteApt(AptList* aptList, int numOfDays)
{	//This function receives a pointer to the current apartment and a number of days, x.
	//The function removes the apartments that were added to the pool in the last x days.

	int* latestAptCodes;
	int size = 0, i;

	latestAptCodes = getLatestAptCodes(aptList, numOfDays, &size);

	for (i = 0; i < size; i++)
		buyApt(aptList, latestAptCodes[i]);

	free(latestAptCodes);
}

int* getLatestAptCodes(AptList* aptList, int numOfDays, int* size)
{	//This function receives a pointer to the current apartment list and a number of days, x.
	//The function creates and returns an array of the codes of the apartments that were added to the pool
	//in the last x days. The function also saves the array's size in the output parameter, size.

	Date currDate;
	AptListNode* currNode;
	double sec;
	time_t currTime, currNodeTime;
	int daysDiff, phySize = 1, logSize = 0;
	int* latestAptCodes;

	latestAptCodes = (int*)malloc(sizeof(int));
	currDate = getCurrTime();
	currTime = dateToTimeT(currDate.day, currDate.month, currDate.year);
	currNode = aptList->head;
	while (currNode) 
	{	//Loop that adds the codes of the apts that were added in the last numOfDays to the array
		currNodeTime = dateToTimeT(currNode->apt->regDate.day, currNode->apt->regDate.month, currNode->apt->regDate.year);
		sec = difftime(currTime, currNodeTime);
		daysDiff = (int)(sec / (60 * 60 * 24));

		if (daysDiff <= numOfDays) //current apartment was added in the last numOfDays
		{
			if (phySize == logSize)
			{
				phySize *= 2;
				latestAptCodes = (int*)realloc(latestAptCodes, sizeof(int) * phySize);
			}

			latestAptCodes[logSize] = currNode->apt->code;
			logSize++;
		}
		currNode = currNode->next;
	}

	if (phySize > logSize)
		latestAptCodes = (int*)realloc(latestAptCodes, sizeof(int) * logSize);

	*size = logSize;
	return latestAptCodes;
}

void getAptByMaxRooms(AptList* filteredAptList, int maxRooms)
{	//This function receives a pointer to the current filtered apartment list and a number of max rooms.
	//The function removes the apartments in the filtered list that have more rooms than the input number.

	AptListNode* currNode, *next;

	currNode = filteredAptList->head;
	while (currNode) 
	{	//Loop that filters out the apts with more rooms than maxRooms from the list
		if (currNode->apt->numRooms > maxRooms)
		{
			next = currNode->next;
			buyApt(filteredAptList, currNode->apt->code);
			currNode = next;
		}

		else
			currNode = currNode->next;
	}
}

void getAptByMinRooms(AptList* filteredAptList, int minRooms)
{	//This function receives a pointer to the current filtered apartment list and a number of min rooms.
	//The function removes the apartments in the filtered list that have less rooms than the input number.

	AptListNode* currNode, *next;

	currNode = filteredAptList->head;
	while (currNode)
	{	//Loop that filters out the apts with less rooms than minRooms from the list
		if (currNode->apt->numRooms < minRooms)
		{
			next = currNode->next;
			buyApt(filteredAptList, currNode->apt->code);
			currNode = next;
		}

		else
			currNode = currNode->next;
	}
}

void getAptByMaxPrice(AptList* filteredAptList, int maxPrice)
{	//This function receives a pointer to the current filtered apartment list and a number of max price.
	//The function removes the apartments in the filtered list that their price is higher than
	//the input number.

	AptListNode* currNode, *next;

	currNode = filteredAptList->head;
	while (currNode)
	{	//Loop that filters out the apts that cost more than maxPrice from the list
		if (currNode->apt->price > maxPrice)
		{
			next = currNode->next;
			buyApt(filteredAptList, currNode->apt->code);
			currNode = next;
		}

		else
			currNode = currNode->next;
	}
}

void getAptByDate(AptList* filteredAptList, Date maxDate)
{	//This function receives a pointer to the current filtered apartment list and a Date struct,
	//representing the max date of entering an apartment. The function removes the apartments in the
	//filtered list that their enter date is further than the max date.

	time_t maxDateT, currNodeDateT;
	AptListNode* currNode, *next;
	double sec;
	int daysDiff;

	maxDateT = dateToTimeT(maxDate.day, maxDate.month, maxDate.year);
	currNode = filteredAptList->head;
	while (currNode)
	{	//Loop that filters out the apts that their enter date is further than maxDate
		currNodeDateT = dateToTimeT(currNode->apt->enterDate.day, currNode->apt->enterDate.month, currNode->apt->enterDate.year);
		sec = difftime(maxDateT, currNodeDateT);
		daysDiff = (int)(sec / (60 * 60 * 24));

		if (daysDiff < 0)
		{ //current apartment's enter date is further than maxDate
			next = currNode->next;
			buyApt(filteredAptList, currNode->apt->code);
			currNode = next;
		}

		else
			currNode = currNode->next;
	}
}

void getApt(AptList* fullAptList, char* filters)
{	//This function receives a pointer to an apartment list and a string containing all the filters
	//for printing apartments. The function prints the relavant apartments' details, according to the 
	//input filters.

	AptList* filteredAptList;
	AptListNode* currNode;
	char* filter, *filtersCopy;
	char day[2], month[2], year[4];
	Date dateParam;
	int printFromTail = FALSE, res;

	filtersCopy = (char*)malloc(strlen(filters) + 1);
	strcpy(filtersCopy, filters);
	filteredAptList = createAptListCopy(fullAptList);
	filter = strtok(filtersCopy, " ");
	while (filter) //Loop that filters the copy list according to all of the input filters
	{
		if (!strcmp(filter + 1, "MaximumPrice"))
		{
			filter = strtok(NULL, " ");
			getAptByMaxPrice(filteredAptList, atoi(filter));
		}

		else if (!strcmp(filter + 1, "MinimumNumRooms"))
		{
			filter = strtok(NULL, " ");
			getAptByMinRooms(filteredAptList, atoi(filter));
		}

		else if (!strcmp(filter + 1, "MaximumNumRooms"))
		{
			filter = strtok(NULL, " ");
			getAptByMaxRooms(filteredAptList, atoi(filter));
		}

		else if (!strcmp(filter + 1, "Date"))
		{
			filter = strtok(NULL, " ");
			day[0] = filter[0];
			day[1] = filter[1];
			month[0] = filter[2];
			month[1] = filter[3];
			year[0] = filter[4];
			year[1] = filter[5];
			year[2] = filter[6];
			year[3] = filter[7];
			dateParam = makeDate((short int)atoi(day), (short int)atoi(month), (short int)atoi(year));
			getAptByDate(filteredAptList, dateParam);
		}

		else if (!strcmp(filter + 1, "sr"))
			printFromTail = TRUE;

		filter = strtok(NULL, " ");
	}

	if (!printFromTail)
		printAptListFromHead(filteredAptList);

	else
		printAptListFromTail(filteredAptList);

	freeAptList(filteredAptList);
	free(filtersCopy);
}

void printAptListFromHead(AptList* filteredAptList)
{	//This function receives a pointer to an apartment list and prints it's apartments' details
	//from the head to the tail.
	AptListNode* currNode;

	currNode = filteredAptList->head;
	while (currNode)
	{
		printf("Apt details:\nCode: %d\nAddress: %s\nNumber of rooms: %d\nPrice: %d\nEntry date: %hd.%hd.%hd\nDatabase entry date: %hd.%hd.%hd\n",
			currNode->apt->code, currNode->apt->address, currNode->apt->numRooms, currNode->apt->price, currNode->apt->enterDate.day,
			currNode->apt->enterDate.month, currNode->apt->enterDate.year, currNode->apt->regDate.day,
			currNode->apt->regDate.month, currNode->apt->regDate.year);
		currNode = currNode->next;
	}
}

void printAptListFromTail(AptList* filteredAptList)
{	//This function receives a pointer to an apartment list and prints it's apartments' details
	//from the tail to the head.
	AptListNode* currNode;

	currNode = filteredAptList->tail;
	while (currNode)
	{
		printf("Apt details:\nCode: %d\nAddress: %s\nNumber of rooms: %d\nPrice: %d\nEntry date: %hd.%hd.%hd\nDatabase entry date: %hd.%hd.%hd\n",
			currNode->apt->code, currNode->apt->address, currNode->apt->numRooms, currNode->apt->price, currNode->apt->enterDate.day,
			currNode->apt->enterDate.month, currNode->apt->enterDate.year, currNode->apt->regDate.day,
			currNode->apt->regDate.month, currNode->apt->regDate.year);
		currNode = currNode->prev;
	}
}

int getAptListSize(AptList* aptList)
{	//This function receives a pointer to an apartment list and returns the number of apartments in contains.

	AptListNode* currNode;
	int res = 0;

	currNode = aptList->head;
	while (currNode)
	{
		res++;
		currNode = currNode->next;
	}

	return res;
}

AptList* createAptListCopy(AptList* aptList)
{	//This function receives a pointer to an apartment list. It creates and returns an apartment list
	//which is an exact copy of the input apartment list.

	AptList* aptListCopy;
	Apartment* aptCopy;
	Apartment** aptCopyArray;
	AptListNode* currNodeOrig, *currNodeCopy;
	int aptListSize, i = 0;

	aptListSize = getAptListSize(aptList);

	aptCopyArray = (Apartment**)malloc(aptListSize * sizeof(Apartment*));
	aptListCopy = (AptList*)calloc(1, sizeof(AptList));
	aptListCopy->lastCode = aptList->lastCode;

	currNodeOrig = aptList->head;
	while (currNodeOrig)
	{
		//Make the apt copy:
		aptCopyArray[i] = (Apartment*)malloc(sizeof(Apartment));
		aptCopyArray[i]->address = (char*)malloc(strlen(currNodeOrig->apt->address) + 1);
		strcpy(aptCopyArray[i]->address, currNodeOrig->apt->address);
		aptCopyArray[i]->price = currNodeOrig->apt->price;
		aptCopyArray[i]->code = currNodeOrig->apt->code;
		aptCopyArray[i]->enterDate = currNodeOrig->apt->enterDate;
		aptCopyArray[i]->numRooms = currNodeOrig->apt->numRooms;
		aptCopyArray[i]->regDate = currNodeOrig->apt->regDate;

		if (isEmptyList(aptListCopy))
			insertAptToTail(aptListCopy, aptCopyArray[i], NULL);
		else
			insertAptToTail(aptListCopy, aptCopyArray[i], aptListCopy->tail);

		currNodeOrig = currNodeOrig->next;
		i++;
	}

	return aptListCopy;
}

void insertAptToTail(AptList* aptList, Apartment* apt, AptListNode* prev)
{	//This function receives a pointer to an apartment list and a pointer to an apartment.
	//It creates a new node with the input apartment pointer and inserts it to the list's tail.

	AptListNode* newNode;

	newNode = createNewAptListNode(NULL, prev);
	newNode->apt = apt;

	if (isEmptyList(aptList))
		aptList->head = aptList->tail = newNode;

	else
	{
		aptList->tail->next = newNode;
		aptList->tail = newNode;
	}
}

void freeAptList(AptList* aptList)
{	//This function receives a pointer to an apartment list and frees all it's allocated memory.
	AptListNode* currNode, *next;

	currNode = aptList->head;
	while (currNode)
	{
		next = currNode->next;
		free(currNode->apt->address);
		free(currNode->apt);
		free(currNode);
		currNode = next;
	}
	free(aptList);
}

void saveApts(AptList* aptList)
{	//This function receives a pointer to an apartment list and saves it's contents in a binary file.
	//The saving format will be as follows:
	//1. Code - short int. 2. Address - length of the string as short int, then the string without '\0'.
	//3. Price - int. 4. 20 bits (3 bytes) that will represent number of rooms and entry date (4 msb are
	//number of rooms, next 5 bits are entry day, next 4 bits are entry month and 7 next bits are entry year).
	//5. 2 bytes that will represent the apt's registry date in the pool.

	FILE* aptBinFile;
	AptListNode* currNode;
	char aptBinFileName[] = "apartments.bin";
	short int currAddressLen;
	BYTE bToWriteEnter;
	TWOBYTES tbToWriteEnter, tbToWriteReg;

	aptBinFile = fopen(aptBinFileName, "wb");
	currNode = aptList->head;
	while (currNode)
	{
		currAddressLen = (short int)strlen(currNode->apt->address);
		fwrite(&((short int)currNode->apt->code), sizeof(short int), 1, aptBinFile);
		fwrite(&currAddressLen, sizeof(short int), 1, aptBinFile);
		fwrite(currNode->apt->address, sizeof(char), currAddressLen, aptBinFile);
		fwrite(&currNode->apt->price, sizeof(int), 1, aptBinFile);

		bToWriteEnter = getByteData(currNode->apt, &tbToWriteEnter);
		tbToWriteReg = getTwoByteData(currNode->apt->regDate);

		fwrite(&bToWriteEnter, sizeof(BYTE), 1, aptBinFile);
		fwrite(&tbToWriteEnter, sizeof(TWOBYTES), 1, aptBinFile);
		fwrite(&tbToWriteReg, sizeof(TWOBYTES), 1, aptBinFile);

		currNode = currNode->next;
	}

	fclose(aptBinFile);
}

void loadApts(AptList* aptList)
{	//This function receives a pointer to an empty list of apartments and loads all the apartments
	//from the existing binary file to it.

	FILE* aptBinFile;
	AptListNode* nextApt, *prevApt;
	BYTE firstByte;
	TWOBYTES nextBytes, regDateBytes, enterDateBytes;
	Date enterDate, regDate;
	char aptBinFileName[] = "apartments.bin";
	long int aptFileSize;
	int lastCode = 0, price, numRooms;
	short int code, addressLen;
	char* address;

	aptBinFile = fopen(aptBinFileName, "rb");
	if (!aptBinFile)
		return;
	aptFileSize = fileSize(aptBinFile);
	nextApt = (AptListNode*)malloc(sizeof(AptListNode));
	prevApt = (AptListNode*)malloc(sizeof(AptListNode));

	while (ftell(aptBinFile) < aptFileSize)
	{
		//read simple data from binary file:
		fread(&code, sizeof(short int), 1, aptBinFile);
		fread(&addressLen, sizeof(short int), 1, aptBinFile);
		address = (char*)calloc(addressLen + 1, sizeof(char));
		fread(address, sizeof(char), addressLen, aptBinFile);
		fread(&price, sizeof(int), 1, aptBinFile);
		fread(&firstByte, sizeof(BYTE), 1, aptBinFile);
		fread(&nextBytes, sizeof(TWOBYTES), 1, aptBinFile);
		fread(&regDateBytes, sizeof(TWOBYTES), 1, aptBinFile);

		//read data in bits from binary file:
		numRooms = (firstByte >> 4);
		regDate = getDateFromTwoBytes(regDateBytes);
		enterDateBytes = firstByte;
		enterDateBytes = enterDateBytes << 12;
		enterDateBytes |= (nextBytes >> 4);
		enterDate = getDateFromTwoBytes(enterDateBytes);

		//create the apartment and add it to the list:
		findAptLocation(aptList, price, &nextApt, &prevApt);
		addAptToList(aptList, nextApt, prevApt, address, price, numRooms, enterDate, regDate, code);

		if (code > aptList->lastCode)
			aptList->lastCode = code;

		free(address);
	}

	fclose(aptBinFile);
}

BYTE getByteData(Apartment* apt, TWOBYTES* tbToWrite)
{	//This function receives a pointer to an Apartment and an output parameter of type TWOBYTES*.
	//The function returns the first byte of of the input apartment's data to be written to the binary file,
	//and the second and third bytes are updated in the output parameter.

	BYTE bToWrite, enterDay;
	TWOBYTES enterMonth, enterYear;

	bToWrite = apt->numRooms;
	bToWrite = (bToWrite << 4);
	enterDay = apt->enterDate.day;
	bToWrite |= (enterDay >> 1); //finished first byte of apt data

	(*tbToWrite) = enterDay;
	(*tbToWrite) = ((*tbToWrite) << 15);
	enterMonth = apt->enterDate.month;
	(*tbToWrite) |= (enterMonth << 11);
	enterYear = apt->enterDate.year - 2000;
	(*tbToWrite) |= (enterYear << 4); //finished second and third bytes of apt data

	return bToWrite;
}

TWOBYTES getTwoByteData(Date aptRegDate)
{	//This function receives an apartment's database registry date and returns in the needed 2-byte
	//form in order to save it in the binary file.

	TWOBYTES tbToWrite, regDay, regMonth, regYear;

	regDay = aptRegDate.day;
	regMonth = aptRegDate.month;
	regYear = aptRegDate.year - 2000;

	tbToWrite = (regDay << 11);
	tbToWrite |= (regMonth << 7);
	tbToWrite |= regYear;

	return tbToWrite;
}

Date getDateFromTwoBytes(TWOBYTES dateBytes)
{	//This function receives two bytes that indicate a date in the following format:
	//first 5 bits indicate the day, next 4 bits indicate the month and last 7 bits indicate the year.
	//The function creates and returns a Date structure from the input bits.

	Date res;
	unsigned short int day, month = (dateBytes << 5), year = (dateBytes << 9);

	day = (dateBytes >> 11);
	month = (month >> 12);
	year = (year >> 9);
	year += 2000;

	res.day = day;
	res.month = month;
	res.year = year;

	return res;
}

long int fileSize(FILE* file)
{	//This function receives a pointer to a binary file and returns the file's size in bytes.
	long int res;

	fseek(file, 0, SEEK_END);
	res = ftell(file);
	fseek(file, 0, SEEK_SET);

	return res;
}