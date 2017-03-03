/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include <stdio.h>
#include <wchar.h>

#include "../array.h"

void mdea_destroy(void *ptr)
{
	free(ptr);
}

void print_array(struct MdeaArray *array)
{
	const wchar_t *iter;

	printf("%zu elements:\n", array->size);
	mdea_array_foreach(iter, array) {
		printf("%ls\n", iter);
	}
}

int main(int argc, char *argv[])
{
	struct MdeaArray array;
	char line[256];

	mdea_array_init(&array);

	while (fscanf(stdin, "%s", line) == 1) {
		if (line[0] == '+') {
			size_t len = strlen(line + 1) + 1;
			wchar_t tmp[len];
			mbstowcs(tmp, line + 1, len);
			mdea_array_add(&array, wcsdup(tmp));
		} else if (line[0] == '-') {
			long key = strtol(line + 1, NULL, 10);
			mdea_array_remove(&array, key);
		} else if (strcmp(line, "x") == 0) {
			mdea_array_deinit(&array);
			mdea_array_init(&array);
		} else {
			fprintf(stderr, "ERROR: bad command: %s\n", line);
			return -1;
		}
		print_array(&array);
	}

	mdea_array_deinit(&array);
}

