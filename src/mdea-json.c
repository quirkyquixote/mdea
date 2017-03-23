
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <wctype.h>

#include "mdea.h"


int node_from_file(const char *path, struct mdea_node **node, char **error)
{
	int ret = -1;
	int file = open(path, O_RDONLY);
	if (file >= 0) {
		struct mdea_parser *p = mdea_file_parser(file);
		struct mdea_emitter *e = mdea_node_emitter(node);
		ret = mdea_parse(p, e, error);
		mdea_parser_destroy(p);
		mdea_emitter_destroy(e);
		close(file);
	} else {
		perror(path);
	}
	return ret;
}

int node_from_string(const char *string, struct mdea_node **node, char **error)
{
	size_t len = strlen(string) + 3;
	char tmp[len];
	snprintf(tmp, len, "%s", string);
	{
		struct mdea_parser *p = mdea_string_parser(tmp);
		struct mdea_emitter *e = mdea_node_emitter(node);
		int ret = mdea_parse(p, e, NULL);
		mdea_parser_destroy(p);
		mdea_emitter_destroy(e);
		if (ret == 0)
			return 0;
	}
	snprintf(tmp, len, "\"%s\"", string);
	{
		struct mdea_parser *p = mdea_string_parser(tmp);
		struct mdea_emitter *e = mdea_node_emitter(node);
		int ret = mdea_parse(p, e, error);
		mdea_parser_destroy(p);
		mdea_emitter_destroy(e);
		if (ret == 0)
			return 0;
	}
	return -1;
}

int node_to_file(const char *path, struct mdea_node *node, char **error)
{
	int ret = -1;
	int file = open(path, O_CREAT|O_WRONLY|O_TRUNC);
	if (file >= 0) {
		struct mdea_parser *p = mdea_node_parser(node);
		struct mdea_emitter *e = mdea_file_emitter(file);
		ret = mdea_write(e, node, error);
		mdea_parser_destroy(p);
		mdea_emitter_destroy(e);
		close(file);
	} else {
		perror(path);
	}
	return ret;
}

int test(int argc, char *argv[], char **error)
{
	if (argc <= 1)
		return -1;
	if (strcmp(argv[1], "get") == 0) {
		if (argc != 4)
			return -1;
		struct mdea_node *root;
		if (node_from_file(argv[3], &root, error) != 0)
			return -1;
		size_t len = strlen(argv[2]) + 1;
		char path[len];
		snprintf(path, len, "%s", argv[2]);
		struct mdea_node *node;
		if (mdea_get(root, path, &node, error) != 0)
			return -1;
		struct mdea_emitter *e = mdea_file_emitter(STDOUT_FILENO);
		if (mdea_write(e, node, error) != 0)
			return -1;
		mdea_emitter_destroy(e);
		return 0;
	}
	if (strcmp(argv[1], "set") == 0) {
		if (argc != 5)
			return -1;
		struct mdea_node *root;
		if (node_from_file(argv[4], &root, NULL) != 0)
			root = mdea_object_node();
		struct mdea_node *node;
		if (node_from_string(argv[3], &node, error) != 0)
			return -1;
		size_t len = strlen(argv[2]) + 1;
		char path[len];
		snprintf(path, len, "%s", argv[2]);
		if (mdea_set(&root, path, node, error) != 0)
			return -1;
		return node_to_file(argv[4], root, error);
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
