/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_TOKEN_H_
#define MDEA_TOKEN_H_

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

/*
 * Token read from a file or string
 */
struct mdea_token {
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
		MDEA_TOK_BREAK,
		MDEA_TOK_INDENT,
		MDEA_TOK_END
	} type;
	union {
		wchar_t *string;
		double number;
	};
};

#endif  // MDEA_TOKEN_H_
