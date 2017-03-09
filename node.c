/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "node.h"

void mdea_destroy(void *ptr)
{
	struct mdea_node *node = ptr;
	node->type->destroy(node);
	free(node);
}

