/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "../mdea.h"

void print_object(struct MdeaObject *object)
{
	wchar_t *key;
	struct MdeaNode *val;
	const wchar_t *str;

	printf("%zu elements:\n", object->size);
	mdea_object_foreach(key, val, object) {
		mdea_get_string(val, &str);
		printf("%ls: %ls\n", key, str);
	}
}

int main(int argc, char *argv[])
{
	struct MdeaObject object;
	char line[256];

	mdea_object_init(&object);

	while (fscanf(stdin, "%s", line) == 1) {
		if (line[0] == '+') {
			size_t len = strlen(line + 1) + 1;
			wchar_t tmp[len];
			mbstowcs(tmp, line + 1, len);
			mdea_object_add(&object, tmp, mdea_string(tmp));
		} else if (line[0] == '-') {
			size_t len = strlen(line + 1) + 1;
			wchar_t tmp[len];
			mbstowcs(tmp, line + 1, len);
			mdea_object_remove(&object, tmp);
		} else if (strcmp(line, "x") == 0) {
			mdea_object_deinit(&object);
			mdea_object_init(&object);
		} else {
			fprintf(stderr, "ERROR: bad command: %s\n", line);
			return -1;
		}
		print_object(&object);
	}

	mdea_object_deinit(&object);
}

