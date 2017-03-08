/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "string_emitter.h"

struct mdea_string_emitter {
	const struct mdea_emitter_type *type;
	wchar_t *buf;
	size_t len;
};

void mdea_string_emitter_destroy(void *p)
{
	struct mdea_string_emitter *e = p;
}

int mdea_string_emitter_emit_number(void *p, double number, wchar_t **error)
{
	struct mdea_string_emitter *e = p;
	int tmp;
	tmp = swprintf(e->buf, e->len, L"%lg", number);
	e->buf += tmp;
	e->len -= tmp;
	return 0;
}

int mdea_string_emitter_emit_string(void *p, const wchar_t *string, wchar_t **error)
{
	struct mdea_string_emitter *e = p;
	int tmp;
	tmp = swprintf(e->buf, e->len, L"%ls", string);
	e->buf += tmp;
	e->len -= tmp;
	return 0;
}

static const struct mdea_emitter_type mdea_string_emitter_type = {
	mdea_string_emitter_destroy,
	mdea_string_emitter_emit_number,
	mdea_string_emitter_emit_string,
};

struct mdea_emitter *mdea_string_emitter(wchar_t *buf, size_t len)
{
	struct mdea_string_emitter *e = calloc(1, sizeof(*e));
	e->type = &mdea_string_emitter_type;
	e->buf = buf;
	e->len = len;
	return (void *)e;
}

