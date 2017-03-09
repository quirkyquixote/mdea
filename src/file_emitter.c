/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "file_emitter.h"

struct mdea_file_emitter {
	const struct mdea_emitter_type *type;
	FILE *file;
};

void mdea_file_emitter_destroy(void *p)
{
	struct mdea_file_emitter *e = p;
}

int mdea_file_emitter_emit_number(void *p, double number, wchar_t **error)
{
	struct mdea_file_emitter *e = p;
	fwprintf(e->file, L"%lg", number);
	return 0;
}

int mdea_file_emitter_emit_string(void *p, const wchar_t *string, wchar_t **error)
{
	struct mdea_file_emitter *e = p;
	fwprintf(e->file, L"%ls", string);
	return 0;
}

static const struct mdea_emitter_type mdea_file_emitter_type = {
	mdea_file_emitter_destroy,
	mdea_file_emitter_emit_number,
	mdea_file_emitter_emit_string,
};

struct mdea_emitter *mdea_file_emitter(FILE *file)
{
	struct mdea_file_emitter *e = calloc(1, sizeof(*e));
	e->type = &mdea_file_emitter_type;
	e->file = file;
	return (void *)e;
}

