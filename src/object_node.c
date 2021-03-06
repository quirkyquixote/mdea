/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "object_node.h"

struct mdea_object_node {
	const struct mdea_node_type *type;
	int ref_count;
	struct mdea_object object;
};

void mdea_object_node_destroy(void *p)
{
	struct mdea_object_node *n = p;
	mdea_object_deinit(&n->object);
}

int mdea_object_node_serialize(void *p, struct mdea_emitter *e, int indent, char **error)
{
	struct mdea_object *object;
	mdea_get_object(p, &object, NULL);
	int is_first = 1;
	if (mdea_emitter_emit(e, mdea_lcurly_token, error) != 0)
		return -1;
	for (int i = 0; i < object->size; ++i) {
		if (is_first)
			is_first = 0;
		else if (mdea_emitter_emit(e, mdea_comma_token, error) != 0)
			return -1;
		if (mdea_emitter_emit(e, mdea_string_token(object->fields[i].key), error) != 0)
			return -1;
		else if (mdea_emitter_emit(e, mdea_colon_token, error) != 0)
			return -1;
		mdea_serialize(object->fields[i].val, e, indent + 1, error);
	}
	if (mdea_emitter_emit(e, mdea_rcurly_token, error) != 0)
		return -1;
	return 0;
}

int mdea_object_node_get_number(void *p, double *number, char **error)
{
	mdea_error(error, "Expected number, got object");
	return -1;
}

int mdea_object_node_get_string(void *p, const char **string, char **error)
{
	mdea_error(error, "Expected string, got object");
	return -1;
}

int mdea_object_node_get_boolean(void *p, int *boolean, char **error)
{
	mdea_error(error, "Expected boolean, got object");
	return -1;
}

int mdea_object_node_get_array(void *p, struct mdea_array **array, char **error)
{
	mdea_error(error, "Expected array, got object");
	return -1;
}

int mdea_object_node_get_object(void *p, struct mdea_object **object, char **error)
{
	if (object != NULL) {
		struct mdea_object_node *n = p;
		*object = &n->object;
	}
	return 0;
}

const struct mdea_node_type mdea_object_node_type = {
	mdea_object_node_destroy,
	mdea_object_node_serialize,
	mdea_object_node_get_number,
	mdea_object_node_get_string,
	mdea_object_node_get_boolean,
	mdea_object_node_get_array,
	mdea_object_node_get_object,
};

struct mdea_node *mdea_object_node(void)
{
	struct mdea_object_node *n = calloc(1, sizeof(*n));
	n->type = &mdea_object_node_type;
	mdea_object_init(&n->object);
	return (void *)n;
}

