/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "file_parser.h"

#include <wctype.h>

#include "error.h"

struct mdea_file_parser {
	const struct mdea_parser_type *type;
	FILE *file;
};

void mdea_file_parser_destroy(void *p)
{
}

int mdea_file_parser_next(void *p, struct mdea_token *tok, wchar_t **error)
{
	struct mdea_file_parser *t = p;
	mdea_token_destroy(tok);
	int c;
	do
		c = fgetwc(t->file);
	while (iswspace(c));
	if (c == WEOF) {
		tok->type = MDEA_TOK_END;
		return 0;
	} else if (c == '[') {
		tok->type = MDEA_TOK_LBRACKET;
		return 0;
	} else if (c == ']') {
		tok->type = MDEA_TOK_RBRACKET;
		return 0;
	} else if (c == '{') {
		tok->type = MDEA_TOK_LCURLY;
		return 0;
	} else if (c == '}') {
		tok->type = MDEA_TOK_RCURLY;
		return 0;
	} else if (c == ',') {
		tok->type = MDEA_TOK_COMMA;
		return 0;
	} else if (c == ':') {
		tok->type = MDEA_TOK_COLON;
		return 0;
	} else if (c == '"') {
		size_t alloc = 0;
		size_t size = 0;
		wchar_t *buf = NULL;
		int escaped = 0;
		for (;;) {
			c = fgetwc(t->file);
			if (c == WEOF) {
				mdea_error(error, L"Unexpected end of file");
				return -1;
			}
			if (!escaped) {
				if (c == '"')
					c = 0;
				else if (c == '\\')
					escaped = 1;
			} else {
				escaped = 0;
			}
			if (size == alloc) {
				alloc = alloc ? alloc * 2 : 2;
				buf = realloc(buf, sizeof(*buf) * alloc);
			}
			buf[size] = c;
			if (c == 0) {
				tok->type = MDEA_TOK_STRING;
				tok->string = buf;
				return 0;
			}
			++size;
		}
	} else if ((c >= '0' && c <= '9') || c == '.') {
		ungetwc(c, t->file);
		tok->type = MDEA_TOK_NUMBER;
		fwscanf(t->file, L"%lf", &tok->number);
		return 0;
	} else if (c == 'n') {
		if (fgetwc(t->file) != 'u' || fgetwc(t->file) != 'l'
				|| fgetwc(t->file) != 'l') {
			mdea_error(error, L"Expected null");
			return -1;
		}
		tok->type = MDEA_TOK_NULL;
		return 0;
	} else if (c == 't') {
		if (fgetwc(t->file) != 'r' || fgetwc(t->file) != 'u'
				|| fgetwc(t->file) != 'e') {
			mdea_error(error, L"Expected true");
			return -1;
		}
		tok->type = MDEA_TOK_TRUE;
		return 0;
	} else if (c == 'f') {
		if (fgetwc(t->file) != 'a' || fgetwc(t->file) != 'l'
				|| fgetwc(t->file) != 's' || fgetwc(t->file) != 'e') {
			mdea_error(error, L"Expected false");
			return -1;
		}
		tok->type = MDEA_TOK_FALSE;
		return 0;
	} else {
		if (iswprint(c))
			mdea_error(error, L"Unexpected character: '%lc'", c);
		else
			mdea_error(error, L"Unexpected character: \\u%04X", c);
		return -1;
	}
}

const struct mdea_parser_type mdea_file_parser_type = {
	mdea_file_parser_destroy,
	mdea_file_parser_next,
};

struct mdea_parser *mdea_file_parser(FILE *file)
{
	struct mdea_file_parser *t = calloc(1, sizeof(*t));
	t->type = &mdea_file_parser_type;
	t->file = file;
	return (void *)t;
}
