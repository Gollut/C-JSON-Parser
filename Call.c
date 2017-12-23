#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include "jsonParser.h"
int main() {
	node* jsonTree;
	node* answerTree;
	int size;
	int jsonStatus, answerStatus;
	char jsonPath[1000];
	scanf("%s", jsonPath);
	jsonStatus = parseJSON(jsonPath, &jsonTree, &size);
	if (jsonStatus)
	{
		printf("File was not found");
	}
	else {
		char* answer = (char*)malloc(sizeof(char) * size);
		char* request = (char*)malloc(sizeof(char) * size);
		strcpy(answer, "");
		scanf("%s", request);
		answerStatus = findNode(jsonTree, &answerTree, request, size);
		if (answerStatus)
			printf("Not found");
		else {
			parseTree(answerTree, &answer);
			printf("%s", answer);
		}
		freeNode(jsonTree);
		free(answer);
		free(request);
	}
	scanf("%s", jsonPath);
	return 0;
}