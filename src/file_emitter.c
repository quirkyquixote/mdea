/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "file_emitter.h"

#include <string.h>
#include <unistd.h>

#include "error.h"

struct mdea_file_emitter {
	const struct mdea_emitter_type *type;
	int fd;
	int indent;
	int last_token_type;
};

void mdea_file_emitter_destroy(void *p)
{
	struct mdea_file_emitter *e = p;
}

static inline int emit(struct mdea_file_emitter *e, const char *str)
{
	size_t len = strlen(str);
	return write(e->fd, str, len) == len ? 0 : -1;
}

int mdea_file_emitter_emit(void *p, struct mdea_token tok, char **error)
{
	struct mdea_file_emitter *e = p;

	if ((e->last_token_type == MDEA_TOK_LBRACKET && tok.type != MDEA_TOK_RBRACKET) ||
		(e->last_token_type == MDEA_TOK_LCURLY && tok.type != MDEA_TOK_RCURLY)) {
		++e->indent;
		emit(e, "\n");
		for (int i = 0; i < e->indent; ++i)
			emit(e, "  ");
	}

	if ((tok.type == MDEA_TOK_RBRACKET && e->last_token_type != MDEA_TOK_LBRACKET) ||
		(tok.type == MDEA_TOK_RCURLY && e->last_token_type != MDEA_TOK_LCURLY)) {
		--e->indent;
		emit(e, "\n");
		for (int i = 0; i < e->indent; ++i)
			emit(e, "  ");
	}

	if (tok.type == MDEA_TOK_END) {
		/* do nothing */
	} else if (tok.type == MDEA_TOK_NULL) {
		emit(e, "null");
	} else if (tok.type == MDEA_TOK_TRUE) {
		emit(e, "true");
	} else if (tok.type == MDEA_TOK_FALSE) {
		emit(e, "false");
	} else if (tok.type == MDEA_TOK_NUMBER) {
		char buf[64];
		snprintf(buf, sizeof(buf), "%lg", tok.number);
		emit(e, buf);
	} else if (tok.type == MDEA_TOK_STRING) {
		emit(e, "\"");
		emit(e, tok.string);
		emit(e, "\"");
	} else if (tok.type == MDEA_TOK_LBRACKET) {
		emit(e, "[");
	} else if (tok.type == MDEA_TOK_RBRACKET) {
		emit(e, "]");
	} else if (tok.type == MDEA_TOK_LCURLY) {
		emit(e, "{");
	} else if (tok.type == MDEA_TOK_RCURLY) {
		emit(e, "}");
	} else if (tok.type == MDEA_TOK_COMMA) {
		emit(e, ",\n");
		for (int i = 0; i < e->indent; ++i)
			emit(e, "  ");
	} else if (tok.type == MDEA_TOK_COLON) {
		emit(e, ": ");
	} else {
		mdea_error(error, "Bad token: %d", tok.type);
		return -1;
	}
	e->last_token_type = tok.type;
	return 0;
}

static const struct mdea_emitter_type mdea_file_emitter_type = {
	mdea_file_emitter_destroy,
	mdea_file_emitter_emit,
};

struct mdea_emitter *mdea_file_emitter(int fd)
{
	struct mdea_file_emitter *e = calloc(1, sizeof(*e));
	e->type = &mdea_file_emitter_type;
	e->fd = fd;
	return (void *)e;
}

