/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "file_emitter.h"

#include "error.h"

struct mdea_file_emitter {
	const struct mdea_emitter_type *type;
	FILE *file;
	int indent;
	int last_token_type;
};

void mdea_file_emitter_destroy(void *p)
{
	struct mdea_file_emitter *e = p;
}

int mdea_file_emitter_emit(void *p, struct mdea_token tok, char **error)
{
	struct mdea_file_emitter *e = p;

	if ((e->last_token_type == MDEA_TOK_LBRACKET && tok.type != MDEA_TOK_RBRACKET) ||
		(e->last_token_type == MDEA_TOK_LCURLY && tok.type != MDEA_TOK_RCURLY)) {
		++e->indent;
		fprintf(e->file, "\n");
		for (int i = 0; i < e->indent; ++i)
			fprintf(e->file, "  ");
	}

	if ((tok.type == MDEA_TOK_RBRACKET && e->last_token_type != MDEA_TOK_LBRACKET) ||
		(tok.type == MDEA_TOK_RCURLY && e->last_token_type != MDEA_TOK_LCURLY)) {
		--e->indent;
		fprintf(e->file, "\n");
		for (int i = 0; i < e->indent; ++i)
			fprintf(e->file, "  ");
	}

	if (tok.type == MDEA_TOK_END) {
		/* do nothing */
	} else if (tok.type == MDEA_TOK_NULL) {
		fprintf(e->file, "null");
	} else if (tok.type == MDEA_TOK_TRUE) {
		fprintf(e->file, "true");
	} else if (tok.type == MDEA_TOK_FALSE) {
		fprintf(e->file, "false");
	} else if (tok.type == MDEA_TOK_NUMBER) {
		fprintf(e->file, "%lg", tok.number);
	} else if (tok.type == MDEA_TOK_STRING) {
		fprintf(e->file, "\"%s\"", tok.string);
	} else if (tok.type == MDEA_TOK_LBRACKET) {
		fprintf(e->file, "[");
	} else if (tok.type == MDEA_TOK_RBRACKET) {
		fprintf(e->file, "]");
	} else if (tok.type == MDEA_TOK_LCURLY) {
		fprintf(e->file, "{");
	} else if (tok.type == MDEA_TOK_RCURLY) {
		fprintf(e->file, "}");
	} else if (tok.type == MDEA_TOK_COMMA) {
		fprintf(e->file, ",\n");
		for (int i = 0; i < e->indent; ++i)
			fprintf(e->file, "  ");
	} else if (tok.type == MDEA_TOK_COLON) {
		fprintf(e->file, ": ");
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

struct mdea_emitter *mdea_file_emitter(FILE *file)
{
	struct mdea_file_emitter *e = calloc(1, sizeof(*e));
	e->type = &mdea_file_emitter_type;
	e->file = file;
	return (void *)e;
}

