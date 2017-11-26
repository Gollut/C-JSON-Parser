#include "stringProcessing.h"
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
char *parseJSON(char *request, char *jsonPath)
{
	char *jsonString;
	char **requestedParts;
	int requestedPartsCount;
	requestedPartsCount = countParts(request);
	if (requestedPartsCount == -1)
		return "Syntax error";
	requestedParts = parseRequest(request);
	jsonString = getJSON(jsonPath);
	return parseString(jsonString, requestedParts, requestedPartsCount);
}