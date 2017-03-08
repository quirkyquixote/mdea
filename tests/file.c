/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include <stdio.h>

#include "../mdea.h"
#include "../file_parser.h"
#include "../file_emitter.h"

int main(int argc, char *argv[])
{
	struct mdea_node *root;
	wchar_t *error;

	struct mdea_parser *t = mdea_file_parser(stdin);
	if (mdea_read(t, &root, &error) != 0)
		fwprintf(stderr, L"ERROR: %ls\n", error);
	mdea_parser_destroy(t);

	struct mdea_emitter *e = mdea_file_emitter(stdout);
	if (mdea_write(e, root, &error) != 0)
		fwprintf(stderr, L"ERROR: %ls\n", error);
	mdea_emitter_destroy(e);
}

