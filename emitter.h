/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_EMITTER_H_
#define MDEA_EMITTER_H_

#include <wchar.h>
#include <stdlib.h>

/*
 * Functions for an emitter type
 */
struct mdea_emitter_type {
	/* Destroy (but not free) */
	void (*destroy)(void *);
	/* Emit a number */
	int (*emit_number)(void *, double number, wchar_t **error);
	/* Emit a string */
	int (*emit_string)(void *, const wchar_t *string, wchar_t **error);
};

/*
 * Exposed fields for all node types
 */
struct mdea_emitter {
	/* Pointer to type definition */
	const struct mdea_emitter_type *type;
};

/* Destroy */
static inline void mdea_emitter_destroy(struct mdea_emitter *e)
{
	e->type->destroy(e);
	free(e);
}

/* Emit a number */
static inline int mdea_emitter_emit_number(struct mdea_emitter *e,
		double number, wchar_t **error)
{
	return e->type->emit_number(e, number, error);
}

/* Emit a string */
static inline int mdea_emitter_emit_string(struct mdea_emitter *e,
		const wchar_t *string, wchar_t **error)
{
	return e->type->emit_string(e, string, error);
}

#endif  // MDEA_EMITTER_H_
