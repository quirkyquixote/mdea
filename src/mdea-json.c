
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <wctype.h>

#include "mdea.h"

int test(int argc, char *argv[], char **error)
{
	if (argc <= 1)
		return -1;
	if (strcmp(argv[1], "get") == 0) {
		if (argc != 4)
			return -1;
		struct mdea_node *root;
		if (mdea_node_from_file_path(argv[3], &root, error) != 0)
			return -1;
		struct mdea_node *node;
		if (mdea_get(root, argv[2], &node, error) != 0)
			return -1;
		if (mdea_node_to_file_desc(node, STDOUT_FILENO, error) != 0)
			return -1;
		return 0;
	}
	if (strcmp(argv[1], "set") == 0) {
		if (argc != 5)
			return -1;
		struct mdea_node *root;
		if (mdea_node_from_file_path(argv[4], &root, NULL) != 0)
			root = mdea_object_node();
		struct mdea_node *node;
		if (mdea_node_from_string(argv[3], &node, error) != 0)
			return -1;
		if (mdea_set(&root, argv[2], node, error) != 0)
			return -1;
		return mdea_node_to_file_path(root, argv[4], error);
	}
	return -1;
}

int main(int argc, char *argv[])
{
	char *error = NULL;
	if (test(argc, argv, &error) != 0) {
		if (error == NULL) {
			fprintf(stderr, "usage: mdea get <key> <path>\n");
			fprintf(stderr, "       mdea set <key> <val> <path>\n");
		} else {
			fprintf(stderr, "ERROR: %s\n", error);
		}
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
