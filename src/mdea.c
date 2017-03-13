/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "mdea.h"

#include <ctype.h>

struct path_token {
	enum {
		PATH_TOKEN_END,
		PATH_TOKEN_INDEX,
		PATH_TOKEN_FIELD,
	} type;
	union {
		int index;
		char *field;
	};
};

static void path_token_destroy(struct path_token *tok)
{
	if (tok->type == PATH_TOKEN_FIELD)
		free(tok->field);
}

static int path_token_next(char **path, struct path_token *tok, char **error)
{
	if (**path == 0) {
		tok->type = PATH_TOKEN_END;
		return 0;
	}
	if (**path == '[') {
		int key, tmp;
		if (sscanf(*path, "[%d]%n", &key, &tmp) < 1) {
			mdea_error(error, "expected [<number>]");
			return -1;
		}
		*path += tmp;
		tok->type = PATH_TOKEN_INDEX;
		tok->index = key;
		return 0;
	}
	if (**path == '.') {
		char key[strlen(*path) + 1];
		int tmp;
		if (sscanf(*path, ".%[^[.]%n", key, &tmp) < 1) {
			mdea_error(error, "expected .string");
			return -1;
		}
		*path += tmp;
		tok->type = PATH_TOKEN_FIELD;
		tok->field = strdup(key);
		return 0;
	}
	if (isprint(**path))
		mdea_error(error, "Unexpected character: '%lc'", **path);
	else
		mdea_error(error, "Unexpected character: \\u%04X", **path);
	return -1;
}

int mdea_get(struct mdea_node *root, char *key, struct mdea_node **rval, char **error)
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
			if (mdea_array_get(array, tok.index, rval) != 0) {
				mdea_error(error, "No such index: %d", tok.index);
				break;
			}
		} else if (tok.type == PATH_TOKEN_FIELD) {
			struct mdea_object *object;
			if (mdea_get_object(*rval, &object, error) != 0)
				break;
			if (mdea_object_get(object, tok.field, rval) != 0) {
				mdea_error(error, "No such field: %ls", tok.field);
				break;
			}
		} else {
			mdea_error(error, "Expected number or string");
			break;
		}
		path_token_destroy(&tok);
	}
	path_token_destroy(&tok);
	return ret;
}

int mdea_set(struct mdea_node **root, char *key, struct mdea_node *val, char **error)
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
			*root = mdea_array_node();
		if (mdea_get_array(*root, &array, error) != 0)
			goto cleanup;
		struct mdea_node *node;
		if (mdea_array_get(array, tok.index, &node) == 0) {
			if (mdea_set(&node, key, val, error) == 0) {
				ret = 0;
				goto cleanup;
			}
		}
		node = NULL;
		if (mdea_set(&node, key, val, error) != 0)
			goto cleanup;
		if (mdea_array_insert(array, tok.index, node) != 0) {
			mdea_error(error, "Array accessed out of bounds");
			goto cleanup;
		}
		ret = 0;
	} else if (tok.type == PATH_TOKEN_FIELD) {
		struct mdea_object *object;
		if (*root == NULL)
			*root = mdea_object_node();
		if (mdea_get_object(*root, &object, error) != 0)
			goto cleanup;
		struct mdea_node *node;
		if (mdea_object_get(object, tok.field, &node) == 0) {
			if (mdea_set(&node, key, val, error) == 0) {
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
		mdea_error(error, "Expected number or string");
	}
cleanup:
	path_token_destroy(&tok);
	return ret;
}
