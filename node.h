/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_NODE_H_
#define MDEA_NODE_H_

#include "error.h"
#include "array.h"
#include "object.h"

struct mdea_node_type {
	void (*destroy)(void *);
	int (*serialize)(void *, FILE *, int indent, wchar_t **);
	int (*get_number)(void *, double *, wchar_t **);
	int (*get_string)(void *, const wchar_t **, wchar_t **);
	int (*get_boolean)(void *, int *, wchar_t **);
	int (*get_array)(void *, struct mdea_array **, wchar_t **);
	int (*get_object)(void *, struct mdea_object **, wchar_t **);
};

struct mdea_node {
	const struct mdea_node_type *type;
};

extern const struct mdea_node_type mdea_null_node_type;
extern const struct mdea_node_type mdea_number_node_type;
extern const struct mdea_node_type mdea_string_node_type;
extern const struct mdea_node_type mdea_boolean_node_type;
extern const struct mdea_node_type mdea_array_node_type;
extern const struct mdea_node_type mdea_object_node_type;

struct mdea_node *mdea_null(void);
struct mdea_node *mdea_number(double number);
struct mdea_node *mdea_string(const wchar_t *string);
struct mdea_node *mdea_boolean(int boolean);
struct mdea_node *mdea_array(void);
struct mdea_node *mdea_object(void);

static inline int mdea_is_null(struct mdea_node *node)
{
	return node->type == &mdea_null_node_type;
}

static inline int mdea_is_number(struct mdea_node *node)
{
	return node->type == &mdea_number_node_type;
}

static inline int mdea_is_string(struct mdea_node *node)
{
	return node->type == &mdea_string_node_type;
}

static inline int mdea_is_boolean(struct mdea_node *node)
{
	return node->type == &mdea_boolean_node_type;
}

static inline int mdea_is_array(struct mdea_node *node)
{
	return node->type == &mdea_array_node_type;
}

static inline int mdea_is_object(struct mdea_node *node)
{
	return node->type == &mdea_object_node_type;
}

static inline int mdea_serialize(struct mdea_node *node, FILE *file, int indent, wchar_t **error)
{
	return node->type->serialize(node, file, indent, error);
}

static inline int mdea_get_number(struct mdea_node *node, double *number, wchar_t **error)
{
	return node->type->get_number(node, number, error);
}

static inline int mdea_get_string(struct mdea_node *node, const wchar_t **string, wchar_t **error)
{
	return node->type->get_string(node, string, error);
}

static inline int mdea_get_boolean(struct mdea_node *node, int *boolean, wchar_t **error)
{
	return node->type->get_boolean(node, boolean, error);
}

static inline int mdea_get_array(struct mdea_node *node, struct mdea_array **array, wchar_t **error)
{
	return node->type->get_array(node, array, error);
}

static inline int mdea_get_object(struct mdea_node *node, struct mdea_object **object, wchar_t **error)
{
	return node->type->get_object(node, object, error);
}

#endif  // MDEA_NODE_H_

