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

int mdea_file_emitter_emit(void *p, struct mdea_token tok, wchar_t **error)
{
	struct mdea_file_emitter *e = p;

	if ((e->last_token_type == MDEA_TOK_LBRACKET && tok.type != MDEA_TOK_RBRACKET) ||
		(e->last_token_type == MDEA_TOK_LCURLY && tok.type != MDEA_TOK_RCURLY)) {
		++e->indent;
		fwprintf(e->file, L"\n");
		for (int i = 0; i < e->indent; ++i)
			fwprintf(e->file, L"  ");
	}

	if ((tok.type == MDEA_TOK_RBRACKET && e->last_token_type != MDEA_TOK_LBRACKET) ||
		(tok.type == MDEA_TOK_RCURLY && e->last_token_type != MDEA_TOK_LCURLY)) {
		--e->indent;
		fwprintf(e->file, L"\n");
		for (int i = 0; i < e->indent; ++i)
			fwprintf(e->file, L"  ");
	}

	if (tok.type == MDEA_TOK_END) {
		/* do nothing */
	} else if (tok.type == MDEA_TOK_NULL) {
		fwprintf(e->file, L"null");
	} else if (tok.type == MDEA_TOK_TRUE) {
		fwprintf(e->file, L"true");
	} else if (tok.type == MDEA_TOK_FALSE) {
		fwprintf(e->file, L"false");
	} else if (tok.type == MDEA_TOK_NUMBER) {
		fwprintf(e->file, L"%lg", tok.number);
	} else if (tok.type == MDEA_TOK_STRING) {
		fwprintf(e->file, L"\"%ls\"", tok.string);
	} else if (tok.type == MDEA_TOK_LBRACKET) {
		fwprintf(e->file, L"[");
	} else if (tok.type == MDEA_TOK_RBRACKET) {
		fwprintf(e->file, L"]");
	} else if (tok.type == MDEA_TOK_LCURLY) {
		fwprintf(e->file, L"{");
	} else if (tok.type == MDEA_TOK_RCURLY) {
		fwprintf(e->file, L"}");
	} else if (tok.type == MDEA_TOK_COMMA) {
		fwprintf(e->file, L",\n");
		for (int i = 0; i < e->indent; ++i)
			fwprintf(e->file, L"  ");
	} else if (tok.type == MDEA_TOK_COLON) {
		fwprintf(e->file, L": ");
	} else {
		mdea_error(error, L"Bad token: %d", tok.type);
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

