/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "node.h"

struct mdea_null_node {
	const struct mdea_node_type *type;
};

struct mdea_number_node {
	const struct mdea_node_type *type;
	double number;
};

struct mdea_string_node {
	const struct mdea_node_type *type;
	wchar_t *string;
};

struct mdea_boolean_node {
	const struct mdea_node_type *type;
	int boolean;
};

struct mdea_array_node {
	const struct mdea_node_type *type;
	struct mdea_array array;
};

struct mdea_object_node {
	const struct mdea_node_type *type;
	struct mdea_object object;
};

void mdea_destroy(void *ptr)
{
	struct mdea_node *node = ptr;
	node->type->destroy(node);
	free(node);
}

void mdea_node_destroy(void *p)
{
}

void mdea_string_node_destroy(void *p)
{
	struct mdea_string_node *n = p;
	free(n->string);
}

void mdea_array_node_destroy(void *p)
{
	struct mdea_array_node *n = p;
	mdea_array_deinit(&n->array);
}

void mdea_object_node_destroy(void *p)
{
	struct mdea_object_node *n = p;
	mdea_object_deinit(&n->object);
}

int mdea_node_get_number(void *p, double *number, wchar_t **error)
{
	mdea_error(error, L"Not a number");
	return -1;
}

int mdea_node_get_string(void *p, const wchar_t **string, wchar_t **error)
{
	mdea_error(error, L"Not a string");
	return -1;
}

int mdea_node_get_boolean(void *p, int *boolean, wchar_t **error)
{
	mdea_error(error, L"Not a boolean");
	return -1;
}

int mdea_node_get_array(void *p, struct mdea_array **array, wchar_t **error)
{
	mdea_error(error, L"Not an array");
	return -1;
}

int mdea_node_get_object(void *p, struct mdea_object **object, wchar_t **error)
{
	mdea_error(error, L"Not an object");
	return -1;
}

int mdea_number_node_get_number(void *p, double *number, wchar_t **error)
{
	struct mdea_number_node *n = p;
	*number = n->number;
	return 0;
}

int mdea_string_node_get_string(void *p, const wchar_t **string, wchar_t **error)
{
	struct mdea_string_node *n = p;
	*string = n->string;
	return 0;
}

int mdea_boolean_node_get_boolean(void *p, int *boolean, wchar_t **error)
{
	struct mdea_boolean_node *n = p;
	*boolean = n->boolean;
	return 0;
}

int mdea_array_node_get_array(void *p, struct mdea_array **array, wchar_t **error)
{
	struct mdea_array_node *n = p;
	*array = &n->array;
	return 0;
}

int mdea_object_node_get_object(void *p, struct mdea_object **object, wchar_t **error)
{
	struct mdea_object_node *n = p;
	*object = &n->object;
	return 0;
}

int mdea_null_node_serialize(void *p, struct mdea_emitter *f, int indent, wchar_t **error)
{
	if (mdea_emitter_emit_string(f, L"null", error) != 0)
		return -1;
	return 0;
}

int mdea_number_node_serialize(void *p, struct mdea_emitter *f, int indent, wchar_t **error)
{
	double number;
	mdea_get_number(p, &number, error);
	if (mdea_emitter_emit_number(f, number, error) != 0)
		return -1;
	return 0;
}

int mdea_string_node_serialize(void *p, struct mdea_emitter *f, int indent, wchar_t **error)
{
	const wchar_t *string;
	mdea_get_string(p, &string, error);
	if (mdea_emitter_emit_string(f, L"\"", error) != 0)
		return -1;
	if (mdea_emitter_emit_string(f, string, error) != 0)
		return -1;
	if (mdea_emitter_emit_string(f, L"\"", error) != 0)
		return -1;
	return 0;
}

int mdea_boolean_node_serialize(void *p, struct mdea_emitter *f, int indent, wchar_t **error)
{
	int boolean;
	mdea_get_boolean(p, &boolean, error);
	if (mdea_emitter_emit_string(f, boolean ? L"true" : L"false", error) != 0)
		return -1;
	return 0;
}

int mdea_array_node_serialize(void *p, struct mdea_emitter *f, int indent, wchar_t **error)
{
	struct mdea_array *array;
	mdea_get_array(p, &array, NULL);
	if (array->size == 0) {
		if (mdea_emitter_emit_string(f, L"[]", error) != 0)
			return -1;
		return 0;
	}
	int is_first = 1;
	if (mdea_emitter_emit_string(f, L"[\n", error) != 0)
		return -1;
	for (int i = 0; i < array->size; ++i) {
		if (is_first) {
			is_first = 0;
		} else {
			if (mdea_emitter_emit_string(f, L",\n", error) != 0)
				return -1;
		}
		for (int i = 0; i <= indent; ++i) {
			if (mdea_emitter_emit_string(f, L"  ", error) != 0)
				return -1;
		}
		mdea_serialize(array->vals[i], f, indent + 1, error);
	}
	if (mdea_emitter_emit_string(f, L"\n", error) != 0)
		return -1;
	for (int i = 0; i < indent; ++i) {
		if (mdea_emitter_emit_string(f, L"  ", error) != 0)
			return -1;
	}
	if (mdea_emitter_emit_string(f, L"]", error) != 0)
		return -1;
	return 0;
}

