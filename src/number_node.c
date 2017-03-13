/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "number_node.h"

struct mdea_number_node {
	const struct mdea_node_type *type;
	double number;
};

void mdea_number_node_destroy(void *p)
{
}

int mdea_number_node_serialize(void *p, struct mdea_emitter *e, int indent, char **error)
{
	struct mdea_number_node *n = p;
	if (mdea_emitter_emit(e, mdea_number_token(n->number), error) != 0)
		return -1;
	return 0;
}

int mdea_number_node_get_number(void *p, double *number, char **error)
{
	struct mdea_number_node *n = p;
	*number = n->number;
	return 0;
}

int mdea_number_node_get_string(void *p, const char **string, char **error)
{
	mdea_error(error, "Expected string, got number");
	return -1;
}

int mdea_number_node_get_boolean(void *p, int *boolean, char **error)
{
	mdea_error(error, "Expected boolean, got number");
	return -1;
}

int mdea_number_node_get_array(void *p, struct mdea_array **array, char **error)
{
	mdea_error(error, "Expected array, got number");
	return -1;
}

int mdea_number_node_get_object(void *p, struct mdea_object **object, char **error)
{
	mdea_error(error, "Expected object, got number");
	return -1;
}

const struct mdea_node_type mdea_number_node_type = {
	mdea_number_node_destroy,
	mdea_number_node_serialize,
	mdea_number_node_get_number,
	mdea_number_node_get_string,
	mdea_number_node_get_boolean,
	mdea_number_node_get_array,
	mdea_number_node_get_object,
};

struct mdea_node *mdea_number_node(double number)
{
	struct mdea_number_node *n = calloc(1, sizeof(*n));
	n->type = &mdea_number_node_type;
	n->number = number;
	return (void *)n;
}

