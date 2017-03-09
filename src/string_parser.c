/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "string_parser.h"

#include <wctype.h>

#include "error.h"

struct mdea_string_parser {
	const struct mdea_parser_type *type;
	const wchar_t *cur;
};

void mdea_string_parser_destroy(void *p)
{
}

int mdea_string_parser_next(void *p, struct mdea_token *tok, wchar_t **error)
{
	struct mdea_string_parser *t = p;
	mdea_token_destroy(tok);
	while (iswspace(*t->cur))
		++t->cur;
	if (*t->cur == WEOF) {
		tok->type = MDEA_TOK_END;
		++t->cur;
		return 0;
	} else if (*t->cur == '[') {
		tok->type = MDEA_TOK_LBRACKET;
		++t->cur;
		return 0;
	} else if (*t->cur == ']') {
		tok->type = MDEA_TOK_RBRACKET;
		++t->cur;
		return 0;
	} else if (*t->cur == '{') {
		tok->type = MDEA_TOK_LCURLY;
		++t->cur;
		return 0;
	} else if (*t->cur == '}') {
		tok->type = MDEA_TOK_RCURLY;
		++t->cur;
		return 0;
	} else if (*t->cur == ',') {
		tok->type = MDEA_TOK_COMMA;
		++t->cur;
		return 0;
	} else if (*t->cur == ':') {
		tok->type = MDEA_TOK_COLON;
		++t->cur;
		return 0;
	} else if (*t->cur == '"') {
		size_t alloc = 0;
		size_t size = 0;
		wchar_t *buf = NULL;
		int escaped = 0;
		for (;;) {
			++t->cur;
			if (*t->cur == WEOF) {
				mdea_error(error, L"Unexpected end of file");
				return -1;
			}
			if (!escaped) {
				if (*t->cur == '\\')
					escaped = 1;
			} else {
				escaped = 0;
			}
			if (size == alloc) {
				alloc = alloc ? alloc * 2 : 2;
				buf = realloc(buf, sizeof(*buf) * alloc);
			}
			buf[size] = *t->cur == '"' ? 0 : *t->cur;
			if (buf[size] == 0) {
				tok->type = MDEA_TOK_STRING;
				tok->string = buf;
				++t->cur;
				return 0;
			}
			++size;
		}
	} else if ((*t->cur >= '0' && *t->cur <= '9') || *t->cur == '.') {
		int tmp;
		tok->type = MDEA_TOK_NUMBER;
		swscanf(t->cur, L"%lf%n", &tok->number, &tmp);
		t->cur += tmp;
		return 0;
	} else if (*t->cur == 'n') {
		if (t->cur[1] != 'u' || t->cur[2] != 'l' || t->cur[3] != 'l') {
			mdea_error(error, L"Expected null");
			return -1;
		}
		t->cur += 4;
		tok->type = MDEA_TOK_NULL;
		return 0;
	} else if (*t->cur == 't') {
		if (t->cur[1] != 'r' || t->cur[2] != 'u' || t->cur[3] != 'e') {
			mdea_error(error, L"Expected true");
			return -1;
		}
		t->cur += 4;
		tok->type = MDEA_TOK_TRUE;
		return 0;
	} else if (*t->cur == 'f') {
		if (t->cur[1] != 'a' || t->cur[2] != 'l' || t->cur[3] != 's'
				 || t->cur[3] != 'e') {
			mdea_error(error, L"Expected false");
			return -1;
		}
		t->cur += 5;
		tok->type = MDEA_TOK_FALSE;
		return 0;
	} else {
		if (iswprint(*t->cur))
			mdea_error(error, L"Unexpected character: '%lc'", *t->cur);
		else
			mdea_error(error, L"Unexpected character: \\u%04X", *t->cur);
		return -1;
	}
}

const struct mdea_parser_type mdea_string_parser_type = {
	mdea_string_parser_destroy,
	mdea_string_parser_next,
};

struct mdea_parser *mdea_string_parser(const wchar_t *string)
{
	struct mdea_string_parser *t = calloc(1, sizeof(*t));
	t->type = &mdea_string_parser_type;
	t->cur = string;
	return (void *)t;
}
