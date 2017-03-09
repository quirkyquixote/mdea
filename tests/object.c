/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "../mdea.h"

void print_object(struct mdea_object *object)
{
	struct mdea_node *val;
	wchar_t *key;
	const wchar_t *string;

	printf("%zu elements:\n", object->size);
	mdea_object_foreach(key, val, object) {
		mdea_get_string(val, &string, NULL);
		printf("%ls: %ls\n", key, string);
	}
}

int main(int argc, char *argv[])
{
	struct mdea_object object;
	char line[256];

	mdea_object_init(&object);

	while (fscanf(stdin, "%s", line) == 1) {
		if (line[0] == '+') {
			size_t len = strlen(line + 1) + 1;
			wchar_t tmp[len];
			mbstowcs(tmp, line + 1, len);
			mdea_object_insert(&object, tmp, mdea_string(tmp));
		} else if (line[0] == '-') {
			size_t len = strlen(line + 1) + 1;
			wchar_t tmp[len];
			mbstowcs(tmp, line + 1, len);
			mdea_object_erase(&object, tmp);
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

