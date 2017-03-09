/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "boolean_node.h"

struct mdea_boolean_node {
	const struct mdea_node_type *type;
	int boolean;
};

void mdea_boolean_node_destroy(void *p)
{
}

int mdea_boolean_node_serialize(void *p, struct mdea_emitter *e, int indent, wchar_t **error)
{
	struct mdea_boolean_node *n = p;
	if (mdea_emitter_emit_string(e, n->boolean ? L"true" : L"false", error) != 0)
		return -1;
	return 0;
}

int mdea_boolean_node_get_number(void *p, double *number, wchar_t **error)
{
	mdea_error(error, L"Expected number, got boolean");
	return -1;
}

int mdea_boolean_node_get_string(void *p, const wchar_t **string, wchar_t **error)
{
	mdea_error(error, L"Expected string, got boolean");
	return -1;
}

int mdea_boolean_node_get_boolean(void *p, int *boolean, wchar_t **error)
{
	struct mdea_boolean_node *n = p;
	*boolean = n->boolean;
	return 0;
}

int mdea_boolean_node_get_array(void *p, struct mdea_array **array, wchar_t **error)
{
	mdea_error(error, L"Expected array, got boolean");
	return -1;
}

int mdea_boolean_node_get_object(void *p, struct mdea_object **object, wchar_t **error)
{
	mdea_error(error, L"Expected object, got boolean");
	return -1;
}

const struct mdea_node_type mdea_boolean_node_type = {
	mdea_boolean_node_destroy,
	mdea_boolean_node_serialize,
	mdea_boolean_node_get_number,
	mdea_boolean_node_get_string,
	mdea_boolean_node_get_boolean,
	mdea_boolean_node_get_array,
	mdea_boolean_node_get_object,
};

struct mdea_node *mdea_boolean(int boolean)
{
	struct mdea_boolean_node *n = calloc(1, sizeof(*n));
	n->type = &mdea_boolean_node_type;
	n->boolean = boolean;
	return (void *)n;
}

