/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef MDEA_H_
#define MDEA_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "error.h"
#include "escape.h"
#include "null_node.h"
#include "number_node.h"
#include "string_node.h"
#include "boolean_node.h"
#include "array_node.h"
#include "object_node.h"
#include "json_parser.h"
#include "node_parser.h"
#include "json_emitter.h"
#include "node_emitter.h"
#include "file_input.h"
#include "string_input.h"
#include "file_output.h"
#include "string_output.h"

/*
 * Get and set nodes in a tree
 *
 * The key must be a sequence of tokens that determine the field of an object
 * or entry of an array that will be accessed:
 *
 *   .fieldname
 *   [arrayindex]
 *
 * For example:
 *
 *   .foo.bar
 *   [1][5]
 *   .foo[30]
 */
int mdea_get(struct mdea_node *root, char *key, struct mdea_node **rval, char **error);
int mdea_set(struct mdea_node **root, char *key, struct mdea_node *val, char **error);

/* Parse node tree from file descriptor */
static inline int mdea_node_from_file_desc(int file, struct mdea_node **node, char **error)
{
	struct mdea_input *i = mdea_file_input(file);
	struct mdea_parser *p = mdea_json_parser(i);
	struct mdea_emitter *e = mdea_node_emitter(node);
	int ret = mdea_parse(p, e, error);
	mdea_parser_destroy(p);
	mdea_emitter_destroy(e);
	mdea_input_destroy(i);
	return ret;
}

/* Parse node tree from file path */
static inline int mdea_node_from_file_path(const char *path, struct mdea_node **node, char **error)
{
	int file = open(path, O_RDONLY);
	if (file < 0) {
		mdea_error(error, "%s: %s", path, strerror(errno));
		return -1;
	}
	int ret = mdea_node_from_file_desc(file, node, error);
	close(file);
	return ret;
}

/* Parse node tree from string */
static inline int mdea_node_from_string(const char *str, struct mdea_node **node, char **error)
{
	struct mdea_input *i = mdea_string_input(str);
	struct mdea_parser *p = mdea_json_parser(i);
	struct mdea_emitter *e = mdea_node_emitter(node);
	int ret = mdea_parse(p, e, error);
	mdea_parser_destroy(p);
	mdea_emitter_destroy(e);
	mdea_input_destroy(i);
	return ret;
}

/* Write node tree to file descriptor */
static inline int mdea_node_to_file_desc(struct mdea_node *node, int file, char **error)
{
	struct mdea_parser *p = mdea_node_parser(node);
	struct mdea_output *o = mdea_file_output(file);
	struct mdea_emitter *e = mdea_json_emitter(o);
	int ret = mdea_parse(p, e, error);
	mdea_parser_destroy(p);
	mdea_emitter_destroy(e);
	mdea_output_destroy(o);
	return ret;
}

/* Write node tree to file path */
static inline int mdea_node_to_file_path(struct mdea_node *node, const char *path, char **error)
{
	int file = creat(path, 0777);
	if (file < 0) {
		mdea_error(error, "%s: %s", path, strerror(errno));
		return -1;
	}
	int ret = mdea_node_to_file_desc(node, file, error);
	close(file);
	return ret;
}

/* Write node tree to string */
static inline int mdea_node_to_string(struct mdea_node *node, char **str, size_t *len, char **error)
{
	struct mdea_parser *p = mdea_node_parser(node);
	struct mdea_output *o = mdea_string_output(str, len);
	struct mdea_emitter *e = mdea_json_emitter(o);
	int ret = mdea_parse(p, e, error);
	mdea_parser_destroy(p);
	mdea_emitter_destroy(e);
	mdea_output_destroy(o);
	return ret;
}

#endif  // MDEA_H_

