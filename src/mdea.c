/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "mdea.h"

#include <wctype.h>

struct path_token {
	enum {
		PATH_TOKEN_END,
		PATH_TOKEN_INDEX,
		PATH_TOKEN_FIELD,
	} type;
	union {
		int index;
		wchar_t *field;
	};
};

static void path_token_destroy(struct path_token *tok)
{
	if (tok->type == PATH_TOKEN_FIELD)
		free(tok->field);
}

static int path_token_next(wchar_t **path, struct path_token *tok, wchar_t **error)
{
	if (**path == 0) {
		tok->type = PATH_TOKEN_END;
		return 0;
	}
	if (**path == '[') {
		int key, tmp;
		if (swscanf(*path, L"[%d]%n", &key, &tmp) < 1) {
			mdea_error(error, L"expected [<number>]");
			return -1;
		}
		*path += tmp;
		tok->type = PATH_TOKEN_INDEX;
		tok->index = key;
		return 0;
	}
	if (**path == '.') {
		wchar_t key[wcslen(*path) + 1];
		int tmp;
		if (swscanf(*path, L".%l[^[.]%n", key, &tmp) < 1) {
			mdea_error(error, L"expected .string");
			return -1;
		}
		*path += tmp;
		tok->type = PATH_TOKEN_FIELD;
		tok->field = wcsdup(key);
		return 0;
	}
	if (iswprint(**path))
		mdea_error(error, L"Unexpected character: '%lc'", **path);
	else
		mdea_error(error, L"Unexpected character: \\u%04X", **path);
	return -1;
}

int mdea_get(struct mdea_node *root, wchar_t *key, struct mdea_node **rval, wchar_t **error)
{
	struct path_token tok;
	int ret = -1;
	*rval = root;
	while (path_token_next(&key, &tok, error) == 0) {
		if (tok.type == PATH_TOKEN_END) {
			ret = 0;
			break;
		} else if (tok.type == PATH_TOKEN_INDEX) {
			struct mdea_array *array;
			if (mdea_get_array(*rval, &array, error) != 0)
				break;
			if (mdea_array_get(array, tok.index, rval) != 0)
				break;
		} else if (tok.type == PATH_TOKEN_FIELD) {
			struct mdea_object *object;
			if (mdea_get_object(*rval, &object, error) != 0)
				break;
			if (mdea_object_get(object, tok.field, rval) != 0)
				break;
		} else {
			mdea_error(error, L"Expected number or string");
			break;
		}
		path_token_destroy(&tok);
	}
	path_token_destroy(&tok);
	return ret;
}

int mdea_set(struct mdea_node **root, wchar_t *key, struct mdea_node *val, wchar_t **error)
{
	struct path_token tok;
	int ret = -1;
	if (path_token_next(&key, &tok, error) != 0)
		return -1;
	if (tok.type == PATH_TOKEN_END) {
		*root = val;
		ret = 0;
	} else if (tok.type == PATH_TOKEN_INDEX) {
		struct mdea_array *array;
		if (*root == NULL)
			*root = mdea_array();
		if (mdea_get_array(*root, &array, error) != 0)
			goto cleanup;
		struct mdea_node *node;
		if (mdea_array_get(array, tok.index, &node) == 0) {
			if (mdea_set(&node, key, val, error) != 0) {
				ret = 0;
				goto cleanup;
			}
		}
		node = NULL;
		if (mdea_set(&node, key, val, error) != 0)
			goto cleanup;
		if (mdea_array_insert(array, tok.index, node) != 0) {
			mdea_error(error, L"Array accessed out of bounds");
			goto cleanup;
		}
		ret = 0;
	} else if (tok.type == PATH_TOKEN_FIELD) {
		struct mdea_object *object;
		if (*root == NULL)
			*root = mdea_object();
		if (mdea_get_object(*root, &object, error) != 0)
			goto cleanup;
		struct mdea_node *node;
		if (mdea_object_get(object, tok.field, &node) == 0) {
			if (mdea_set(&node, key, val, error) != 0) {
				ret = 0;
				goto cleanup;
			}
		}
		node = NULL;
		if (mdea_set(&node, key, val, error) != 0)
			goto cleanup;
		mdea_object_insert(object, tok.field, node);
		ret = 0;
	} else {
		mdea_error(error, L"Expected number or string");
	}
cleanup:
	path_token_destroy(&tok);
	return ret;
}
