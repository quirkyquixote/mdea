/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "../mdea.h"

int main(int argc, char *argv[])
{
	wchar_t line[1024];
	wchar_t *escaped;
	wchar_t *error;

	if (argc > 1) {
		freopen(argv[1], "r", stdin);
	}

	while (fgetws(line, sizeof(line), stdin) != NULL) {
		size_t len = wcslen(line);
		if (line[len - 1] == '\n') {
			line[len - 1] = 0;
			--len;
		}
		escaped = mdea_escape(line, &error);
		if (escaped == NULL) {
			fwprintf(stdout, L"ERROR: %ls\n", error);
			free(error);
		} else {
			fwprintf(stdout, L"%ls\n", escaped);
			free(escaped);
		}
	}
}
