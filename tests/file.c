/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include <stdio.h>
#include <unistd.h>

#include "mdea.h"

int main(int argc, char *argv[])
{
	char *error;
	struct mdea_input *i = mdea_file_input(STDIN_FILENO);
	struct mdea_parser *t = mdea_json_parser(i);
	struct mdea_emitter *e = mdea_file_emitter(STDOUT_FILENO);
	if (mdea_parse(t, e, &error) != 0)
		fprintf(stderr, "ERROR: %s\n", error);
	mdea_parser_destroy(t);
	mdea_emitter_destroy(e);
	mdea_input_destroy(i);
}

