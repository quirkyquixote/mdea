/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "mdea.h"

void print_object(struct mdea_object *object)
{
	struct mdea_node *val;
	char *key;
	const char *string;

	printf("%zu elements:\n", object->size);
	mdea_object_foreach(key, val, object) {
		mdea_get_string(val, &string, NULL);
		printf("%s: %s\n", key, string);
	}
}

int main(int argc, char *argv[])
{
	struct mdea_object object;
	char line[256];

	mdea_object_init(&object);

	while (fscanf(stdin, "%s", line) == 1) {
		if (line[0] == '+') {
			mdea_object_insert(&object, line + 1, mdea_string_node(line + 1));
		} else if (line[0] == '-') {
			mdea_object_erase(&object, line + 1);
		} else if (strcmp(line, "x") == 0) {
			mdea_object_clear(&object);
		} else {
			fprintf(stderr, "ERROR: bad command: %s\n", line);
			return -1;
		}
		print_object(&object);
	}

	mdea_object_deinit(&object);
}

