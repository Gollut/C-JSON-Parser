#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "jsonParser.h"

int main(int argc, char **argv) {
	node* jsonTree;
	node* answerTree;
	int size;
	int jsonStatus, answerStatus;
	char jsonPath[1000];
	if (argc >= 3 && !strcmp(argv[1], "--input")) {
		strcpy(jsonPath, argv[2]);
	}
	else {
		printf("Using default input.json sample.\n");
		strcpy(jsonPath, "input.json");
	}
	jsonStatus = parseJSON(jsonPath, &jsonTree, &size);
	if (jsonStatus)
	{
		printf("File was not found");
	}
	else {
		while (1)
		{
			char* answer = (char*)malloc(sizeof(char) * size);
			char* request = (char*)malloc(sizeof(char) * size);
			strcpy(answer, "");
			scanf("%s", request);
			answerStatus = findNode(jsonTree, &answerTree, request, size);
			if (answerStatus)
				printf("Not found");
			else {
				parseTree(answerTree, &answer);
				printf("%s\n", answer);
			}
			free(answer);
			free(request);
		}
		freeNode(jsonTree);
	}
	return 0;
}