/*******************************************************************************
* File Name          : hashAssignmentV3.cpp
* Description        : Implemenation of a hash skelleton
*
* Author:              PROG8130 / ???
* Date:                Nov 24, 2021
******************************************************************************
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

unsigned int myHashFunction(char*);						// Implementation of hash algorithm
unsigned int putIntoHashTable(char* , unsigned int);    // function to add to hash table
unsigned int getFromHashTable(char* , unsigned int);	// function to retrieve data from hash table

#define INPUT_BUFFER_SIZE 200	// local buffer used for adding data to the hash table
#define HASH_SIZE 10			// size of hash table to be used

// data structure used to keep track of hashed data
typedef struct myHashStruct {
	char* ptrBuffer;                       // pointer to data stored in hash 
	struct myHashStruct* ptrNextHashData;  // pointer to next item in this hash bucket (or NULL if no more)
}HASH_ELEMENT;

HASH_ELEMENT* myHash[HASH_SIZE];           // create an empty hash table structure (note this is basically an arrary of linked list heads)

int main()
{
	char    inputBuffer[INPUT_BUFFER_SIZE];

	// Initialize hash table to empty one
	for (int i = 0; i < HASH_SIZE; i++)
	{
		if ((myHash[i] = (struct myHashStruct*)calloc(1, sizeof(struct myHashStruct))) == NULL)
		{
			printf("calloc failed!\n");
			return(-1);
		}
		else {
			myHash[i]->ptrBuffer = NULL;
			myHash[i]->ptrNextHashData = NULL;
		}
	}

	// add to hash table loop
	while (1)
	{
		printf("enter data to be added to hash table or exit when done\n");

		// get strings from the console and place in hash until nothing entered
		scanf_s("%199s", inputBuffer, INPUT_BUFFER_SIZE);
		inputBuffer[INPUT_BUFFER_SIZE - 1] = '\0';

		// stop adding data into hash table when "exit" is entered
		if (strcmp(inputBuffer, "exit") == 0)
			break;

		if (putIntoHashTable(inputBuffer, strlen(inputBuffer)) == HASH_SIZE)
			printf("Error putting into hash table\n");
	}

	// check if data is in hash table
	while (1)
	{
		unsigned int hashIndexNumber = 0;

		printf("Enter data to find, done when complete\n");

		// get strings from the console and check if in hash table
		scanf_s("%199s", inputBuffer, INPUT_BUFFER_SIZE);
		inputBuffer[INPUT_BUFFER_SIZE - 1] = '\0';

		// stop adding data into hash table when "done" is entered
		if (strcmp(inputBuffer, "done") == 0)
			break;

		if ((hashIndexNumber = getFromHashTable(inputBuffer, strlen(inputBuffer))) == HASH_SIZE)
			printf("%s not found in hash table\n", inputBuffer);
		else
			printf("%s found in hash table at %u\n", inputBuffer, hashIndexNumber);
	}

	return 0;
}

// FUNCTION      : myHashFunction
// DESCRIPTION   :
//   Calculate a hash value to use in storing the data into the hash table
// PARAMETERS    :
//   ptrInputBuffer - a pointer to the buffer to be put into the hash table
//
// RETURNS       :
//   Hash value calculated ( HASH_SIZE on failure)

unsigned int myHashFunction(char* ptrInputBuffer){

	unsigned long hash = 5381;
	int c;
	if (ptrInputBuffer != NULL) {
		// djb2 algorithm with base 33
		while (c = ((int)*ptrInputBuffer++))
			hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
		
		printf("Hash: %d \n", hash);

		return hash % HASH_SIZE;
	}
	else{
		printf("Hash function error!\n");
		return HASH_SIZE;
	}	
}

// FUNCTION      : putIntoHashTable
// DESCRIPTION   :
//   Put the supplied data into a hash table
// PARAMETERS    :
//   ptrInputBuffer - a pointer to the buffer to be put into the hash table
//   bufferLength   - how many characters are in the buffer being put into the hash table
//
// RETURNS       :
//   Hash value used ( HASH_SIZE on failure)

unsigned int putIntoHashTable(char* ptrInputData, unsigned int bufferLength)
{
	unsigned int inputHashValue = myHashFunction(ptrInputData);

	HASH_ELEMENT* current = myHash[inputHashValue];

	HASH_ELEMENT* newElement;
	char* newbuffer;

	// Head of linked list cant be null
	if (current == NULL) {
		printf("Wrong head to hash index received!\n");
		return HASH_SIZE;
	}

	// Iterate through all elements in hash index
	while (current != NULL) {
		// Check if value is already in hash table
		if (current->ptrBuffer != NULL) {
			if (strcmp(current->ptrBuffer, ptrInputData) == 0) {
				printf("Input already in hash table. \n");
				return HASH_SIZE;
			}
			else {
				// Element is not in table and must be added
				if (current->ptrNextHashData == NULL) {
					// Allocate space for new element (pointers)
					if ((newElement = (HASH_ELEMENT*)calloc(1, sizeof(HASH_ELEMENT))) == NULL)
					{
						printf("calloc failed to allocate new element!\n");
						return HASH_SIZE;
					}
					// Allocate space for char buffer
					if ((newbuffer = (char*)calloc(1, bufferLength+1)) == NULL)
					{
						printf("calloc failed to allocate new buffer!\n");
						return HASH_SIZE;
					}
					// Put data into the new element
					strcpy_s(newbuffer, bufferLength+1, ptrInputData);
					newElement->ptrBuffer = newbuffer;
					newElement->ptrNextHashData = NULL;
					// Chain it to linked list
					current->ptrNextHashData = newElement;
					printf("New element in index added succesfully\n");
					return inputHashValue;
				}
				else {
					// Iterate to next element in hash index
					current = current->ptrNextHashData;
				}
			}
		}
		
		// Element is already there but not the buffer
		else {
			// Allocate space for char buffer
			if ((newbuffer = (char*)calloc(1, bufferLength+1)) == NULL)
			{
				printf("calloc failed to allocate new buffer!\n");
				return HASH_SIZE;
			}
			// Put data into the new element
			printf("Input: %s \n Size: %d\n", ptrInputData, bufferLength);
			strcpy_s(newbuffer, bufferLength+1, ptrInputData);
			current->ptrBuffer = newbuffer;
			current->ptrNextHashData = NULL;
			printf("First element in index added succesfully.\n");
			return inputHashValue;
		}
	}	

	return inputHashValue;
}

// FUNCTION      : getFromHashTable
// DESCRIPTION   :
//   Read as much data as there is room for from the hash table
// PARAMETERS    :
//   ptrOutputBuffer - a pointer to the buffer to place the data read from hash table
//   bufferLength    - maxiumum number of characters that can be read
//
// RETURNS       :
//   Hash value used ( return HASH_SIZE value on failure)

unsigned int getFromHashTable(char* ptrOutputData, unsigned int bufferLength)
{
	unsigned int hashValue = myHashFunction(ptrOutputData);
	HASH_ELEMENT* current = myHash[hashValue];
	unsigned int chainCounter = 0;

	// Head of linked list cant be null
	if (current == NULL) {
		printf("Wrong head to hash index received!\n");
		return 0;
	}

	// Iterate through all elements in hash index
	while (current != NULL) {
		// Is the buffer of current element not empty?
		if (current->ptrBuffer != NULL) {
			if (strcmp(current->ptrBuffer, ptrOutputData) == 0) {
				printf("Element found in hash table: \n");
				printf("Hash: %d, Chain #: %d.\n", hashValue, chainCounter);
				return hashValue;
			}
			else {
				// The buffer of the current element is different than the one searched
				chainCounter++;
				current = current->ptrNextHashData;
			}
		}
		else {
			// The buffer of the current element is Empty
			printf("Hash index empty.\n");
			return HASH_SIZE;
		}
		
	}
	// Means the linked list does not contain the desired value
	printf("Element not in linked list."
			"Calculated hash: %d, Items searched in index: %d. \n", hashValue, chainCounter);

	return HASH_SIZE;
}
 
