/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "node.h"

void mdea_destroy(struct mdea_node *node)
{
	node->type->destroy(node);
	free(node);
}

