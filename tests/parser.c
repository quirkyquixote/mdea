/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "../json.h"

int main(int argc, char *argv[])
{
	struct JsonNode *root;
	wchar_t *error;

	if (argc > 1) {
		for (int i = 1; i < argc; ++i) {
			FILE *f = fopen(argv[i], "r");
			if (f == NULL) {
				perror(argv[i]);
			} else {
				if (json_read(f, &root, &error) != 0)
					fwprintf(stderr, L"ERROR: %ls\n", error);
				if (json_write(stdout, root, 0, &error) != 0)
					fwprintf(stderr, L"ERROR: %ls\n", error);
				fclose(f);
			}
		}
	} else {
		if (json_read(stdin, &root, &error) != 0)
			fwprintf(stderr, L"ERROR: %ls\n", error);
		if (json_write(stdout, root, 0, &error) != 0)
			fwprintf(stderr, L"ERROR: %ls\n", error);
	}
}

