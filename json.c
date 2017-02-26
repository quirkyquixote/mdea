/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "json.h"

#include <stdarg.h>
#include <wctype.h>

void json_error(wchar_t **error, wchar_t *msg, ...)
{
	if (error == NULL)
		return;
	va_list ap;
	va_start(ap, msg);
	*error = calloc(1024, sizeof(**error));
	vswprintf(*error, 1024, msg, ap);
	va_end(ap);
}

void json_destroy(struct JsonNode *node)
{
	switch (node->type) {
	case JSON_NULL:
	case JSON_NUMBER:
	case JSON_BOOLEAN:
		break;
	case JSON_STRING:
		free(node->string);
		break;
	case JSON_ARRAY:
		json_array_deinit(&node->array);
		break;
	case JSON_OBJECT:
		json_object_deinit(&node->object);
		break;
	}
	free(node);
}

int read_char(FILE *f)
{
	int c;
	do
		c = fgetwc(f);
	while (isspace(c));
	return c;
}

int read_string(FILE *f, wchar_t **rval, wchar_t **error)
{
	int c;
	size_t size = 0, alloc = 0;
	*rval = NULL;
	for (;;) {
		c = fgetwc(f);
		if (c == WEOF) {
			json_error(error, L"Unexpected end of file");
			return -1;
		}
		else if (c == '"')
			c = 0;
		else if (c == '\\')
			c = fgetwc(f);
		if (size == alloc) {
			alloc = alloc ? alloc * 2 : 2;
			*rval = realloc(*rval, sizeof(**rval) * alloc);
		}
		(*rval)[size] = c;
		if (c == 0)
			return 0;
		++size;
	}
}

int json_read(FILE *f, struct JsonNode **rval, wchar_t **error)
{
	int c;
	c = read_char(f);
	if (c == '[') {
		c = read_char(f);
		*rval = json_array();
		if (c == ']')
			return 0;
		ungetc(c, f);
		struct JsonArray *array;
		json_get_array(*rval, &array);
		for (;;) {
			struct JsonNode *tmp;
			if (json_read(f, &tmp, error) != 0)
				return -1;
			json_array_add(array, tmp);
			c = read_char(f);
			if (c == ']')
				return 0;
			if (c != ',') {
				json_error(error, L"Expected end of array");
				return -1;
			}
		}
	} else if (c == '{') {
		c = read_char(f);
		*rval = json_object();
		if (c == '}')
			return 0;
		ungetc(c, f);
		struct JsonObject *obj;
		json_get_object(*rval, &obj);
		for (;;) {
			wchar_t *key;
			struct JsonNode *val;
			c = read_char(f);
			if (c != '"') {
				json_error(error, L"Expected string");
				return -1;
			}
			if (read_string(f, &key, error) != 0)
				return -1;
			c = read_char(f);
			if (c != ':') {
				json_error(error, L"Expected ':'");
				json_object_add(obj, key, json_null());
				free(key);
				return -1;
			}
			if (json_read(f, &val, error) != 0) {
				json_object_add(obj, key, json_null());
				free(key);
				return -1;
			}
			json_object_add(obj, key, val);
			free(key);
			c = read_char(f);
			if (c == '}')
				return 0;
			if (c != ',') {
				json_error(error, L"Expected end of object");
				return -1;
			}
		}
	} else if (c == '"') {
		wchar_t *data;
		if (read_string(f, &data, error) != 0)
			return -1;
		*rval = json_string(data);
		return 0;
	} else if (c == 'n') {
		if (fgetwc(f) != 'u' || fgetwc(f) != 'l' || fgetwc(f) != 'l') {
			json_error(error, L"Expected null");
			return -1;
		}
		*rval = json_null();
		return 0;
	} else if (c == 't') {
		if (fgetwc(f) != 'r' || fgetwc(f) != 'u' || fgetwc(f) != 'e') {
			json_error(error, L"Expected true");
			return -1;
		}
		*rval = json_boolean(1);
		return 0;
	} else if (c == 'f') {
		if (fgetwc(f) != 'a' || fgetwc(f) != 'l' || fgetwc(f) != 's' || fgetwc(f) != 'e') {
			json_error(error, L"Expected false");
			return -1;
		}
		*rval = json_boolean(0);
		return 0;
	} else if ((c >= '0' && c <= '9') || c == '.') {
		ungetc(c, f);
		double data;
		fwscanf(f, L"%lf", &data);
		*rval = json_number(data);
		return 0;
	} else {
		if (iswprint(c))
			json_error(error, L"Unexpected character: '%lc'", c);
		else
			json_error(error, L"Unexpected character: \\%x", c);
		return -1;
	}
}

int json_write(FILE *f, struct JsonNode *root, int indent, wchar_t **error)
{
	switch (root->type) {
		case JSON_NULL:
			fwprintf(f, L"null");
			return 0;
		case JSON_NUMBER:
			fwprintf(f, L"%g", root->number);
			return 0;
		case JSON_STRING:
			fwprintf(f, L"\"%ls\"", root->string);
			return 0;
		case JSON_BOOLEAN:
			fwprintf(f, L"%ls", root->boolean ? L"true" : L"false");
			return 0;
		case JSON_ARRAY:
			if (root->array.size == 0) {
				fwprintf(f, L"[]");
			} else {
				int is_first = 1;
				fwprintf(f, L"[\n");
				for (int i = 0; i < root->array.size; ++i) {
					if (is_first)
						is_first = 0;
					else
						fwprintf(f, L",\n");
					for (int i = 0; i <= indent; ++i)
						fwprintf(f, L"  ");
					json_write(f, root->array.vals[i], indent + 1, error);
				}
				fwprintf(f, L"\n");
				for (int i = 0; i < indent; ++i)
					fwprintf(f, L"  ");
				fwprintf(f, L"]");
			}
			return 0;
		case JSON_OBJECT:
			if (root->object.size == 0) {
				fwprintf(f, L"{}");
			} else {
				int is_first = 1;
				fwprintf(f, L"{\n");
				for (int i = 0; i < root->object.size; ++i) {
					if (is_first)
						is_first = 0;
					else
						fwprintf(f, L",\n");
					for (int i = 0; i <= indent; ++i)
						fwprintf(f, L"  ");
					fwprintf(f, L"\"%ls\": ", root->object.fields[i].key);
					json_write(f, root->object.fields[i].val, indent + 1, error);
				}
				fwprintf(f, L"\n");
				for (int i = 0; i < indent; ++i)
					fwprintf(f, L"  ");
				fwprintf(f, L"}");
			}
			return 0;
	}
	return -1;
}

int json_get(struct JsonNode *root, struct JsonNode **rval, ...)
{
	int found = 0;
	va_list ap;
	va_start(ap, rval);
	for (;;) {
		wchar_t *key = va_arg(ap, wchar_t *);
		if (key == NULL) {
			found = 1;
			break;
		}
		struct JsonObject *obj;
		if (json_get_object(root, &obj) != 0)
			break;
		if (json_object_get(obj, key, (void**)&root) != 0)
			break;
	}
	va_end(ap);
	return found;
}
