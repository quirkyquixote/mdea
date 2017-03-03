/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_NODE_H_
#define MDEA_NODE_H_

#include "error.h"
#include "array.h"
#include "object.h"

struct MdeaNodeType {
	void (*destroy)(void *);
	int (*serialize)(void *, FILE *, int indent, wchar_t **);
	int (*get_number)(void *, double *, wchar_t **);
	int (*get_string)(void *, const wchar_t **, wchar_t **);
	int (*get_boolean)(void *, int *, wchar_t **);
	int (*get_array)(void *, struct MdeaArray **, wchar_t **);
	int (*get_object)(void *, struct MdeaObject **, wchar_t **);
};

struct MdeaNode {
	const struct MdeaNodeType *type;
};

extern const struct MdeaNodeType mdea_null_node_type;
extern const struct MdeaNodeType mdea_number_node_type;
extern const struct MdeaNodeType mdea_string_node_type;
extern const struct MdeaNodeType mdea_boolean_node_type;
extern const struct MdeaNodeType mdea_array_node_type;
extern const struct MdeaNodeType mdea_object_node_type;

struct MdeaNode *mdea_null(void);
struct MdeaNode *mdea_number(double number);
struct MdeaNode *mdea_string(const wchar_t *string);
struct MdeaNode *mdea_boolean(int boolean);
struct MdeaNode *mdea_array(void);
struct MdeaNode *mdea_object(void);

static inline int mdea_is_null(struct MdeaNode *node)
{
	return node->type == &mdea_null_node_type;
}

static inline int mdea_is_number(struct MdeaNode *node)
{
	return node->type == &mdea_number_node_type;
}

static inline int mdea_is_string(struct MdeaNode *node)
{
	return node->type == &mdea_string_node_type;
}

static inline int mdea_is_boolean(struct MdeaNode *node)
{
	return node->type == &mdea_boolean_node_type;
}

static inline int mdea_is_array(struct MdeaNode *node)
{
	return node->type == &mdea_array_node_type;
}

static inline int mdea_is_object(struct MdeaNode *node)
{
	return node->type == &mdea_object_node_type;
}

static inline int mdea_serialize(struct MdeaNode *node, FILE *file, int indent, wchar_t **error)
{
	return node->type->serialize(node, file, indent, error);
}

static inline int mdea_get_number(struct MdeaNode *node, double *number, wchar_t **error)
{
	return node->type->get_number(node, number, error);
}

static inline int mdea_get_string(struct MdeaNode *node, const wchar_t **string, wchar_t **error)
{
	return node->type->get_string(node, string, error);
}

static inline int mdea_get_boolean(struct MdeaNode *node, int *boolean, wchar_t **error)
{
	return node->type->get_boolean(node, boolean, error);
}

static inline int mdea_get_array(struct MdeaNode *node, struct MdeaArray **array, wchar_t **error)
{
	return node->type->get_array(node, array, error);
}

static inline int mdea_get_object(struct MdeaNode *node, struct MdeaObject **object, wchar_t **error)
{
	return node->type->get_object(node, object, error);
}

#endif  // MDEA_NODE_H_

