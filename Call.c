#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "JSONparser.h"
int main() {

	char request[256];
	node* json;
	node* answerTree;
	char *answer;
	char jsonPath[] = "input.json";
	scanf("%s", request);

	json = parseJSON(jsonPath);
	answerTree = findNode(json, request);
	answer = parseTree(answerTree);
	printf("%s", answer);

	freeNode(json);
	free(answer);
	scanf("%s", &request);
	return 0;
}