int mdea_object_node_serialize(void *p, struct mdea_emitter *f, int indent, wchar_t **error)
{
	struct mdea_object *object;
	mdea_get_object(p, &object, NULL);
	if (object->size == 0) {
		if (mdea_emitter_emit_string(f, L"{}", error) != 0)
			return -1;
		return 0;
	}
	int is_first = 1;
	if (mdea_emitter_emit_string(f, L"{\n", error) != 0)
		return -1;
	for (int i = 0; i < object->size; ++i) {
		if (is_first) {
			is_first = 0;
		} else {
			if (mdea_emitter_emit_string(f, L",\n", error) != 0)
				return -1;
		}
		for (int i = 0; i <= indent; ++i) {
			if (mdea_emitter_emit_string(f, L"  ", error) != 0)
				return -1;
		}
		if (mdea_emitter_emit_string(f, L"\"", error) != 0)
			return -1;
		if (mdea_emitter_emit_string(f, object->fields[i].key, error) != 0)
			return -1;
		if (mdea_emitter_emit_string(f, L"\": ", error) != 0)
			return -1;
		mdea_serialize(object->fields[i].val, f, indent + 1, error);
	}
	if (mdea_emitter_emit_string(f, L"\n", error) != 0)
		return -1;
	for (int i = 0; i < indent; ++i) {
		if (mdea_emitter_emit_string(f, L"  ", error) != 0)
			return -1;
	}
	if (mdea_emitter_emit_string(f, L"}", error) != 0)
		return -1;
	return 0;
}

const struct mdea_node_type mdea_null_node_type = {
	mdea_node_destroy,
	mdea_null_node_serialize,
	mdea_node_get_number,
	mdea_node_get_string,
	mdea_node_get_boolean,
	mdea_node_get_array,
	mdea_node_get_object,
};

const struct mdea_node_type mdea_number_node_type = {
	mdea_node_destroy,
	mdea_number_node_serialize,
	mdea_number_node_get_number,
	mdea_node_get_string,
	mdea_node_get_boolean,
	mdea_node_get_array,
	mdea_node_get_object,
};

const struct mdea_node_type mdea_string_node_type = {
	mdea_string_node_destroy,
	mdea_string_node_serialize,
	mdea_node_get_number,
	mdea_string_node_get_string,
	mdea_node_get_boolean,
	mdea_node_get_array,
	mdea_node_get_object,
};

const struct mdea_node_type mdea_boolean_node_type = {
	mdea_node_destroy,
	mdea_boolean_node_serialize,
	mdea_node_get_number,
	mdea_node_get_string,
	mdea_boolean_node_get_boolean,
	mdea_node_get_array,
	mdea_node_get_object,
};

const struct mdea_node_type mdea_array_node_type = {
	mdea_array_node_destroy,
	mdea_array_node_serialize,
	mdea_node_get_number,
	mdea_node_get_string,
	mdea_node_get_boolean,
	mdea_array_node_get_array,
	mdea_node_get_object,
};

const struct mdea_node_type mdea_object_node_type = {
	mdea_object_node_destroy,
	mdea_object_node_serialize,
	mdea_node_get_number,
	mdea_node_get_string,
	mdea_node_get_boolean,
	mdea_node_get_array,
	mdea_object_node_get_object,
};

struct mdea_node *mdea_null(void)
{
	struct mdea_null_node *n = calloc(1, sizeof(*n));
	n->type = &mdea_null_node_type;
	return (void *)n;
}

struct mdea_node *mdea_number(double number)
{
	struct mdea_number_node *n = calloc(1, sizeof(*n));
	n->type = &mdea_number_node_type;
	n->number = number;
	return (void *)n;
}

struct mdea_node *mdea_string(const wchar_t *string)
{
	struct mdea_string_node *n = calloc(1, sizeof(*n));
	n->type = &mdea_string_node_type;
	n->string = wcsdup(string);
	return (void *)n;
}

struct mdea_node *mdea_boolean(int boolean)
{
	struct mdea_boolean_node *n = calloc(1, sizeof(*n));
	n->type = &mdea_boolean_node_type;
	n->boolean = boolean;
	return (void *)n;
}

struct mdea_node *mdea_array(void)
{
	struct mdea_array_node *n = calloc(1, sizeof(*n));
	n->type = &mdea_array_node_type;
	mdea_array_init(&n->array);
	return (void *)n;
}

struct mdea_node *mdea_object(void)
{
	struct mdea_object_node *n = calloc(1, sizeof(*n));
	n->type = &mdea_object_node_type;
	mdea_object_init(&n->object);
	return (void *)n;
}

