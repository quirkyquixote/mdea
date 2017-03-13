/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_EMITTER_H_
#define MDEA_EMITTER_H_

#include <wchar.h>
#include <stdlib.h>

#include "token.h"

/*
 * Functions for an emitter type
 */
struct mdea_emitter_type {
	/* Destroy (but not free) */
	void (*destroy)(void *);
	/* Emit a token */
	int (*emit)(void *, struct mdea_token tok, char **error);
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

/* Emit a token */
static inline int mdea_emitter_emit(struct mdea_emitter *e, struct mdea_token tok, char **error)
{
	return e->type->emit(e, tok, error);
}

#endif  // MDEA_EMITTER_H_
