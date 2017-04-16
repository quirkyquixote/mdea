/* Copyright Luis Sanz <luis.sanz@gmail.com> 2017 */

#include "mdea.h"

int test(int argc, char *argv[], char **error)
{
	struct mdea_node *node;
	size_t alloc = 0, len = 0;
	char *buf = NULL;

	do {
		alloc += 256;
		buf = realloc(buf, alloc);
		len += read(STDIN_FILENO, buf + len, alloc - len);
	} while (len == alloc);

	if (mdea_node_from_string(buf, &node, error) != 0)
		return -1;
	if (mdea_node_to_string(node, &buf, &alloc, error) != 0)
		return -1;
	mdea_unref(node);
	write(STDOUT_FILENO, buf, strlen(buf));
	free(buf);
	return 0;
}

int main(int argc, char *argv[])
{
	char *error;

	if (test(argc, argv, &error) != 0) {
		fprintf(stderr, "ERROR: %s\n", error);
		free(error);
		return -1;
	}
	return 0;
}

