/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include <stdio.h>

#include "mdea.h"

int main(int argc, char *argv[])
{
	wchar_t *error;
	struct mdea_parser *t = mdea_file_parser(stdin);
	struct mdea_emitter *e = mdea_file_emitter(stdout);
	if (mdea_parse(t, e, &error) != 0)
		fwprintf(stderr, L"ERROR: %ls\n", error);
	mdea_parser_destroy(t);
	mdea_emitter_destroy(e);
}

