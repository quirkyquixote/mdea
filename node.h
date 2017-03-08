/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_NODE_H_
#define MDEA_NODE_H_

#include "error.h"
#include "array.h"
#include "object.h"
#include "emitter.h"

/*
 * Type of a node
 */
struct mdea_node_type {
	/* Destroy contents (but not free) */
	void (*destroy)(void *);
	/* Serialize data */
	int (*serialize)(void *, struct mdea_emitter *, int indent, wchar_t **);
	/* Cast to number */
	int (*get_number)(void *, double *, wchar_t **);
	/* Cast to string */
	int (*get_string)(void *, const wchar_t **, wchar_t **);
	/* Cast to boolean */
	int (*get_boolean)(void *, int *, wchar_t **);
	/* Cast to array */
	int (*get_array)(void *, struct mdea_array **, wchar_t **);
	/* Cast to object */
	int (*get_object)(void *, struct mdea_object **, wchar_t **);
};

/*
 * Exposed fields of all node types
 */
struct mdea_node {
	/* Pointer to type definition */
	const struct mdea_node_type *type;
};

/* Create null node */
struct mdea_node *mdea_null(void);
/* Create number node */
struct mdea_node *mdea_number(double number);
/* Create string node */
struct mdea_node *mdea_string(const wchar_t *string);
/* Create boolean node */
struct mdea_node *mdea_boolean(int boolean);
/* Create array node */
struct mdea_node *mdea_array(void);
/* Create object node */
struct mdea_node *mdea_object(void);

/* Serialize a node */
static inline int mdea_serialize(struct mdea_node *node, struct mdea_emitter *e, int indent, wchar_t **error)
{
	return node->type->serialize(node, e, indent, error);
}

/* Extract number from node */
static inline int mdea_get_number(struct mdea_node *node, double *number, wchar_t **error)
{
	return node->type->get_number(node, number, error);
}

/* Extract string from node */
static inline int mdea_get_string(struct mdea_node *node, const wchar_t **string, wchar_t **error)
{
	return node->type->get_string(node, string, error);
}

/* Extract boolean from node */
static inline int mdea_get_boolean(struct mdea_node *node, int *boolean, wchar_t **error)
{
	return node->type->get_boolean(node, boolean, error);
}

/* Extract array from node */
static inline int mdea_get_array(struct mdea_node *node, struct mdea_array **array, wchar_t **error)
{
	return node->type->get_array(node, array, error);
}

/* Extract object from node */
static inline int mdea_get_object(struct mdea_node *node, struct mdea_object **object, wchar_t **error)
{
	return node->type->get_object(node, object, error);
}

/* Write node */
static inline int mdea_write(struct mdea_emitter *f, struct mdea_node *node, wchar_t **error)
{
	return mdea_serialize(node, f, 0, error);
}

#endif  // MDEA_NODE_H_

