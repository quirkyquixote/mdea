/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include <stdio.h>

#include "../mdea.h"

int main(int argc, char *argv[])
{
	struct mdea_node *root;
	wchar_t *error;

	if (argc > 1) {
		for (int i = 1; i < argc; ++i) {
			FILE *f = fopen(argv[i], "r");
			if (f == NULL) {
				perror(argv[i]);
			} else {
				if (mdea_read(f, &root, &error) != 0)
					fwprintf(stderr, L"ERROR: %ls\n", error);
				if (mdea_write(stdout, root, &error) != 0)
					fwprintf(stderr, L"ERROR: %ls\n", error);
				fclose(f);
			}
		}
	} else {
		if (mdea_read(stdin, &root, &error) != 0)
			fwprintf(stderr, L"ERROR: %ls\n", error);
		if (mdea_write(stdout, root, &error) != 0)
			fwprintf(stderr, L"ERROR: %ls\n", error);
	}
}

