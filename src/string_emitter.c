/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "string_emitter.h"

#include "error.h"

struct mdea_string_emitter {
	const struct mdea_emitter_type *type;
	wchar_t *buf;
	size_t len;
	int indent;
	int last_token_type;
};

void mdea_string_emitter_destroy(void *p)
{
	struct mdea_string_emitter *e = p;
}

int emit(struct mdea_string_emitter *e, const wchar_t *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int ret = vswprintf(e->buf, e->len, fmt, ap);
	va_end(ap);
	e->buf += ret;
	e->len -= ret;
	return ret;
}

int mdea_string_emitter_emit(void *p, struct mdea_token tok, wchar_t **error)
{
	struct mdea_string_emitter *e = p;

	if ((e->last_token_type == MDEA_TOK_LBRACKET && tok.type != MDEA_TOK_RBRACKET) ||
		(e->last_token_type == MDEA_TOK_LCURLY && tok.type != MDEA_TOK_RCURLY)) {
		++e->indent;
		emit(e, L"\n");
		for (int i = 0; i < e->indent; ++i)
			emit(e, L"  ");
	}

	if ((tok.type == MDEA_TOK_RBRACKET && e->last_token_type != MDEA_TOK_LBRACKET) ||
		(tok.type == MDEA_TOK_RCURLY && e->last_token_type != MDEA_TOK_LCURLY)) {
		--e->indent;
		emit(e, L"\n");
		for (int i = 0; i < e->indent; ++i)
			emit(e, L"  ");
	}

	if (tok.type == MDEA_TOK_END) {
		/* do nothing */
	} else if (tok.type == MDEA_TOK_NULL) {
		emit(e, L"null");
	} else if (tok.type == MDEA_TOK_TRUE) {
		emit(e, L"true");
	} else if (tok.type == MDEA_TOK_FALSE) {
		emit(e, L"false");
	} else if (tok.type == MDEA_TOK_NUMBER) {
		emit(e, L"%lg", tok.number);
	} else if (tok.type == MDEA_TOK_STRING) {
		emit(e, L"\"%ls\"", tok.string);
	} else if (tok.type == MDEA_TOK_LBRACKET) {
		emit(e, L"[");
	} else if (tok.type == MDEA_TOK_RBRACKET) {
		emit(e, L"]");
	} else if (tok.type == MDEA_TOK_LCURLY) {
		emit(e, L"{");
	} else if (tok.type == MDEA_TOK_RCURLY) {
		emit(e, L"}");
	} else if (tok.type == MDEA_TOK_COMMA) {
		emit(e, L",\n");
		for (int i = 0; i < e->indent; ++i)
			emit(e, L"  ");
	} else if (tok.type == MDEA_TOK_COLON) {
		emit(e, L": ");
	} else {
		mdea_error(error, L"Bad token: %d", tok.type);
		return -1;
	}
	e->last_token_type = tok.type;
	return 0;
}

static const struct mdea_emitter_type mdea_string_emitter_type = {
	mdea_string_emitter_destroy,
	mdea_string_emitter_emit,
};

struct mdea_emitter *mdea_string_emitter(wchar_t *buf, size_t len)
{
	struct mdea_string_emitter *e = calloc(1, sizeof(*e));
	e->type = &mdea_string_emitter_type;
	e->buf = buf;
	e->len = len;
	return (void *)e;
}

