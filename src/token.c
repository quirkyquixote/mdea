/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "token.h"

const struct mdea_token mdea_null_token = { MDEA_TOK_NULL };
const struct mdea_token mdea_true_token = { MDEA_TOK_TRUE };
const struct mdea_token mdea_false_token = { MDEA_TOK_FALSE };
const struct mdea_token mdea_lbracket_token = { MDEA_TOK_LBRACKET };
const struct mdea_token mdea_rbracket_token = { MDEA_TOK_RBRACKET };
const struct mdea_token mdea_lcurly_token = { MDEA_TOK_LCURLY };
const struct mdea_token mdea_rcurly_token = { MDEA_TOK_RCURLY };
const struct mdea_token mdea_comma_token = { MDEA_TOK_COMMA };
const struct mdea_token mdea_colon_token = { MDEA_TOK_COLON };
const struct mdea_token mdea_end_token = { MDEA_TOK_END };

