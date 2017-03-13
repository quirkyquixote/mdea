/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include <stdio.h>
#include <wchar.h>

#include "mdea.h"

void print_array(struct mdea_array *array)
{
	struct mdea_node *iter;
	const char *string;
	printf("%zu elements:\n", array->size);
	mdea_array_foreach(iter, array) {
		mdea_get_string(iter, &string, NULL);
		printf("%s\n", string);
	}
}

int main(int argc, char *argv[])
{
	struct mdea_array array;
	char line[256];

	mdea_array_init(&array);

	while (fscanf(stdin, "%s", line) == 1) {
		if (line[0] == '+') {
			mdea_array_push_back(&array, mdea_string_node(line + 1));
		} else if (line[0] == '-') {
			long key = strtol(line + 1, NULL, 10);
			mdea_array_erase(&array, key);
		} else if (strcmp(line, "x") == 0) {
			mdea_array_clear(&array);
		} else {
			fprintf(stderr, "ERROR: bad command: %s\n", line);
			return -1;
		}
		print_array(&array);
	}

	mdea_array_deinit(&array);
}

