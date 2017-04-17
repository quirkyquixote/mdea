/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "json_parser.h"

#include <ctype.h>
#include <string.h>
#include <unistd.h>

#include "error.h"

struct mdea_json_parser {
	const struct mdea_parser_type *type;
	struct mdea_input *in;
	size_t alloc;
	size_t size;
	char *buf;
};

void mdea_json_parser_destroy(void *p)
{
	struct mdea_json_parser *f = p;
	if (f->buf)
		free(f->buf);
}

int mdea_json_parser_next(void *p, struct mdea_token *tok, char **error)
{
	struct mdea_json_parser *t = p;
	int c;
	do
		c = mdea_input_get(t->in);
	while (isspace(c));
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
		t->size = 0;
		int escaped = 0;
		for (;;) {
			c = mdea_input_get(t->in);
			if (c == WEOF) {
				mdea_error(error, "Unexpected end of json");
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
			if (t->size == t->alloc) {
				t->alloc = t->alloc ? t->alloc * 2 : 2;
				t->buf = realloc(t->buf, sizeof(*t->buf) * t->alloc);
			}
			t->buf[t->size] = c;
			if (c == 0) {
				tok->type = MDEA_TOK_STRING;
				tok->string = t->buf;
				return 0;
			}
			++t->size;
		}
	} else if ((c >= '0' && c <= '9') || c == '.') {
		char buf[64];
		int i = 0;
		do {
			buf[i++] = c;
			c = mdea_input_get(t->in);
		} while (strchr("0123456789.Ee+-", c) != NULL);
		mdea_input_unget(t->in, c);
		buf[i] = 0;
		tok->type = MDEA_TOK_NUMBER;
		sscanf(buf, "%lf", &tok->number);
		return 0;
	} else if (c == 'n') {
		if (mdea_input_get(t->in) != 'u'
				|| mdea_input_get(t->in) != 'l'
				|| mdea_input_get(t->in) != 'l') {
			mdea_error(error, "Expected null");
			return -1;
		}
		tok->type = MDEA_TOK_NULL;
		return 0;
	} else if (c == 't') {
		if (mdea_input_get(t->in) != 'r'
				|| mdea_input_get(t->in) != 'u'
				|| mdea_input_get(t->in) != 'e') {
			mdea_error(error, "Expected true");
			return -1;
		}
		tok->type = MDEA_TOK_TRUE;
		return 0;
	} else if (c == 'f') {
		if (mdea_input_get(t->in) != 'a'
				|| mdea_input_get(t->in) != 'l'
				|| mdea_input_get(t->in) != 's'
				|| mdea_input_get(t->in) != 'e') {
			mdea_error(error, "Expected false");
			return -1;
		}
		tok->type = MDEA_TOK_FALSE;
		return 0;
	} else {
		if (isprint(c))
			mdea_error(error, "Unexpected character: '%lc'", c);
		else
			mdea_error(error, "Unexpected character: \\u%04X", c);
		return -1;
	}
}

int mdea_json_parser_parse(void *p, struct mdea_emitter *e, char **error)
{
	struct mdea_json_parser *t = p;
	struct mdea_token tok;
	while (mdea_json_parser_next(t, &tok, error) == 0) {
		if (tok.type == MDEA_TOK_END)
			return 0;
		if (mdea_emitter_emit(e, tok, error) != 0)
			return -1;
	}
	return -1;
}

const struct mdea_parser_type mdea_json_parser_type = {
	mdea_json_parser_destroy,
	mdea_json_parser_parse,
};

struct mdea_parser *mdea_json_parser(struct mdea_input *in)
{
	struct mdea_json_parser *t = calloc(1, sizeof(*t));
	t->type = &mdea_json_parser_type;
	t->in = in;
	t->alloc = 0;
	t->size = 0;
	t->buf = NULL;
	return (void *)t;
}
