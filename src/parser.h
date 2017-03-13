/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_PARSER_H_
#define MDEA_PARSER_H_

#include "emitter.h"

/*
 * Parser type
 */
struct mdea_parser_type {
	/* Destroy (but not free) */
	void (*destroy)(void *);
	/* Extract next token */
	int (*parse)(void *, struct mdea_emitter *, char **error);
};

/*
 * Generic parser
 */
struct mdea_parser {
	/* Type of parser */
	const struct mdea_parser_type *type;
};

/* Destroy parser */
static inline void mdea_parser_destroy(struct mdea_parser *t)
{
	t->type->destroy(t);
	free(t);
}

/* Parse */
static inline int mdea_parse(struct mdea_parser *t, struct mdea_emitter *e,
		char **error)
{
	return t->type->parse(t, e, error);
}

#endif  // MDEA_TOKEN_H_
