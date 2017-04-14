/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "null_node.h"

struct mdea_null_node {
	const struct mdea_node_type *type;
	int ref_count;
};

void mdea_null_node_destroy(void *p)
{
}

int mdea_null_node_serialize(void *p, struct mdea_emitter *e, int indent, char **error)
{
	if (mdea_emitter_emit(e, mdea_null_token, error) != 0)
		return -1;
	return 0;
}

int mdea_null_node_get_number(void *p, double *number, char **error)
{
	mdea_error(error, "Expected number, got null");
	return -1;
}

int mdea_null_node_get_string(void *p, const char **string, char **error)
{
	mdea_error(error, "Expected string, got null");
	return -1;
}

int mdea_null_node_get_boolean(void *p, int *boolean, char **error)
{
	mdea_error(error, "Expected boolean, got null");
	return -1;
}

int mdea_null_node_get_array(void *p, struct mdea_array **array, char **error)
{
	mdea_error(error, "Expected array, got null");
	return -1;
}

int mdea_null_node_get_object(void *p, struct mdea_object **object, char **error)
{
	mdea_error(error, "Expected object, got null");
	return -1;
}

const struct mdea_node_type mdea_null_node_type = {
	mdea_null_node_destroy,
	mdea_null_node_serialize,
	mdea_null_node_get_number,
	mdea_null_node_get_string,
	mdea_null_node_get_boolean,
	mdea_null_node_get_array,
	mdea_null_node_get_object,
};

struct mdea_node *mdea_null_node(void)
{
	struct mdea_null_node *n = calloc(1, sizeof(*n));
	n->type = &mdea_null_node_type;
	n->ref_count = 1;
	return (void *)n;
}

