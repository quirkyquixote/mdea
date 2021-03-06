/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "node_emitter.h"

#include "mdea.h"

struct mdea_node_emitter {
	const struct mdea_emitter_type *type;
	struct mdea_array stack;
	char *key;
	struct mdea_node *node;
	struct mdea_node **root;
	int state;
};

void mdea_node_emitter_destroy(void *p)
{
	struct mdea_node_emitter *e = p;
	if (e->key)
		free(e->key);
	mdea_array_deinit(&e->stack);
}

int mdea_node_emitter_emit(void *p, struct mdea_token tok, char **error)
{
	struct mdea_node_emitter *e = p;

	if (e->state == 0) {
		if (tok.type == MDEA_TOK_NULL) {
			*e->root = e->node = mdea_null_node();
			e->state = 1;
		} else if (tok.type == MDEA_TOK_TRUE) {
			*e->root = e->node = mdea_boolean_node(1);
			e->state = 1;
		} else if (tok.type == MDEA_TOK_FALSE) {
			*e->root = e->node = mdea_boolean_node(0);
			e->state = 1;
		} else if (tok.type == MDEA_TOK_NUMBER) {
			*e->root = e->node = mdea_number_node(tok.number);
			e->state = 1;
		} else if (tok.type == MDEA_TOK_STRING) {
			*e->root = e->node = mdea_string_node(tok.string);
			e->state = 1;
		} else if (tok.type == MDEA_TOK_LBRACKET) {
			*e->root = e->node = mdea_array_node();
			e->state = 2;
		} else if (tok.type == MDEA_TOK_LCURLY) {
			*e->root = e->node = mdea_object_node();
			e->state = 4;
		} else {
			mdea_error(error, "Expected value at root");
			return -1;
		}
		mdea_ref(e->node);
	} else if (e->state == 1) {
		if (tok.type != MDEA_TOK_END) {
			mdea_error(error, "Expected end of stream");
			return -1;
		}
	} else if (e->state == 2) {
		struct mdea_array *array;
		mdea_get_array(e->node, &array, NULL);
		if (tok.type == MDEA_TOK_NULL) {
			mdea_array_push_back(array, mdea_null_node());
			e->state = 3;
		} else if (tok.type == MDEA_TOK_TRUE) {
			mdea_array_push_back(array, mdea_boolean_node(1));
			e->state = 3;
		} else if (tok.type == MDEA_TOK_FALSE) {
			mdea_array_push_back(array, mdea_boolean_node(0));
			e->state = 3;
		} else if (tok.type == MDEA_TOK_NUMBER) {
			mdea_array_push_back(array, mdea_number_node(tok.number));
			e->state = 3;
		} else if (tok.type == MDEA_TOK_STRING) {
			mdea_array_push_back(array, mdea_string_node(tok.string));
			e->state = 3;
		} else if (tok.type == MDEA_TOK_LBRACKET) {
			mdea_array_push_back(&e->stack, e->node);
			mdea_unref(e->node);
			e->node = mdea_array_node();
			mdea_array_push_back(array, e->node);
			e->state = 2;
		} else if (tok.type == MDEA_TOK_LCURLY) {
			mdea_array_push_back(&e->stack, e->node);
			mdea_unref(e->node);
			e->node = mdea_object_node();
			mdea_array_push_back(array, e->node);
			e->state = 4;
		} else if (mdea_array_empty(array)) {
			if (tok.type == MDEA_TOK_RBRACKET)
				goto pop_stack;
			mdea_error(error, "Expected value or end of array");
			return -1;
		} else {
			mdea_error(error, "Expected value in array");
			return -1;
		}
	} else if (e->state == 3) {
		if (tok.type == MDEA_TOK_COMMA) {
			e->state = 2;
		} else if (tok.type == MDEA_TOK_RBRACKET) {
			goto pop_stack;
		} else {
			mdea_error(error, "Expected comma or end of array");
			return -1;
		}
	} else if (e->state == 4) {
		struct mdea_object *object;
		mdea_get_object(e->node, &object, NULL);
		if (tok.type == MDEA_TOK_STRING) {
			e->key = strdup(tok.string);
			e->state = 5;
		} else if (mdea_object_empty(object)) {
			if (tok.type == MDEA_TOK_RCURLY)
				goto pop_stack;
			mdea_error(error, "Expected key or end of object");
			return -1;
		} else {
			return -1;
			mdea_error(error, "Expected key in object");
		}
	} else if (e->state == 5) {
		if (tok.type == MDEA_TOK_COLON) {
			e->state = 6;
		} else {
			return -1;
			mdea_error(error, "Expected colon after key");
		}
	} else if (e->state == 6) {
		struct mdea_object *object;
		mdea_get_object(e->node, &object, NULL);
		if (tok.type == MDEA_TOK_NULL) {
			mdea_object_insert(object, e->key, mdea_null_node());
			e->state = 7;
		} else if (tok.type == MDEA_TOK_TRUE) {
			mdea_object_insert(object, e->key, mdea_boolean_node(1));
			e->state = 7;
		} else if (tok.type == MDEA_TOK_FALSE) {
			mdea_object_insert(object, e->key, mdea_boolean_node(0));
			e->state = 7;
		} else if (tok.type == MDEA_TOK_NUMBER) {
			mdea_object_insert(object, e->key, mdea_number_node(tok.number));
			e->state = 7;
		} else if (tok.type == MDEA_TOK_STRING) {
			mdea_object_insert(object, e->key, mdea_string_node(tok.string));
			e->state = 7;
		} else if (tok.type == MDEA_TOK_LBRACKET) {
			mdea_array_push_back(&e->stack, e->node);
			mdea_unref(e->node);
			e->node = mdea_array_node();
			mdea_object_insert(object, e->key, e->node);
			e->state = 2;
		} else if (tok.type == MDEA_TOK_LCURLY) {
			mdea_array_push_back(&e->stack, e->node);
			mdea_unref(e->node);
			e->node = mdea_object_node();
			mdea_object_insert(object, e->key, e->node);
			e->state = 4;
		} else {
			return -1;
			mdea_error(error, "Expected value after colon");
		}
		free(e->key);
		e->key = NULL;
	} else if (e->state == 7) {
		if (tok.type == MDEA_TOK_COMMA) {
			e->state = 4;
		} else if (tok.type == MDEA_TOK_RCURLY) {
			goto pop_stack;
		} else {
			mdea_error(error, "Expected comma or end of object");
			return -1;
		}
	} else {
		mdea_error(error, "FATAL: Corrupt state machine");
		return -1;
	}
	return 0;
pop_stack:
	if (mdea_array_empty(&e->stack)) {
		e->node = NULL;
		e->state = 1;
	} else {
		mdea_array_get_back(&e->stack, &e->node);
		mdea_array_pop_back(&e->stack);
		if (mdea_get_array(e->node, NULL, NULL) == 0)
			e->state = 3;
		else if (mdea_get_object(e->node, NULL, NULL) == 0)
			e->state = 7;
		else
			return -1;
	}
	return 0;
}

static const struct mdea_emitter_type mdea_node_emitter_type = {
	mdea_node_emitter_destroy,
	mdea_node_emitter_emit,
};

struct mdea_emitter *mdea_node_emitter(struct mdea_node **root)
{
	struct mdea_node_emitter *e = calloc(1, sizeof(*e));
	e->type = &mdea_node_emitter_type;
	e->root = root;
	mdea_array_init(&e->stack);
	return (void *)e;
}

