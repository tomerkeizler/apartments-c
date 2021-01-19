/*
Commands header
*/

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 7
#define NUM_OF_CMDS 10

#include "aptHeader.h"


// --------------------------- STRUCTS ---------------------------

typedef struct lastComQueue {
	char* short_term_history[N];
	int front;
	int back;
	int size;
} LastComQueue;

typedef struct comListNode {
	struct comListNode* next;
	struct comListNode* prev;
	char* com;
} ComListNode;

typedef struct oldComList {
	ComListNode* head;
	ComListNode* tail;
	int size;
} OldComList;


// --------------------------- FUNCTIONS ---------------------------


////// text file functions
void loadCommands(OldComList* comList, LastComQueue* comQueue);
char* getLine(FILE* f);
void saveCommands(OldComList* comList, LastComQueue* comQueue);


////// general functions
void executeCom(AptList* aptList, OldComList* comList, LastComQueue* comQueue, char* com);
void saveCom(OldComList* comList, LastComQueue* comQueue, char* comToSave);
void printCom(int num, char* com);
char* getComByNum(OldComList* comList, LastComQueue* comQueue, int num);
char* replaceCom(const char *cmd, const char *pattern, const char *replace);
char* duplicateStr(char* str);
char* getLineInput();


////// restore commands functions
void execLastCom(AptList* aptList, OldComList* comList, LastComQueue* comQueue);
void execComByNum(AptList* aptList, OldComList* comList, LastComQueue* comQueue, int num);
void printShortHistory(int sizeOldComs, LastComQueue* comQueue);
void PrintAllHistory(OldComList* comList, LastComQueue* comQueue);
void execComStrReplace(AptList* aptList, OldComList* comList, LastComQueue* comQueue, int num, char* str1, char* str2);


////// Commands queue fucntions
LastComQueue* makeEmptyQueue();
void freeQueue(LastComQueue* comQueue);
void enqueueCommand(LastComQueue* comQueue, char* comToAdd);
char* dequeueCommand(LastComQueue* comQueue);
void moveQueueForward(LastComQueue* comQueue);
int isFullQueue(LastComQueue* comQueue);
int isEmptyQueue(LastComQueue* comQueue);
char* getQueueComByNum(LastComQueue* comQueue, int num);
char* getLastQueueCom(LastComQueue* comQueue);


////// Commands list fucntions
OldComList* makeEmptyComList();
void freeComList(OldComList* comList);
void insertCommandToEndList(OldComList* comList, char* com);
int isEmptyComList(OldComList comList);
char* getListComByNum(OldComList* comList, int num);

