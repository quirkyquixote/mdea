/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "node_parser.h"

#include <wctype.h>

#include "error.h"

struct mdea_node_parser {
	const struct mdea_parser_type *type;
	struct mdea_node *node;
};

void mdea_node_parser_destroy(void *p)
{
	struct mdea_node_parser *t = p;
}

int mdea_node_parser_parse(void *p, struct mdea_emitter *e, wchar_t **error)
{
	struct mdea_node_parser *t = p;
	return mdea_serialize(t->node, e, 0, error);
}

const struct mdea_parser_type mdea_node_parser_type = {
	mdea_node_parser_destroy,
	mdea_node_parser_parse,
};

struct mdea_parser *mdea_node_parser(struct mdea_node *node)
{
	struct mdea_node_parser *t = calloc(1, sizeof(*t));
	t->type = &mdea_node_parser_type;
	t->node = node;
	return (void *)t;
}
