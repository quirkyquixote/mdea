/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "mdea.h"

#include <stdarg.h>
#include <wctype.h>

#include "token.h"

void mdea_destroy(void *ptr)
{
	struct MdeaNode *node = ptr;
	switch (node->type) {
		case MDEA_NULL:
		case MDEA_NUMBER:
		case MDEA_BOOLEAN:
			break;
		case MDEA_STRING:
			free(node->string);
			break;
		case MDEA_ARRAY:
			mdea_array_deinit(&node->array);
			break;
		case MDEA_OBJECT:
			mdea_object_deinit(&node->object);
			break;
	}
	free(node);
}

int mdea_parse(FILE *file, struct MdeaToken *tok, struct MdeaNode **rval, wchar_t **error)
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
		struct MdeaArray *array;
		mdea_get_array(*rval, &array);
		for (;;) {
			struct MdeaNode *tmp;
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
		struct MdeaObject *object;
		mdea_get_object(*rval, &object);
		for (;;) {
			struct MdeaToken tok2;
			struct MdeaNode *tmp;
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

int mdea_read(FILE *f, struct MdeaNode **rval, wchar_t **error)
{
	struct MdeaToken tok;
	int ret = -1;
	if (mdea_next_token(f, &tok, error) == 0) {
		if (mdea_parse(f, &tok, rval, error) == 0)
			ret = 0;
		mdea_token_destroy(&tok);
	}
	return ret;
}

int mdea_write(FILE *f, struct MdeaNode *root, int indent, wchar_t **error)
{
	switch (root->type) {
		case MDEA_NULL:
			fwprintf(f, L"null");
			return 0;
		case MDEA_NUMBER:
			fwprintf(f, L"%g", root->number);
			return 0;
		case MDEA_STRING:
			fwprintf(f, L"\"%ls\"", root->string);
			return 0;
		case MDEA_BOOLEAN:
			fwprintf(f, L"%ls", root->boolean ? L"true" : L"false");
			return 0;
		case MDEA_ARRAY:
			if (root->array.size == 0) {
				fwprintf(f, L"[]");
			} else {
				int is_first = 1;
				fwprintf(f, L"[\n");
				for (int i = 0; i < root->array.size; ++i) {
					if (is_first)
						is_first = 0;
					else
						fwprintf(f, L",\n");
					for (int i = 0; i <= indent; ++i)
						fwprintf(f, L"  ");
					mdea_write(f, root->array.vals[i], indent + 1, error);
				}
				fwprintf(f, L"\n");
				for (int i = 0; i < indent; ++i)
					fwprintf(f, L"  ");
				fwprintf(f, L"]");
			}
			return 0;
		case MDEA_OBJECT:
			if (root->object.size == 0) {
				fwprintf(f, L"{}");
			} else {
				int is_first = 1;
				fwprintf(f, L"{\n");
				for (int i = 0; i < root->object.size; ++i) {
					if (is_first)
						is_first = 0;
					else
						fwprintf(f, L",\n");
					for (int i = 0; i <= indent; ++i)
						fwprintf(f, L"  ");
					fwprintf(f, L"\"%ls\": ", root->object.fields[i].key);
					mdea_write(f, root->object.fields[i].val, indent + 1, error);
				}
				fwprintf(f, L"\n");
				for (int i = 0; i < indent; ++i)
					fwprintf(f, L"  ");
				fwprintf(f, L"}");
			}
			return 0;
	}
	return -1;
}

int mdea_get(struct MdeaNode *root, struct MdeaNode **rval, ...)
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
		struct MdeaObject *obj;
		if (mdea_get_object(root, &obj) != 0)
			break;
		if (mdea_object_get(obj, key, (void**)&root) != 0)
			break;
	}
	va_end(ap);
	return found;
}
