#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "jsonParser.h"

node *newNode(node *parent, el_type type, char* key, int depth, char* content, int size)
{
	node *newNode = malloc(sizeof(node));
	newNode->next = NULL;
	newNode->child = NULL;
	newNode->parent = parent;
	newNode->children = 0;
	if (key != NULL)
	{
		newNode->key = (char*)malloc(sizeof(char) * size);
		strcpy(newNode->key, key);
	}
	else newNode->key = NULL;
	if (content != NULL)
	{
		newNode->content = (char*)malloc(sizeof(char) * size);
		strcpy(newNode->content, content);
	}
	else newNode->content = NULL;
	newNode->depth = depth;
	newNode->type = type;
	return newNode;
}

node *addSibling(node *n, el_type type, char *key, char *content, int size)
{
	if (n == NULL)
		return NULL;

	while (n->next)
		n = n->next;

	return (n->next = newNode(n->parent, type, key, n->depth, content, size));
}

node *addChild(node *n, el_type type, char *key, char *content, int size)
{
	if (n == NULL)
	{
		return NULL;
	}
	n->children++;
	if (n->child)
		return addSibling(n->child, type, key, content, size);
	else
	{
		return (n->child = newNode(n, type, key, n->depth + 1, content, size));
	}
}

void printElementContent(node *cNode, char **answer, int rootLevel)
{
	if (cNode->content != NULL)
	{
		if (cNode->key != NULL && cNode->parent->type == DICT && rootLevel == 0)
			sprintf(*answer, "%s%s:%s", *answer, cNode->key, cNode->content);
		else sprintf(*answer, "%s%s", *answer, cNode->content);
	}
	else {
		if (cNode->key != NULL && cNode->parent->type == DICT && rootLevel == 0)
			sprintf(*answer, "%s%s:", *answer, cNode->key);
		if (cNode->type == LIST)
			sprintf(*answer, "%s[", *answer);
		else if (cNode->type == DICT)
			sprintf(*answer, "%s{", *answer);
	}
}
void printEnding(node *cNode, char **answer)
{
	if (cNode->type == LIST)
		sprintf(*answer, "%s]", *answer);
	else if (cNode->type == DICT)
		sprintf(*answer, "%s}", *answer);
}
void printNode(node *cNode, char **answer)
{
	printElementContent(cNode, answer, 0);

	if (cNode->child != NULL)
		printNode(cNode->child, answer);

	printEnding(cNode, answer);

	if (cNode->next != NULL)
	{
		sprintf(*answer, "%s,", *answer);
		printNode(cNode->next, answer);
	}
}

void freeNode(node *cNode)
{
	if (cNode->next != NULL)
		freeNode(cNode->next);

	if (cNode->children > 0 && cNode->child != NULL)
		freeNode(cNode->child);

	if (cNode->key != NULL)
		free(cNode->key);

	if (cNode->content != NULL)
		free(cNode->content);

	free(cNode);
}

int getJSON(char *jsonPath, char **jsonString, int *size)
{
	FILE *jsonFile;
	jsonFile = fopen(jsonPath, "r");
	if (jsonFile == NULL)
		return 1;
	fseek(jsonFile, 0, SEEK_END);
	*size = ftell(jsonFile);
	*jsonString = realloc(*jsonString, sizeof(char) * (*size));
	strcpy(*jsonString, "");
	rewind(jsonFile);
	fread(*jsonString, sizeof(char), *size, jsonFile);
	fclose(jsonFile);
	return 0;
}
char **parseRequest(char *request, char **requestedParts)
{
	char *resultString = request;
	resultString = strtok(resultString, "[]");
	for (int i = 0; resultString != NULL; i++)
	{
		requestedParts[i] = resultString;
		resultString = strtok(NULL, "[]");
	}
	return requestedParts;
}

int countParts(char *request)
{
	int countL = 0, countR = 0, i = 0;
	while (request[i] != '\0')
	{
		if (request[i] == '[')
			countL++;
		else if (request[i] == ']')
			countR++;
		i++;
	}
	if (countL != countR)
		return -1;
	return countL;
}
void parseTree(node *node, char **answer)
{
	printElementContent(node, answer, 1);
	if (node->child != NULL)
		printNode(node->child, answer);
	printEnding(node, answer);
}
int parseJSON(char *jsonPath, node **destinationTree, int *size)
{
	int status = 1;
	char *json = (char*)malloc(sizeof(char));
	if (!getJSON(jsonPath, &json, size))
	{
		status = buildTree(json, destinationTree, *size);
	}
	free(json);
	return status;
}
int findNode(node *root, node **destination, char *request, int size)
{
	char **requestedParts;
	int requestedPartsCount;
	*destination = root;
	if (strcmp(request, "*") == 0)
		return 0;
	requestedPartsCount = countParts(request);
	if (requestedPartsCount < 1)
		return 1;
	requestedParts = (char**)malloc(sizeof(char) * size * requestedPartsCount);
	requestedParts = parseRequest(request, requestedParts);
	for (int partIt = 0; partIt < requestedPartsCount; partIt++)
	{
		*destination = (*destination)->child;
		while (strcmp((*destination)->key, requestedParts[partIt]) != 0)
		{
			*destination = (*destination)->next;
			if (*destination == NULL)
			{
				free(requestedParts);
				return 1;
			}
		}
	}
	free(requestedParts);
	return 0;
}

