#include "treeStringProcessing.h"

char *getJSON(char *jsonPath)
{
	long size;
	char *jsonString;
	FILE *jsonFile;
	jsonFile = fopen(jsonPath, "r");
	fseek(jsonFile, 0, SEEK_END);
	size = ftell(jsonFile);
	rewind(jsonFile);
	jsonString = (char*)calloc(1, size + 1);
	fread(jsonString, sizeof(char), size, jsonFile);
	fclose(jsonFile);
	return jsonString;
}
char **parseRequest(char *request)
{
	char *requestedParts[1000];
	char *resultString = request;
	int i = 0;
	resultString = strtok(resultString, "[]");
	for (i = 0; resultString != NULL; i++)
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
node *parseJSON(char *jsonPath)
{
	return buildTree(getJSON(jsonPath));
}
node *findNode(node* root, char* request)
{
	char **requestedParts;
	int requestedPartsCount;
	if (request == "*")
		return root;
	requestedPartsCount = countParts(request);
	if (requestedPartsCount == -1)
		return NULL;
	requestedParts = parseRequest(request);
	currentNode = root;
	for (int partIt = 0; partIt < requestedPartsCount; partIt++)
	{
		currentNode = currentNode->child;
		while (strcmp(currentNode->key, requestedParts[partIt]) != 0)
		{
			currentNode = currentNode->next;
			if (currentNode == NULL)
				return NULL;
		}
	}
	return currentNode;
}
char *parseTree(node* node)
{
	if (node != NULL)
	{
		answer = (char*)calloc(1, sizeof(char) * ELEMENT_SIZE);
		printElementContent(node, answer, 1);
		if (node->child != NULL)
			printNode(node->child, answer);
		printEnding(node, answer);
		return answer;
	}
	else return NULL;
}