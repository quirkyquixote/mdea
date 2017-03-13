/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "file_parser.h"

#include <ctype.h>

#include "error.h"

struct mdea_file_parser {
	const struct mdea_parser_type *type;
	FILE *file;
	size_t alloc;
	size_t size;
	char *buf;
};

void mdea_file_parser_destroy(void *p)
{
	struct mdea_file_parser *f = p;
	if (f->buf)
		free(f->buf);
}

wint_t get_utf8(struct mdea_file_parser *p)
{
	wint_t c;
	c = fgetc(p->file);
	if ((c & 0x80) == 0)
		return c;
	if ((c & 0xE0) == 0xC0) {
		c &= 0x1F;
		c += (fgetc(p->file) & 0x3F) << 5;
		return c;
	}
	if ((c & 0xF0) == 0xE0) {
		c &= 0x0F;
		c += (fgetc(p->file) & 0x3F) << 4;
		c += (fgetc(p->file) & 0x3F) << 10;
		return c;
	}
	if ((c & 0xF8) == 0xF0) {
		c &= 0x07;
		c += (fgetc(p->file) & 0x3F) << 3;
		c += (fgetc(p->file) & 0x3F) << 9;
		c += (fgetc(p->file) & 0x3F) << 15;
		return c;
	}
	return -1;
}


int mdea_file_parser_next(void *p, struct mdea_token *tok, char **error)
{
	struct mdea_file_parser *t = p;
	int c;
	do
		c = fgetc(t->file);
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
			c = fgetc(t->file);
			if (c == WEOF) {
				mdea_error(error, "Unexpected end of file");
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
		ungetc(c, t->file);
		tok->type = MDEA_TOK_NUMBER;
		fscanf(t->file, "%lf", &tok->number);
		return 0;
	} else if (c == 'n') {
		if (fgetc(t->file) != 'u' || get_utf8(t) != 'l'
				|| fgetc(t->file) != 'l') {
			mdea_error(error, "Expected null");
			return -1;
		}
		tok->type = MDEA_TOK_NULL;
		return 0;
	} else if (c == 't') {
		if (fgetc(t->file) != 'r' || get_utf8(t) != 'u'
				|| fgetc(t->file) != 'e') {
			mdea_error(error, "Expected true");
			return -1;
		}
		tok->type = MDEA_TOK_TRUE;
		return 0;
	} else if (c == 'f') {
		if (fgetc(t->file) != 'a' || get_utf8(t) != 'l'
				|| fgetc(t->file) != 's' || get_utf8(t) != 'e') {
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

int mdea_file_parser_parse(void *p, struct mdea_emitter *e, char **error)
{
	struct mdea_file_parser *t = p;
	struct mdea_token tok;
	while (mdea_file_parser_next(t, &tok, error) == 0) {
		if (tok.type == MDEA_TOK_END)
			return 0;
		if (mdea_emitter_emit(e, tok, error) != 0)
			return -1;
	}
	return -1;
}

const struct mdea_parser_type mdea_file_parser_type = {
	mdea_file_parser_destroy,
	mdea_file_parser_parse,
};

struct mdea_parser *mdea_file_parser(FILE *file)
{
	struct mdea_file_parser *t = calloc(1, sizeof(*t));
	t->type = &mdea_file_parser_type;
	t->file = file;
	t->alloc = 0;
	t->size = 0;
	t->buf = NULL;
	return (void *)t;
}
