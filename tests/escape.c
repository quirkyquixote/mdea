/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "../mdea.h"

int main(int argc, char *argv[])
{
	wchar_t line[1024];
	wchar_t escaped[1024];
	wchar_t *error;

	while (fgetws(line, sizeof(line), stdin) != NULL) {
		size_t len = wcslen(line);
		if (line[len - 1] == '\n') {
			line[len - 1] = 0;
			--len;
		}
		fwprintf(stdout, L"%ls\n", line);
		if (mdea_escape(escaped, sizeof(escaped), line, wcslen(line), &error) < 0) {
			fwprintf(stdout, L"ERROR: %ls\n", error);
			free(error);
		} else {
			fwprintf(stdout, L"%ls\n", escaped);
		}
	}
}
