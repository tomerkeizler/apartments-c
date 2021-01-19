/*
aptHeader.h

This file contains the prototypes of the functions concerning the apartments.
The functions' implementations are located in the file "aptSol.c".
*/

#ifndef apts_settings
#define apts_settings


#define _CRT_SECURE_NO_WARNINGS
#define TRUE 1
#define FALSE 0
#define NO_CODE -1

typedef unsigned char BYTE;
typedef unsigned short int TWOBYTES;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// --------------------------- STRUCTS ---------------------------

typedef struct date {
	short int day;
	short int month;
	short int year;
} Date;

typedef struct apartment {
	int code;
	char* address;
	int price;
	short int numRooms;
	Date enterDate; // date of entering the apartment
	Date regDate; // date of registering the apartment in the pool
} Apartment;

typedef struct aptListNode {
	struct aptListNode* next;
	struct aptListNode* prev;
	Apartment* apt;
} AptListNode;

typedef struct aptList {
	AptListNode* head;
	AptListNode* tail;
	int lastCode;
} AptList;

#endif apts_settings


// --------------------------- FUNCTIONS ---------------------------

void buyApt(AptList* aptList, int aptCode);
void deleteApt(AptList* aptList, int numOfDays);

AptList* createEmptyAptList();
int getAptListSize(AptList* aptList);
void freeAptList(AptList* aptList);
void printAptListFromHead(AptList* filteredAptList);
void printAptListFromTail(AptList* filteredAptList);
void insertAptToTail(AptList* aptList, Apartment* apt, AptListNode* prev);
AptList* createAptListCopy(AptList* aptList);
AptListNode* createNewAptListNode(AptListNode* next, AptListNode* prev);
Apartment* createNewApt(int code, char* address, int price, int numRooms, Date enterDate, Date regDate);
void removeAptListNode(AptList* aptList, AptListNode* nodeToRemove);
void addApt(AptList* aptList, char* aptInfo);
void findAptLocation(AptList* aptList, int aptPrice, AptListNode** nextApt, AptListNode** prevApt);
int isEmptyList(AptList* aptList);
void addAptToList(AptList* aptList, AptListNode* nextApt, AptListNode* prevApt, char* address,
	int price, int numRooms, Date enterDate, Date regDate, int aptCode);

Date makeDate(short int day, short int month, short int year);
Date getCurrTime();
short int getMonthFromStr(char* month);
time_t dateToTimeT(int day, int month, int year);

void printLatestApts(AptList* aptList, int numOfDays);
void getApt(AptList* fullAptList, char* filters);
int* getLatestAptCodes(AptList* aptList, int numOfDays, int* size);
void getAptByMaxRooms(AptList* filteredAptList, int maxRooms);
void getAptByMinRooms(AptList* filteredAptList, int minRooms);
void getAptByMaxPrice(AptList* filteredAptList, int maxPrice);
void getAptByDate(AptList* filteredAptList, Date maxDate);

void saveApts(AptList* aptList);
void loadApts(AptList* aptList);
long int fileSize(FILE* file);
Date getDateFromTwoBytes(TWOBYTES dateBytes);
BYTE getByteData(Apartment* apt, TWOBYTES* tbToWrite);
TWOBYTES getTwoByteData(Date aptRegDate);