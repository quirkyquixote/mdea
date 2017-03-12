/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include <stdio.h>

#include "mdea.h"

int main(int argc, char *argv[])
{
	wchar_t *error;
	size_t alloc = 0;
	size_t len = 0;
	wchar_t *buf = NULL;
	wint_t c;
	for (;;) {
		if (len == alloc) {
			alloc = alloc ? 2 * alloc : 2;
			buf = realloc(buf, alloc * sizeof(*buf));
		}
		c = fgetwc(stdin);
		if (c == WEOF)
			break;
		buf[len++] = c;
	}
	buf[len] = 0;
	struct mdea_parser *t = mdea_string_parser(buf);
	struct mdea_emitter *e = mdea_string_emitter(buf, len);
	if (mdea_parse(t, e, &error) != 0)
		fwprintf(stderr, L"ERROR: %ls\n", error);
	else
		fwprintf(stdout, L"%ls", buf);
	mdea_parser_destroy(t);
	mdea_emitter_destroy(e);
}

