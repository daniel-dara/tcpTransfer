#include "globals.h"

// Converts a string to uppercase
char* toUpper(char* str) {
	char* s = str;

	do
	{
		*s = toupper(*s);
	}
	while (*s++);

	return str;
}

void dieWithUserMessage(const char *msg, const char *detail)
{
	fprintf(stderr, "%s: %s\n", msg, detail);
	exit(1);
}

void dieWithSystemMessage(const char *msg)
{
	perror(msg);
	exit(2);
}
