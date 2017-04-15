/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_TOKEN_H_
#define MDEA_TOKEN_H_

#include <stdio.h>
#include <stdlib.h>

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
		MDEA_TOK_END
	} type;
	union {
		double number;
		const char *string;
	};
};

extern const struct mdea_token mdea_null_token;
extern const struct mdea_token mdea_true_token;
extern const struct mdea_token mdea_false_token;
extern const struct mdea_token mdea_lbracket_token;
extern const struct mdea_token mdea_rbracket_token;
extern const struct mdea_token mdea_lcurly_token;
extern const struct mdea_token mdea_rcurly_token;
extern const struct mdea_token mdea_comma_token;
extern const struct mdea_token mdea_colon_token;
extern const struct mdea_token mdea_end_token;

static inline struct mdea_token mdea_number_token(double number)
{
	return (struct mdea_token){ MDEA_TOK_NUMBER, { .number = number } };
}

static inline struct mdea_token mdea_string_token(const char *string)
{
	return (struct mdea_token){ MDEA_TOK_STRING, { .string = string } };
}

#endif  // MDEA_TOKEN_H_
