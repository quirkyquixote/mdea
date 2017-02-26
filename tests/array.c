/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "../json.h"

void print_array(struct JsonArray *array)
{
	struct JsonNode *iter;
	const wchar_t *str;

	printf("%zu elements:\n", array->size);
	json_array_foreach(iter, array) {
		json_get_string(iter, &str);
		printf("%ls\n", str);
	}
}

int main(int argc, char *argv[])
{
	struct JsonArray array;
	char line[256];

	json_array_init(&array);

	while (fscanf(stdin, "%s", line) == 1) {
		if (line[0] == '+') {
			size_t len = strlen(line + 1) + 1;
			wchar_t tmp[len];
			mbstowcs(tmp, line + 1, len);
			json_array_add(&array, json_string(tmp));
		} else if (line[0] == '-') {
			long key = strtol(line + 1, NULL, 10);
			json_array_remove(&array, key);
		} else if (strcmp(line, "x") == 0) {
			json_array_deinit(&array);
			json_array_init(&array);
		} else {
			fprintf(stderr, "ERROR: bad command: %s\n", line);
			return -1;
		}
		print_array(&array);
	}

	json_array_deinit(&array);
}

