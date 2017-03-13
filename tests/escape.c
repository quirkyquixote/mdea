/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include <stdio.h>
#include <unistd.h>

#include "mdea.h"

int main(int argc, char *argv[])
{
	char line[1024];
	char *escaped;
	char *error;

	if (argc > 1) {
		freopen(argv[1], "r", stdin);
	}

	while (fgets(line, sizeof(line), stdin) != NULL) {
		size_t len = strlen(line);
		escaped = mdea_escape(line, &error);
		if (escaped == NULL) {
			fprintf(stdout, "ERROR: %s\n", error);
			free(error);
		} else {
			write(STDOUT_FILENO, escaped, strlen(escaped));
			free(escaped);
		}
	}
}
