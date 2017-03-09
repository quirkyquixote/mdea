/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "string_node.h"

struct mdea_string_node {
	const struct mdea_node_type *type;
	wchar_t *string;
};

void mdea_string_node_destroy(void *p)
{
	struct mdea_string_node *n = p;
	free(n->string);
}

int mdea_string_node_serialize(void *p, struct mdea_emitter *e, int indent, wchar_t **error)
{
	struct mdea_string_node *n = p;
	if (mdea_emitter_emit_string(e, L"\"", error) != 0)
		return -1;
	if (mdea_emitter_emit_string(e, n->string, error) != 0)
		return -1;
	if (mdea_emitter_emit_string(e, L"\"", error) != 0)
		return -1;
	return 0;
}

int mdea_string_node_get_number(void *p, double *number, wchar_t **error)
{
	mdea_error(error, L"Expected number, got string");
	return -1;
}

int mdea_string_node_get_string(void *p, const wchar_t **string, wchar_t **error)
{
	struct mdea_string_node *n = p;
	*string = n->string;
	return 0;
}

int mdea_string_node_get_boolean(void *p, int *boolean, wchar_t **error)
{
	mdea_error(error, L"Expected boolean, got string");
	return -1;
}

int mdea_string_node_get_array(void *p, struct mdea_array **array, wchar_t **error)
{
	mdea_error(error, L"Expected array, got string");
	return -1;
}

int mdea_string_node_get_object(void *p, struct mdea_object **object, wchar_t **error)
{
	mdea_error(error, L"Expected object, got string");
	return -1;
}

const struct mdea_node_type mdea_string_node_type = {
	mdea_string_node_destroy,
	mdea_string_node_serialize,
	mdea_string_node_get_number,
	mdea_string_node_get_string,
	mdea_string_node_get_boolean,
	mdea_string_node_get_array,
	mdea_string_node_get_object,
};

struct mdea_node *mdea_string_node(const wchar_t *string)
{
	struct mdea_string_node *n = calloc(1, sizeof(*n));
	n->type = &mdea_string_node_type;
	n->string = wcsdup(string);
	return (void *)n;
}

