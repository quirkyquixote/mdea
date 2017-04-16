/* Copyright Luis Sanz <luis.sanz@gmail.com> 2017 */

#include "mdea.h"

int test(int argc, char *argv[], char **error)
{
	struct mdea_node *node;

	if (mdea_node_from_file_desc(STDIN_FILENO, &node, error) != 0)
		return -1;
	if (mdea_node_to_file_desc(node, STDOUT_FILENO, error) != 0)
		return -1;
	mdea_unref(node);
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

