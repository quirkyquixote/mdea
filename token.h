/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_TOKEN_H_
#define MDEA_TOKEN_H_

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

struct MdeaToken {
	enum {
		MDEA_TOK_NULL,
		MDEA_TOK_TRUE,
		MDEA_TOK_FALSE,
		MDEA_TOK_NUMBER,
		MDEA_TOK_STRING,
		MDEA_TOK_LBRACKET,
		MDEA_TOK_RBRACKET,
		MDEA_TOK_LCURLY,
		MDEA_TOK_RCURLY,
		MDEA_TOK_COMMA,
		MDEA_TOK_COLON,
		MDEA_TOK_END
	} type;
	union {
		wchar_t *string;
		double number;
	};
};

static inline void mdea_token_destroy(struct MdeaToken *t)
{
	if (t->type == MDEA_TOK_STRING)
		free(t->string);
}

int mdea_next_token(FILE *file, struct MdeaToken *tok, wchar_t **error);

#endif  // MDEA_TOKEN_H_
