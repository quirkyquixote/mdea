/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_EMITTER_H_
#define MDEA_EMITTER_H_

#include <wchar.h>
#include <stdlib.h>

struct mdea_emitter_type {
	void (*destroy)(void *);
	int (*emit_number)(void *, double number, wchar_t **error);
	int (*emit_string)(void *, const wchar_t *string, wchar_t **error);
};

struct mdea_emitter {
	const struct mdea_emitter_type *type;
};

static inline void mdea_emitter_destroy(struct mdea_emitter *e)
{
	e->type->destroy(e);
	free(e);
}

static inline int mdea_emitter_emit_number(struct mdea_emitter *e,
		double number, wchar_t **error)
{
	return e->type->emit_number(e, number, error);
}

static inline int mdea_emitter_emit_string(struct mdea_emitter *e,
		const wchar_t *string, wchar_t **error)
{
	return e->type->emit_string(e, string, error);
}

#endif  // MDEA_EMITTER_H_
