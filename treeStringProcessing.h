#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

const ELEMENT_SIZE = 64000;
const KEY_SIZE = 512;

typedef enum {
	DICT = 1,
	LIST = 2,
	VALUE = 3
} el_type;

struct node {
	struct node *next;
	struct node *child;
	struct node *parent;
	char *key;
	char *content;
	int depth;
	int children;
	el_type type;
};

typedef struct node node;

node * newNode(node *parent, el_type type, char* key, int depth, char* content)
{
	node *newNode = malloc(sizeof(node));
	newNode->next = NULL;
	newNode->child = NULL;
	newNode->parent = parent;
	newNode->children = 0;
	if (key != NULL)
	{
		newNode->key = (char*)calloc(1, sizeof(char) * KEY_SIZE);
		strcpy(newNode->key, key);
	}
	else newNode->key = NULL;
	if (content != NULL)
	{
		newNode->content = (char*)calloc(1, sizeof(char) * ELEMENT_SIZE);
		strcpy(newNode->content, content);
	}
	else newNode->content = NULL;
	newNode->depth = depth;
	newNode->type = type;
	return newNode;
}

node * addSibling(node * n, el_type type, char* key, char* content)
{
	if (n == NULL)
		return NULL;

	while (n->next)
		n = n->next;

	return (n->next = newNode(n->parent, type, key, n->depth, content));
}

node * addChild(node * n, el_type type, char* key, char* content)
{
	if (n == NULL)
	{
		return NULL;
	}
	n->children++;
	if (n->child)
		return addSibling(n->child, type, key, content);
	else
	{
		return (n->child = newNode(n, type, key, n->depth + 1, content));
	}
}

el_type type;
node *currentNode;
char* answer;
char* key;
char* currentElement;
char currentChar;
int elementIt = 0;
int keyIt = 0;
int waitingKey = 0;
int collectingVALUE = 0;

void printElementContent(node* cNode, char* answer, int rootLevel)
{
	if (cNode->content != NULL)
	{
		if (cNode->key != NULL && cNode->parent->type == DICT && rootLevel == 0)
			sprintf(answer, "%s%s:%s", answer, cNode->key, cNode->content);
		else sprintf(answer, "%s%s", answer, cNode->content);
	}
	else {
		if (cNode->key != NULL && cNode->parent->type == DICT && rootLevel == 0)
			sprintf(answer, "%s%s:", answer, cNode->key);
		if (cNode->type == LIST)
			sprintf(answer, "%s[", answer);
		else if (cNode->type == DICT)
			sprintf(answer, "%s{", answer);
	}
}
void printEnding(node* cNode, char* answer)
{
	if (cNode->type == LIST)
		sprintf(answer, "%s]", answer);
	else if (cNode->type == DICT)
		sprintf(answer, "%s}", answer);
}
void printNode(node* cNode, char* answer)
{
	printElementContent(cNode, answer, 0);

	if (cNode->child != NULL)
		printNode(cNode->child, answer);

	printEnding(cNode, answer);

	if (cNode->next != NULL)
	{
		sprintf(answer, "%s,", answer);
		printNode(cNode->next, answer);
	}
}

void freeNode(node* cNode)
{
	if (cNode->next != NULL)
		freeNode(cNode->next);

	if (cNode->child != NULL)
		freeNode(cNode->child);

	free(cNode);
}

void processStartingType()
{
	switch (currentChar) {
	case '[':
		type = LIST;
		break;
	case '{':
		type = DICT;
		waitingKey = 1;
		break;
	};
}

void defaultSymbol() {
	if (waitingKey == 0)
	{
		currentElement[elementIt] = currentChar;
		elementIt += 1;
	}
	else{
		key[keyIt] = currentChar;
		keyIt += 1;
	}
}

void identifyElement()
{
	if (collectingVALUE == 1 && currentChar != '"')
	{
		defaultSymbol();
	}
	else if (isspace(currentChar) == 0) {
		switch (currentChar) {
		case '[':
			if (currentNode->type == LIST)
				sprintf(key, "%d", currentNode->children);
			currentNode = addChild(currentNode, LIST, key, NULL);
			free(key);
			key = (char*)calloc(1, sizeof(char) * KEY_SIZE);
			break;
		case '{':
			waitingKey = 1;
			if (currentNode->type == LIST)
				sprintf(key, "%d", currentNode->children);
			currentNode = addChild(currentNode, DICT, key, NULL);
			free(key);
			free(currentElement);
			currentElement = (char*)calloc(1, sizeof(char) * ELEMENT_SIZE);
			key = (char*)calloc(1, sizeof(char) * KEY_SIZE);
			break;
		case '}':
			if (elementIt > 0)
			{
				addChild(currentNode, VALUE, key, currentElement);
				free(key);
				free(currentElement);
				currentElement = (char*)calloc(1, sizeof(char) * ELEMENT_SIZE);
				key = (char*)calloc(1, sizeof(char) * KEY_SIZE);
				elementIt = 0;
			}
			currentNode = currentNode->parent;
			if (currentNode != NULL && currentNode->type == DICT)
				waitingKey = 1;
			else waitingKey = 0;
			break;
		case ']':
			if (elementIt > 0)
			{
				sprintf(key, "%d", currentNode->children);
				addChild(currentNode, VALUE, key, currentElement);
				free(key);
				free(currentElement);
				currentElement = (char*)calloc(1, sizeof(char) * ELEMENT_SIZE);
				key = (char*)calloc(1, sizeof(char) * KEY_SIZE);
				elementIt = 0;
			}
			currentNode = currentNode->parent;
			if (currentNode != NULL && currentNode->type == DICT)
				waitingKey = 1;
			else waitingKey = 0;
			break;
		case ',':
			if (currentNode->type == LIST)
			{
				sprintf(key, "%d", currentNode->children);
			}
			else {
				waitingKey = 1;
			}
			if (elementIt > 0)
				addChild(currentNode, VALUE, key, currentElement);
			free(key);
			free(currentElement);
			key = (char*)calloc(1, sizeof(char) * KEY_SIZE);
			currentElement = (char*)calloc(1, sizeof(char) * ELEMENT_SIZE);
			elementIt = 0;
			break;
		case ':':
			if (waitingKey == 1)
			{
				keyIt = 0;
				waitingKey = 0;
			}
			else {
				defaultSymbol();
			}
			break;
		case '"':
			defaultSymbol();
			if (collectingVALUE == 0)
				collectingVALUE = 1;
			else collectingVALUE = 0;
			break;
		default:
			defaultSymbol();
		}
	}
}

node *buildTree(char *jsonVALUE) {

	key = (char*)calloc(1, sizeof(char) * KEY_SIZE);
	currentElement = (char*)calloc(1, sizeof(char) * ELEMENT_SIZE);
	currentChar = jsonVALUE[0];

	processStartingType();
	currentNode = newNode(NULL, type, NULL, 0, NULL);
	node *rootBuffer;
	rootBuffer = currentNode;
	for (size_t charIt = 1; charIt < strlen(jsonVALUE); charIt++)
	{
		currentChar = jsonVALUE[charIt];
		identifyElement(currentChar);
	}
	free(key);
	free(currentElement);
	return rootBuffer;
}