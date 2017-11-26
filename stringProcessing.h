#include <stdlib.h>
#include <string.h>
int const DEBUG = 1;
char *answer;
char *key;
char *searchID;
char currentChar;

// ITERATORS AND COUNTERS

// Iterator for key string
int keyIt = 0;

// Local counter of request parts
int partsCount = 0;

// Iterator for json processing
int charIt = 0;

// ID of element in list
int inListID = 0;

// Level of current searching branch
int level = 0;

// Current level
int currentLevel = 0;

// Iterator of answer string
int answerIt = 0;

// VARIABLES-CHECKERS

// Answer ready : stop working, return answer
int answerReady = 0;

// Wrong branch : skip the content
int skip = 0;

// Leaf or branch level: get all from the content
int lastLevel = 0;

// Searching the key state
int waitingKey = 0;

// Check if content is part of a string inside key
int processingString = 0;

// If dict key was accepted
int keyAccepted = 0;

// 0 - list, 1 - dict on each stage
int type[500];

// METHODS

// Key started to be read, process each char
void processKeyDict()
{
	// End of key
	if (currentChar == ':')
	{
		if (strcmp(key, searchID) == 0)
		{
			// Right key, accept
			keyAccepted = 1;
		
			if (level == partsCount)
				lastLevel = 1;
		}
		else {
			skip = 1;
		}
		free(key);
		key = (char*)calloc(1, sizeof(char) * 512);
		keyIt = 0;
		waitingKey = 0;
	}
	else {
		// Collecting key
		key[keyIt] = currentChar;
		keyIt++;
	}
}
void processStartOfList()
{
	currentLevel++;
	type[currentLevel] = 0;
	if (lastLevel == 1 || keyAccepted == 1 && level == partsCount - 1)
	{
		answer[answerIt] = currentChar;
		answerIt++;
	}
	else if (skip == 0)
	{
		if (type[currentLevel - 1] == 0 && inListID == atoi(searchID) || type[currentLevel - 1] == 1)
		{
			level++;
			if (level == partsCount)
			{
				lastLevel = 1;
				answer[answerIt] = currentChar;
				answerIt++;
			}
			inListID = 0;
			keyAccepted = 0;
		}
		else {
			skip = 1;
		}
	}
}
void processEndOfList()
{
	currentLevel--;
	if (lastLevel == 1 || keyAccepted == 1)
	{
		answer[answerIt] = currentChar;
		answerIt++;
		if (lastLevel == 1 && currentLevel == level - 1 || keyAccepted == 1 && currentLevel == level)
		{
			lastLevel = 0;
			keyAccepted = 0;
		}
	}
	if (answerIt > 0 && currentLevel == level && lastLevel == 0 && keyAccepted == 0)
	{
		answerReady = 1;
	}
	if (currentLevel == level)
	{
		skip = 0;
	}
}
void processStartOfDict()
{
	currentLevel++;
	type[currentLevel] = 1;
	if (lastLevel == 1)
	{
		answer[answerIt] = currentChar;
		answerIt++;
	}
	else if (skip == 0)
	{
		if (keyAccepted == 1 || type[currentLevel - 1] == 0 && inListID == atoi(searchID))
		{
			keyAccepted = 0;
			level++;
			if (level == partsCount)
			{
				lastLevel = 1;
				answer[answerIt] = currentChar;
				answerIt++;
			}
			if (currentLevel == level)
			{
				waitingKey = 1;
				keyIt = 0;
			}
		}
		else skip = 1;
	}
}
void processEndOfDict()
{
	currentLevel--;
	if (lastLevel == 1)
	{
		answer[answerIt] = currentChar;
		answerIt++;
	}
	if (currentLevel == level - 1)
	{
		skip = 0;
		lastLevel = 0;
		keyAccepted = 0;
	}
	else if (answerIt > 0 && currentLevel == level && lastLevel == 0 && keyAccepted == 0)
	{
		answerReady = 1;
	}
	if (keyAccepted == 1)
	{
		answer[answerIt] = currentChar;
		answerIt++;
	}
}
void processNextElement()
{
	// Reset status of previous element
	if (keyAccepted == 1 && currentLevel == level || currentLevel == level - 1 && lastLevel == 1)
	{
		keyAccepted = 0;
		lastLevel = 0;
	}
	if (skip == 1 && currentLevel == level)
	{
		skip = 0;
	}
	if (type[currentLevel] == 0 && skip == 0 && lastLevel == 0)
	{
		inListID++;
	}
	else if (type[currentLevel] == 1 && skip == 0 && lastLevel == 0 && keyAccepted == 0 && currentLevel == level)
	{
		waitingKey = 1;
	}
	if (answerIt > 0 && currentLevel == level && lastLevel == 0)
		answerReady = 1;
	else if (lastLevel == 1 || keyAccepted == 1)
	{
		answer[answerIt] = currentChar;
		answerIt++;
	}
}
// Check zero level
void processStartingType()
{
	switch (currentChar) {
	case '[':
		type[currentLevel] = 0;
		break;
	case '{':
		type[currentLevel] = 1;
		waitingKey = 1;
		break;
	};
}

// Switch processingString
void processStringLimits()
{
	if (processingString == 1)
		processingString = 0;
	else processingString = 1;
}

// Check content char without utility value
void processDefaultChar()
{
	if (skip == 0 && answerReady == 0 &&
		(lastLevel == 1 || keyAccepted == 1 || (type[currentLevel] == 0 && currentLevel == level && inListID == atoi(searchID))))
	{
		answer[answerIt] = currentChar;
		answerIt++;
	}
}
void identifyElement()
{
	switch (currentChar) {
	case '[':
		processStartOfList();
		break;
	case '{':
		processStartOfDict();
		break;
	case '}':
		processEndOfDict();
		break;
	case ']':
		processEndOfList();
		break;
	case ',':
		processNextElement();
		break;
	case '"':
		processStringLimits();
	default:
		if (skip == 0)
		{
			processDefaultChar();
		}
		break;
	}
}
void processElement() {
	if (DEBUG == 1)
		printf("      %c      currentLevel: %d level: %d type: %d parentType: %d waitingKey: %d keyAccepted: %d lastLevel: %d skip: %d procStr: %d \n", 
		currentChar, currentLevel, level, type[currentLevel], type[currentLevel-1], waitingKey, keyAccepted, lastLevel, skip, processingString);
	// Check if the char is the part of a key and it is meaningful
	if (waitingKey == 1 && isspace(currentChar) == 0 && skip == 0 && lastLevel == 0)
	{
		processKeyDict();
	}
	// Check if the char is the part of a string and all utility chars should be ignored
	else if (currentChar != '"' && processingString == 1 && skip == 0)
		processDefaultChar();
	else if (isspace(currentChar) == 0) {
		identifyElement();
	}
}

char *parseString(char *jsonString, char **requestedParts, int requestedPartsCount) {
	answer = (char*)calloc(1, sizeof(char) * 2048);
	key = (char*)calloc(1, sizeof(char) * 512);
	partsCount = requestedPartsCount;
	currentChar = jsonString[0];

	//Process zero level
	processStartingType();

	// Processing json till answer is ready
	for (charIt = 1; charIt < strlen(jsonString) && level < requestedPartsCount || lastLevel == 1; charIt++)
	{
		searchID = requestedParts[level];
		currentChar = jsonString[charIt];
		processElement();
		if (answerReady == 1)
		{
			return answer;
		}
	}
	return answer;
}