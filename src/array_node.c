/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "array_node.h"

struct mdea_array_node {
	const struct mdea_node_type *type;
	struct mdea_array array;
};

void mdea_array_node_destroy(void *p)
{
	struct mdea_array_node *n = p;
	mdea_array_deinit(&n->array);
}

int mdea_array_node_serialize(void *p, struct mdea_emitter *e, int indent, wchar_t **error)
{
	struct mdea_array *array;
	mdea_get_array(p, &array, NULL);
	int is_first = 1;
	if (mdea_emitter_emit(e, mdea_lbracket_token, error) != 0)
		return -1;
	for (int i = 0; i < array->size; ++i) {
		if (is_first)
			is_first = 0;
		else if (mdea_emitter_emit(e, mdea_comma_token, error) != 0)
			return -1;
		mdea_serialize(array->vals[i], e, indent + 1, error);
	}
	if (mdea_emitter_emit(e, mdea_rbracket_token, error) != 0)
		return -1;
	return 0;
}

int mdea_array_node_get_number(void *p, double *number, wchar_t **error)
{
	mdea_error(error, L"Expected number, got array");
	return -1;
}

int mdea_array_node_get_string(void *p, const wchar_t **string, wchar_t **error)
{
	mdea_error(error, L"Expected string, got array");
	return -1;
}

int mdea_array_node_get_boolean(void *p, int *boolean, wchar_t **error)
{
	mdea_error(error, L"Expected boolean, got array");
	return -1;
}

int mdea_array_node_get_array(void *p, struct mdea_array **array, wchar_t **error)
{
	struct mdea_array_node *n = p;
	*array = &n->array;
	return 0;
}

int mdea_array_node_get_object(void *p, struct mdea_object **object, wchar_t **error)
{
	mdea_error(error, L"Expected object, got array");
	return -1;
}

const struct mdea_node_type mdea_array_node_type = {
	mdea_array_node_destroy,
	mdea_array_node_serialize,
	mdea_array_node_get_number,
	mdea_array_node_get_string,
	mdea_array_node_get_boolean,
	mdea_array_node_get_array,
	mdea_array_node_get_object,
};

struct mdea_node *mdea_array_node(void)
{
	struct mdea_array_node *n = calloc(1, sizeof(*n));
	n->type = &mdea_array_node_type;
	mdea_array_init(&n->array);
	return (void *)n;
}

