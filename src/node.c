/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "node.h"

void mdea_ref(struct mdea_node *node)
{
	++node->ref_count;
}

void mdea_unref(struct mdea_node *node)
{
	if (--node->ref_count <= 0) {
		node->type->destroy(node);
		free(node);
	}
}

