/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "string_parser.h"

#include <wctype.h>

#include "error.h"

struct mdea_string_parser {
	const struct mdea_parser_type *type;
	const wchar_t *cur;
	size_t alloc;
	size_t size;
	wchar_t *buf;
};

void mdea_string_parser_destroy(void *p)
{
	struct mdea_string_parser *t = p;
	if (t->buf)
		free(t->buf);
}

int mdea_string_parser_next(void *p, struct mdea_token *tok, wchar_t **error)
{
	struct mdea_string_parser *t = p;
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
		t->size = 0;
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
			if (t->size == t->alloc) {
				t->alloc = t->alloc ? t->alloc * 2 : 2;
				t->buf = realloc(t->buf, sizeof(*t->buf) * t->alloc);
			}
			t->buf[t->size] = *t->cur == '"' ? 0 : *t->cur;
			if (t->buf[t->size] == 0) {
				tok->type = MDEA_TOK_STRING;
				tok->string = t->buf;
				++t->cur;
				return 0;
			}
			++t->size;
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

int mdea_string_parser_parse(void *p, struct mdea_emitter *e, wchar_t **error)
{
	struct mdea_string_parser *t = p;
	struct mdea_token tok;
	while (mdea_string_parser_next(t, &tok, error) == 0) {
		if (tok.type == MDEA_TOK_END)
			return 0;
		if (mdea_emitter_emit(e, tok, error) != 0)
			return -1;
	}
	return -1;
}

const struct mdea_parser_type mdea_string_parser_type = {
	mdea_string_parser_destroy,
	mdea_string_parser_parse,
};

struct mdea_parser *mdea_string_parser(const wchar_t *string)
{
	struct mdea_string_parser *t = calloc(1, sizeof(*t));
	t->type = &mdea_string_parser_type;
	t->cur = string;
	t->alloc = 0;
	t->size = 0;
	t->buf = NULL;
	return (void *)t;
}
