/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "token.h"

#include <wctype.h>

#include "error.h"

int mdea_next_token(FILE *file, struct MdeaToken *tok, wchar_t **error)
{
	mdea_token_destroy(tok);
	int c;
	do
		c = fgetwc(file);
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
			c = fgetwc(file);
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
		ungetwc(c, file);
		tok->type = MDEA_TOK_NUMBER;
		fwscanf(file, L"%lf", &tok->number);
		return 0;
	} else if (c == 'n') {
		if (fgetwc(file) != 'u' || fgetwc(file) != 'l'
				|| fgetwc(file) != 'l') {
			mdea_error(error, L"Expected null");
			return -1;
		}
		tok->type = MDEA_TOK_NULL;
		return 0;
	} else if (c == 't') {
		if (fgetwc(file) != 'r' || fgetwc(file) != 'u'
				|| fgetwc(file) != 'e') {
			mdea_error(error, L"Expected true");
			return -1;
		}
		tok->type = MDEA_TOK_TRUE;
		return 0;
	} else if (c == 'f') {
		if (fgetwc(file) != 'a' || fgetwc(file) != 'l'
				|| fgetwc(file) != 's' || fgetwc(file) != 'e') {
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