void processStartingType(char currentChar, el_type *type, int *waitingKey)
{
	switch (currentChar) {
	case '[':
		*type = LIST;
		break;
	case '{':
		*type = DICT;
		*waitingKey = 1;
		break;
	};
}

void defaultSymbol(char currentChar, char *currentElement, char *key, int *elementIt, int *keyIt, int waitingKey) {
	if (waitingKey == 0)
	{
		currentElement[*elementIt] = currentChar;
		*elementIt+=1;
		currentElement[*elementIt] = '\0';
	}
	else {
		key[*keyIt] = currentChar;
		key[*keyIt + 1] = '\0';
		*keyIt+=1;
	}
}

void identifyElement(char currentChar, node **currentNode, el_type *type, char **key, char **currentElement, 
					 int *elementIt, int *keyIt, int *waitingKey, int *collectingValue, int size)
{
	if (*collectingValue == 1 && currentChar != '"')
	{
		defaultSymbol(currentChar, *currentElement, *key, elementIt, keyIt, *waitingKey);
	}
	else if (isspace(currentChar) == 0) {
		switch (currentChar) {
		case '[':
			if (*type == LIST)
				sprintf(*key, "%d", (*currentNode)->children);
			*type = LIST;
			*currentNode = addChild(*currentNode, *type, *key, NULL, size);
			strcpy(*key, "");
			strcpy(*currentElement, "");
			break;
		case '{':
			*waitingKey = 1;
			if (*type == LIST)
				sprintf(*key, "%d", (*currentNode)->children);
			*type = DICT;
			*currentNode = addChild(*currentNode, *type, *key, NULL, size);
			strcpy(*key, "");
			strcpy(*currentElement, "");
			break;
		case '}':
			if (*elementIt > 0)
			{
				addChild(*currentNode, VALUE, *key, *currentElement, size);
				strcpy(*key, "");
				strcpy(*currentElement, "");
				*elementIt = 0;
			}
			if ((*currentNode)->parent != NULL)
				*currentNode = (*currentNode)->parent;
			*type = (*currentNode)->type;
			if (*currentNode != NULL && *type == DICT)
				*waitingKey = 1;
			else *waitingKey = 0;
			break;
		case ']':
			if (*elementIt > 0)
			{
				sprintf(*key, "%d", (*currentNode)->children);
				addChild(*currentNode, VALUE, *key, *currentElement, size);
				strcpy(*key, "");
				strcpy(*currentElement, "");
				*elementIt = 0;
			}
			if ((*currentNode)->parent != NULL)
				*currentNode = (*currentNode)->parent;
			*type = (*currentNode)->type;
			if (*currentNode != NULL && *type == DICT)
				*waitingKey = 1;
			else *waitingKey = 0;
			break;
		case ',':
			if ((*currentNode)->type == LIST)
			{
				sprintf(*key, "%d", (*currentNode)->children);
			}
			else {
				*waitingKey = 1;
			}
			if (*elementIt > 0)
				addChild(*currentNode, VALUE, *key, *currentElement, size);
			strcpy(*key, "");
			strcpy(*currentElement, "");
			*elementIt = 0;
			break;
		case ':':
			if (*waitingKey == 1)
			{
				*keyIt = 0;
				*waitingKey = 0;
			}
			else {
				defaultSymbol(currentChar, *currentElement, *key, elementIt, keyIt, *waitingKey);
			}
			break;
		case '"':
			defaultSymbol(currentChar, *currentElement, *key, elementIt, keyIt, *waitingKey);
			if (*collectingValue == 0)
				*collectingValue = 1;
			else *collectingValue = 0;
			break;
		default:
			defaultSymbol(currentChar, *currentElement, *key, elementIt, keyIt, *waitingKey);
		}
	}
}

int buildTree(char *jsonVALUE, node **destinationTree, int size) {
	node *root;
	el_type type;
	char *key = (char*)malloc(sizeof(char)*size);
	char *currentElement = (char*)malloc(sizeof(char)*size);
	key[0] = '\0';
	currentElement[0] = '\0';
	char currentChar;
	int elementIt = 0;
	int keyIt = 0;
	int waitingKey = 0;
	int collectingValue = 0;
	currentChar = jsonVALUE[0];
	processStartingType(currentChar, &type, &waitingKey);
	*destinationTree = newNode(NULL, type, NULL, 0, NULL, size);
	root = *destinationTree;
	for (int charIt = 1; jsonVALUE[charIt] > -1 && jsonVALUE[charIt] <= 255; charIt++)
	{
		currentChar = jsonVALUE[charIt];
		identifyElement(currentChar, &root, &type, &key, &currentElement, &elementIt, &keyIt, &waitingKey, &collectingValue, size);
	}
	free(key);
	free(currentElement);
	return 0;
}