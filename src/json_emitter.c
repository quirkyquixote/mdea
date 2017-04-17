/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "json_emitter.h"

#include <string.h>
#include <unistd.h>

#include "error.h"

struct mdea_json_emitter {
	const struct mdea_emitter_type *type;
	struct mdea_output *out;
	int indent;
	int last_token_type;
};

void mdea_json_emitter_destroy(void *p)
{
	struct mdea_json_emitter *e = p;
}

int mdea_json_emitter_emit(void *p, struct mdea_token tok, char **error)
{
	struct mdea_json_emitter *e = p;

	if ((e->last_token_type == MDEA_TOK_LBRACKET && tok.type != MDEA_TOK_RBRACKET) ||
		(e->last_token_type == MDEA_TOK_LCURLY && tok.type != MDEA_TOK_RCURLY)) {
		++e->indent;
		mdea_output_puts(e->out, "\n");
		for (int i = 0; i < e->indent; ++i)
			mdea_output_puts(e->out, "  ");
	}

	if ((tok.type == MDEA_TOK_RBRACKET && e->last_token_type != MDEA_TOK_LBRACKET) ||
		(tok.type == MDEA_TOK_RCURLY && e->last_token_type != MDEA_TOK_LCURLY)) {
		--e->indent;
		mdea_output_puts(e->out, "\n");
		for (int i = 0; i < e->indent; ++i)
			mdea_output_puts(e->out, "  ");
	}

	if (tok.type == MDEA_TOK_END) {
		/* do nothing */
	} else if (tok.type == MDEA_TOK_NULL) {
		mdea_output_puts(e->out, "null");
	} else if (tok.type == MDEA_TOK_TRUE) {
		mdea_output_puts(e->out, "true");
	} else if (tok.type == MDEA_TOK_FALSE) {
		mdea_output_puts(e->out, "false");
	} else if (tok.type == MDEA_TOK_NUMBER) {
		char buf[64];
		snprintf(buf, sizeof(buf), "%lg", tok.number);
		mdea_output_puts(e->out, buf);
	} else if (tok.type == MDEA_TOK_STRING) {
		mdea_output_puts(e->out, "\"");
		mdea_output_puts(e->out, tok.string);
		mdea_output_puts(e->out, "\"");
	} else if (tok.type == MDEA_TOK_LBRACKET) {
		mdea_output_puts(e->out, "[");
	} else if (tok.type == MDEA_TOK_RBRACKET) {
		mdea_output_puts(e->out, "]");
	} else if (tok.type == MDEA_TOK_LCURLY) {
		mdea_output_puts(e->out, "{");
	} else if (tok.type == MDEA_TOK_RCURLY) {
		mdea_output_puts(e->out, "}");
	} else if (tok.type == MDEA_TOK_COMMA) {
		mdea_output_puts(e->out, ",\n");
		for (int i = 0; i < e->indent; ++i)
			mdea_output_puts(e->out, "  ");
	} else if (tok.type == MDEA_TOK_COLON) {
		mdea_output_puts(e->out, ": ");
	} else {
		mdea_error(error, "Bad token: %d", tok.type);
		return -1;
	}
	e->last_token_type = tok.type;
	return 0;
}

static const struct mdea_emitter_type mdea_json_emitter_type = {
	mdea_json_emitter_destroy,
	mdea_json_emitter_emit,
};

struct mdea_emitter *mdea_json_emitter(struct mdea_output *out)
{
	struct mdea_json_emitter *e = calloc(1, sizeof(*e));
	e->type = &mdea_json_emitter_type;
	e->out = out;
	return (void *)e;
}

