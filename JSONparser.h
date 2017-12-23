#ifndef PARSER_H
#define PARSER_H
typedef enum {
	DICT = 1,
	LIST = 2,
	VALUE = 3
} el_type;

typedef struct node{
	struct node *next;
	struct node *child;
	struct node *parent;
	char *key;
	char *content;
	int depth;
	int children;
	el_type type;
} node;

node *newNode(node *, el_type, char *, int, char *, int);
node *addSibling(node *, el_type, char *, char *, int);
node *addChild(node *, el_type, char *, char *, int);
int findNode(node *, node **, char *, int);
int parseJSON(char *, node **, int *);
int buildTree(char *, node **, int);
int countParts(char *);
int getJSON(char *, char **, int *);
char **parseRequest(char *, char **);
void printElementContent(node *, char **, int);
void printEnding(node *, char **);
void printNode(node *, char **);
void freeNode(node *);
void processStartingType(char, el_type *, int *);
void defaultSymbol(char, char *, char *, int*, int*, int);
void identifyElement(char, node **, el_type *, char **, char **, int *, int *, int *, int *, int);
void parseTree(node *, char **);
#endif