/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_PARSER_H_
#define MDEA_PARSER_H_

#include "token.h"
#include "node.h"

/*
 * Parser type
 */
struct mdea_parser_type {
	/* Destroy (but not free) */
	void (*destroy)(void *);
	/* Extract next token */
	int (*parse)(void *, struct mdea_token *, wchar_t **error);
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

/* Get next token */
static inline int mdea_parser_parse(struct mdea_parser *t,
		struct mdea_token *tok, wchar_t **error)
{
	return t->type->parse(t, tok, error);
}

/* Parse node */
int mdea_read(struct mdea_parser *p, struct mdea_node **node, wchar_t **error);

#endif  // MDEA_TOKEN_H_
