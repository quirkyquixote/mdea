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
	int (*serialize)(void *, struct mdea_emitter *, int indent, char **);
	/* Cast to number */
	int (*get_number)(void *, double *, char **);
	/* Cast to string */
	int (*get_string)(void *, const char **, char **);
	/* Cast to boolean */
	int (*get_boolean)(void *, int *, char **);
	/* Cast to array */
	int (*get_array)(void *, struct mdea_array **, char **);
	/* Cast to object */
	int (*get_object)(void *, struct mdea_object **, char **);
};

/*
 * Exposed fields of all node types
 */
struct mdea_node {
	/* Pointer to type definition */
	const struct mdea_node_type *type;
};

/* Serialize a node */
static inline int mdea_serialize(struct mdea_node *node, struct mdea_emitter *e, int indent, char **error)
{
	return node->type->serialize(node, e, indent, error);
}

/* Extract number from node */
static inline int mdea_get_number(struct mdea_node *node, double *number, char **error)
{
	return node->type->get_number(node, number, error);
}

/* Extract string from node */
static inline int mdea_get_string(struct mdea_node *node, const char **string, char **error)
{
	return node->type->get_string(node, string, error);
}

/* Extract boolean from node */
static inline int mdea_get_boolean(struct mdea_node *node, int *boolean, char **error)
{
	return node->type->get_boolean(node, boolean, error);
}

/* Extract array from node */
static inline int mdea_get_array(struct mdea_node *node, struct mdea_array **array, char **error)
{
	return node->type->get_array(node, array, error);
}

/* Extract object from node */
static inline int mdea_get_object(struct mdea_node *node, struct mdea_object **object, char **error)
{
	return node->type->get_object(node, object, error);
}

/* Write node */
static inline int mdea_write(struct mdea_emitter *f, struct mdea_node *node, char **error)
{
	return mdea_serialize(node, f, 0, error);
}

#endif  // MDEA_NODE_H_

