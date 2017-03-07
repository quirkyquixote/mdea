/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "mdea.h"

#include <stdarg.h>
#include <wctype.h>

#include "token.h"

int mdea_parse(FILE *file, struct mdea_token *tok, struct mdea_node **rval, wchar_t **error)
{
	if (tok->type == MDEA_TOK_END) {
		mdea_error(error, L"Unexpected end of file");
		return -1;
	} else if (tok->type == MDEA_TOK_NULL) {
		*rval = mdea_null();
		return 0;
	} else if (tok->type == MDEA_TOK_TRUE) {
		*rval = mdea_boolean(1);
		return 0;
	} else if (tok->type == MDEA_TOK_FALSE) {
		*rval = mdea_boolean(0);
		return 0;
	} else if (tok->type == MDEA_TOK_STRING) {
		*rval = mdea_string(tok->string);
		return 0;
	} else if (tok->type == MDEA_TOK_NUMBER) {
		*rval = mdea_number(tok->number);
		return 0;
	} else if (tok->type == MDEA_TOK_LBRACKET) {
		*rval = mdea_array();
		if (mdea_next_token(file, tok, error) != 0)
			return -1;
		if (tok->type == MDEA_TOK_RBRACKET)
			return 0;
		struct mdea_array *array;
		mdea_get_array(*rval, &array, NULL);
		for (;;) {
			struct mdea_node *tmp;
			if (mdea_parse(file, tok, &tmp, error) != 0)
				return -1;
			mdea_array_add(array, tmp);
			if (mdea_next_token(file, tok, error) != 0)
				return -1;
			if (tok->type == MDEA_TOK_RBRACKET)
				return 0;
			if (tok->type != MDEA_TOK_COMMA) {
				mdea_error(error, L"Expected end of list");
				return -1;
			}
			if (mdea_next_token(file, tok, error) != 0)
				return -1;
		}
	} else if (tok->type == MDEA_TOK_LCURLY) {
		*rval = mdea_object();
		if (mdea_next_token(file, tok, error) != 0)
			return -1;
		if (tok->type == MDEA_TOK_RCURLY)
			return 0;
		struct mdea_object *object;
		mdea_get_object(*rval, &object, NULL);
		for (;;) {
			struct mdea_token tok2;
			struct mdea_node *tmp;
			if (tok->type != MDEA_TOK_STRING) {
				mdea_error(error, L"Expected string");
				return -1;
			}
			if (mdea_next_token(file, &tok2, error) != 0)
				return -1;
			if (tok2.type != MDEA_TOK_COLON) {
				mdea_error(error, L"Expected ':'");
				return -1;
			}
			if (mdea_next_token(file, &tok2, error) != 0)
				return -1;
			if (mdea_parse(file, &tok2, &tmp, error) != 0)
				return -1;
			mdea_object_add(object, tok->string, tmp);
			if (mdea_next_token(file, tok, error) != 0)
				return -1;
			if (tok->type == MDEA_TOK_RCURLY)
				return 0;
			if (tok->type != MDEA_TOK_COMMA) {
				mdea_error(error, L"Expected end of list");
				return -1;
			}
			if (mdea_next_token(file, tok, error) != 0)
				return -1;
		}
	}
	mdea_error(error, L"bad token type: %d\n", tok->type);
	return -1;
}

int mdea_read(FILE *f, struct mdea_node **rval, wchar_t **error)
{
	struct mdea_token tok;
	int ret = -1;
	if (mdea_next_token(f, &tok, error) == 0) {
		if (mdea_parse(f, &tok, rval, error) == 0)
			ret = 0;
		mdea_token_destroy(&tok);
	}
	return ret;
}

int mdea_write(FILE *f, struct mdea_node *root, wchar_t **error)
{
	return mdea_serialize(root, f, 0, error);
}

int mdea_get(struct mdea_node *root, struct mdea_node **rval, ...)
{
	int found = 0;
	va_list ap;
	va_start(ap, rval);
	for (;;) {
		wchar_t *key = va_arg(ap, wchar_t *);
		if (key == NULL) {
			found = 1;
			break;
		}
		struct mdea_object *obj;
		if (mdea_get_object(root, &obj, NULL) != 0)
			break;
		if (mdea_object_get(obj, key, (void**)&root) != 0)
			break;
	}
	va_end(ap);
	return found;
}
