/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "../json.h"

void print_object(struct JsonObject *object)
{
	wchar_t *key;
	struct JsonNode *val;
	const wchar_t *str;

	printf("%zu elements:\n", object->size);
	json_object_foreach(key, val, object) {
		json_get_string(val, &str);
		printf("%ls: %ls\n", key, str);
	}
}

int main(int argc, char *argv[])
{
	struct JsonObject object;
	char line[256];

	json_object_init(&object);

	while (fscanf(stdin, "%s", line) == 1) {
		if (line[0] == '+') {
			size_t len = strlen(line + 1) + 1;
			wchar_t tmp[len];
			mbstowcs(tmp, line + 1, len);
			json_object_add(&object, tmp, json_string(tmp));
		} else if (line[0] == '-') {
			size_t len = strlen(line + 1) + 1;
			wchar_t tmp[len];
			mbstowcs(tmp, line + 1, len);
			json_object_remove(&object, tmp);
		} else if (strcmp(line, "x") == 0) {
			json_object_deinit(&object);
			json_object_init(&object);
		} else {
			fprintf(stderr, "ERROR: bad command: %s\n", line);
			return -1;
		}
		print_object(&object);
	}

	json_object_deinit(&object);
}

