/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "node.h"

struct MdeaNullNode {
	const struct MdeaNodeType *type;
};

struct MdeaNumberNode {
	const struct MdeaNodeType *type;
	double number;
};

struct MdeaStringNode {
	const struct MdeaNodeType *type;
	wchar_t *string;
};

struct MdeaBooleanNode {
	const struct MdeaNodeType *type;
	int boolean;
};

struct MdeaArrayNode {
	const struct MdeaNodeType *type;
	struct MdeaArray array;
};

struct MdeaObjectNode {
	const struct MdeaNodeType *type;
	struct MdeaObject object;
};

void mdea_destroy(void *ptr)
{
	struct MdeaNode *node = ptr;
	node->type->destroy(node);
	free(node);
}

void mdea_node_destroy(void *p)
{
}

void mdea_string_node_destroy(void *p)
{
	struct MdeaStringNode *n = p;
	free(n->string);
}

void mdea_array_node_destroy(void *p)
{
	struct MdeaArrayNode *n = p;
	mdea_array_deinit(&n->array);
}

void mdea_object_node_destroy(void *p)
{
	struct MdeaObjectNode *n = p;
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

int mdea_node_get_array(void *p, struct MdeaArray **array, wchar_t **error)
{
	mdea_error(error, L"Not an array");
	return -1;
}

int mdea_node_get_object(void *p, struct MdeaObject **object, wchar_t **error)
{
	mdea_error(error, L"Not an object");
	return -1;
}

int mdea_number_node_get_number(void *p, double *number, wchar_t **error)
{
	struct MdeaNumberNode *n = p;
	*number = n->number;
	return 0;
}

int mdea_string_node_get_string(void *p, const wchar_t **string, wchar_t **error)
{
	struct MdeaStringNode *n = p;
	*string = n->string;
	return 0;
}

int mdea_boolean_node_get_boolean(void *p, int *boolean, wchar_t **error)
{
	struct MdeaBooleanNode *n = p;
	*boolean = n->boolean;
	return 0;
}

int mdea_array_node_get_array(void *p, struct MdeaArray **array, wchar_t **error)
{
	struct MdeaArrayNode *n = p;
	*array = &n->array;
	return 0;
}

int mdea_object_node_get_object(void *p, struct MdeaObject **object, wchar_t **error)
{
	struct MdeaObjectNode *n = p;
	*object = &n->object;
	return 0;
}

int mdea_null_node_serialize(void *p, FILE *f, int indent, wchar_t **error)
{
	fwprintf(f, L"null");
	return 0;
}

int mdea_number_node_serialize(void *p, FILE *f, int indent, wchar_t **error)
{
	double number;
	mdea_get_number(p, &number, error);
	fwprintf(f, L"%g", number);
	return 0;
}

int mdea_string_node_serialize(void *p, FILE *f, int indent, wchar_t **error)
{
	const wchar_t *string;
	mdea_get_string(p, &string, error);
	fwprintf(f, L"\"%ls\"", string);
	return 0;
}

int mdea_boolean_node_serialize(void *p, FILE *f, int indent, wchar_t **error)
{
	int boolean;
	mdea_get_boolean(p, &boolean, error);
	fwprintf(f, L"%ls", boolean ? L"true" : L"false");
	return 0;
}

int mdea_array_node_serialize(void *p, FILE *f, int indent, wchar_t **error)
{
	struct MdeaArray *array;
	mdea_get_array(p, &array, NULL);
	if (array->size == 0) {
		fwprintf(f, L"[]");
		return 0;
	}
	int is_first = 1;
	fwprintf(f, L"[\n");
	for (int i = 0; i < array->size; ++i) {
		if (is_first)
			is_first = 0;
		else
			fwprintf(f, L",\n");
		for (int i = 0; i <= indent; ++i)
			fwprintf(f, L"  ");
		mdea_serialize(array->vals[i], f, indent + 1, error);
	}
	fwprintf(f, L"\n");
	for (int i = 0; i < indent; ++i)
		fwprintf(f, L"  ");
	fwprintf(f, L"]");
	return 0;
}

int mdea_object_node_serialize(void *p, FILE *f, int indent, wchar_t **error)
{
	struct MdeaObject *object;
	mdea_get_object(p, &object, NULL);
	if (object->size == 0) {
		fwprintf(f, L"{}");
		return 0;
	}
	int is_first = 1;
	fwprintf(f, L"{\n");
	for (int i = 0; i < object->size; ++i) {
		if (is_first)
			is_first = 0;
		else
			fwprintf(f, L",\n");
		for (int i = 0; i <= indent; ++i)
			fwprintf(f, L"  ");
		fwprintf(f, L"\"%ls\": ", object->fields[i].key);
		mdea_serialize(object->fields[i].val, f, indent + 1, error);
	}
	fwprintf(f, L"\n");
	for (int i = 0; i < indent; ++i)
		fwprintf(f, L"  ");
	fwprintf(f, L"}");
	return 0;
}

const struct MdeaNodeType mdea_null_node_type = {
	mdea_node_destroy,
	mdea_null_node_serialize,
	mdea_node_get_number,
	mdea_node_get_string,
	mdea_node_get_boolean,
	mdea_node_get_array,
	mdea_node_get_object,
};

const struct MdeaNodeType mdea_number_node_type = {
	mdea_node_destroy,
	mdea_number_node_serialize,
	mdea_number_node_get_number,
	mdea_node_get_string,
	mdea_node_get_boolean,
	mdea_node_get_array,
	mdea_node_get_object,
};

const struct MdeaNodeType mdea_string_node_type = {
	mdea_string_node_destroy,
	mdea_string_node_serialize,
	mdea_node_get_number,
	mdea_string_node_get_string,
	mdea_node_get_boolean,
	mdea_node_get_array,
	mdea_node_get_object,
};

const struct MdeaNodeType mdea_boolean_node_type = {
	mdea_node_destroy,
	mdea_boolean_node_serialize,
	mdea_node_get_number,
	mdea_node_get_string,
	mdea_boolean_node_get_boolean,
	mdea_node_get_array,
	mdea_node_get_object,
};

const struct MdeaNodeType mdea_array_node_type = {
	mdea_array_node_destroy,
	mdea_array_node_serialize,
	mdea_node_get_number,
	mdea_node_get_string,
	mdea_node_get_boolean,
	mdea_array_node_get_array,
	mdea_node_get_object,
};

const struct MdeaNodeType mdea_object_node_type = {
	mdea_object_node_destroy,
	mdea_object_node_serialize,
	mdea_node_get_number,
	mdea_node_get_string,
	mdea_node_get_boolean,
	mdea_node_get_array,
	mdea_object_node_get_object,
};

struct MdeaNode *mdea_null(void)
{
	struct MdeaNullNode *n = calloc(1, sizeof(*n));
	n->type = &mdea_null_node_type;
	return (void *)n;
}

struct MdeaNode *mdea_number(double number)
{
	struct MdeaNumberNode *n = calloc(1, sizeof(*n));
	n->type = &mdea_number_node_type;
	n->number = number;
	return (void *)n;
}

struct MdeaNode *mdea_string(const wchar_t *string)
{
	struct MdeaStringNode *n = calloc(1, sizeof(*n));
	n->type = &mdea_string_node_type;
	n->string = wcsdup(string);
	return (void *)n;
}

struct MdeaNode *mdea_boolean(int boolean)
{
	struct MdeaBooleanNode *n = calloc(1, sizeof(*n));
	n->type = &mdea_boolean_node_type;
	n->boolean = boolean;
	return (void *)n;
}

struct MdeaNode *mdea_array(void)
{
	struct MdeaArrayNode *n = calloc(1, sizeof(*n));
	n->type = &mdea_array_node_type;
	mdea_array_init(&n->array);
	return (void *)n;
}

struct MdeaNode *mdea_object(void)
{
	struct MdeaObjectNode *n = calloc(1, sizeof(*n));
	n->type = &mdea_object_node_type;
	mdea_object_init(&n->object);
	return (void *)n;
}

