/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "node_emitter.h"

#include "mdea.h"

struct mdea_node_emitter {
	const struct mdea_emitter_type *type;
	struct mdea_array stack;
	wchar_t *key;
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

int mdea_node_emitter_emit(void *p, struct mdea_token tok, wchar_t **error)
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
			return -1;
		}
	} else if (e->state == 1) {
		if (tok.type != MDEA_TOK_END)
			return -1;
	} else if (e->state == 2) {
		if (tok.type == MDEA_TOK_NULL) {
			struct mdea_array *array;
			mdea_get_array(e->node, &array, NULL);
			mdea_array_push_back(array, mdea_null_node());
			e->state = 3;
		} else if (tok.type == MDEA_TOK_TRUE) {
			struct mdea_array *array;
			mdea_get_array(e->node, &array, NULL);
			mdea_array_push_back(array, mdea_boolean_node(1));
			e->state = 3;
		} else if (tok.type == MDEA_TOK_FALSE) {
			struct mdea_array *array;
			mdea_get_array(e->node, &array, NULL);
			mdea_array_push_back(array, mdea_boolean_node(0));
			e->state = 3;
		} else if (tok.type == MDEA_TOK_NUMBER) {
			struct mdea_array *array;
			mdea_get_array(e->node, &array, NULL);
			mdea_array_push_back(array, mdea_number_node(tok.number));
			e->state = 3;
		} else if (tok.type == MDEA_TOK_STRING) {
			struct mdea_array *array;
			mdea_get_array(e->node, &array, NULL);
			mdea_array_push_back(array, mdea_string_node(tok.string));
			e->state = 3;
		} else if (tok.type == MDEA_TOK_LBRACKET) {
			mdea_array_push_back(&e->stack, e->node);
			struct mdea_array *array;
			mdea_get_array(e->node, &array, NULL);
			e->node = mdea_array_node();
			mdea_array_push_back(array, e->node);
			e->state = 2;
		} else if (tok.type == MDEA_TOK_LCURLY) {
			mdea_array_push_back(&e->stack, e->node);
			struct mdea_array *array;
			mdea_get_array(e->node, &array, NULL);
			e->node = mdea_object_node();
			mdea_array_push_back(array, e->node);
			e->state = 4;
		} else if (tok.type == MDEA_TOK_RBRACKET) {
			goto pop_stack;
		} else {
			return -1;
		}
	} else if (e->state == 3) {
		if (tok.type == MDEA_TOK_COMMA) {
			e->state = 2;
		} else if (tok.type == MDEA_TOK_RBRACKET) {
			goto pop_stack;
		} else {
			return -1;
		}
	} else if (e->state == 4) {
		if (tok.type == MDEA_TOK_STRING) {
			e->key = wcsdup(tok.string);
			e->state = 5;
		} else if (tok.type == MDEA_TOK_RCURLY) {
			goto pop_stack;
		} else {
			return -1;
		}
	} else if (e->state == 5) {
		if (tok.type == MDEA_TOK_COLON) {
			e->state = 6;
		} else {
			return -1;
		}
	} else if (e->state == 6) {
		if (tok.type == MDEA_TOK_NULL) {
			struct mdea_object *object;
			mdea_get_object(e->node, &object, NULL);
			mdea_object_insert(object, e->key, mdea_null_node());
			free(e->key);
			e->state = 7;
		} else if (tok.type == MDEA_TOK_TRUE) {
			struct mdea_object *object;
			mdea_get_object(e->node, &object, NULL);
			mdea_object_insert(object, e->key, mdea_boolean_node(1));
			free(e->key);
			e->state = 7;
		} else if (tok.type == MDEA_TOK_FALSE) {
			struct mdea_object *object;
			mdea_get_object(e->node, &object, NULL);
			mdea_object_insert(object, e->key, mdea_boolean_node(0));
			free(e->key);
			e->state = 7;
		} else if (tok.type == MDEA_TOK_NUMBER) {
			struct mdea_object *object;
			mdea_get_object(e->node, &object, NULL);
			mdea_object_insert(object, e->key, mdea_number_node(tok.number));
			free(e->key);
			e->state = 7;
		} else if (tok.type == MDEA_TOK_STRING) {
			struct mdea_object *object;
			mdea_get_object(e->node, &object, NULL);
			mdea_object_insert(object, e->key, mdea_string_node(tok.string));
			free(e->key);
			e->state = 7;
		} else if (tok.type == MDEA_TOK_LBRACKET) {
			mdea_array_push_back(&e->stack, e->node);
			struct mdea_object *object;
			mdea_get_object(e->node, &object, NULL);
			e->node = mdea_array_node();
			mdea_object_insert(object, e->key, e->node);
			free(e->key);
			e->state = 2;
		} else if (tok.type == MDEA_TOK_LCURLY) {
			mdea_array_push_back(&e->stack, e->node);
			struct mdea_object *object;
			mdea_get_object(e->node, &object, NULL);
			e->node = mdea_object_node();
			mdea_object_insert(object, e->key, e->node);
			free(e->key);
			e->state = 4;
		} else {
			return -1;
		}
	} else if (e->state == 7) {
		if (tok.type == MDEA_TOK_COMMA) {
			e->state = 4;
		} else if (tok.type == MDEA_TOK_RBRACKET) {
			goto pop_stack;
		} else {
			return -1;
		}
	} else {
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
			e->state = 2;
		else if (mdea_get_object(e->node, NULL, NULL) == 0)
			e->state = 4;
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

