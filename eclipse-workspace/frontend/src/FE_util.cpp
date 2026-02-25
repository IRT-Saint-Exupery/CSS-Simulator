#include <iostream>
#include <cstring>
#include "FE_util.h"
#include "FE_error.h"


std::vector<int> vectorFromStringOfIds (const char* cStringOfIds)
{
	std::vector<int> idVector;
	const char *ptr = cStringOfIds;

	int n, id;
	while (sscanf (ptr, "%d %n", &id, &n) == 1) {
		idVector.push_back (id);
		ptr += n;
	}

	return idVector;
}

bool lineToIgnore (const char* line)
{
	char spaces[512];
	char commentMark[3]="//";
	return ((strcmp ("\n", line) == 0) || 	// uniquement un \n
			(strcmp ("\r\n", line) == 0) || 	// uniquement un \r\n
			(sscanf (line, "%[ \t]\n", spaces)) || // This is a space or tab line
			(sscanf (line, "%2c", commentMark) && strcmp (commentMark, "//") == 0) || // This is a comment line
			(sscanf (line, "%[ \t]%2c", spaces, commentMark) && strcmp (commentMark, "//") == 0)); // This is a comment line
}
