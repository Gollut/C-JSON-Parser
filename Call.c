#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "JSONparser.h"
int main() {
	char request[256];
	char *answer;
	char jsonPath[] = "input.json";
	scanf("%s", request);
	answer = parseJSON(request, jsonPath);
	printf("%s", answer);
	scanf("%s", &request);
	return 0;
}