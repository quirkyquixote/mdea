/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "mdea.h"

#include <stdarg.h>
#include <wctype.h>


void mdea_error(wchar_t **error, wchar_t *msg, ...)
{
	if (error == NULL)
		return;
	va_list ap;
	va_start(ap, msg);
	*error = calloc(1024, sizeof(**error));
	vswprintf(*error, 1024, msg, ap);
	va_end(ap);
}

wchar_t *mdea_escape(const wchar_t *str, wchar_t **error)
{
	size_t alloc = 0;
	size_t size = 0;
	wchar_t *buf = NULL;
	for (;;) {
		if (alloc == size) {
			alloc = alloc ? 2 * alloc : 2;
			buf = realloc(buf, alloc * sizeof(*buf));
		}
		if (*str != '\\') {
			buf[size] = *str;
			if (*str == 0)
				break;
			++str;
		} else {
			++str;
			if (*str == 0) {
				free(buf);
				mdea_error(error, L"Unexpected end of string");
				return NULL;
			}
			if (*str == '0') {
				buf[size] = 0;
				++str;
			} else if (*str == '\'') {
				buf[size] = '\'';
				++str;
			} else if (*str == '"') {
				buf[size] = '"';
				++str;
			} else if (*str == '\\') {
				buf[size] = '\\';
				++str;
			} else if (*str == 'n') {
				buf[size] = '\n';
				++str;
			} else if (*str == 'r') {
				buf[size] = '\r';
				++str;
			} else if (*str == 'v') {
				buf[size] = '\v';
				++str;
			} else if (*str == 't') {
				buf[size] = '\t';
				++str;
			} else if (*str == 'b') {
				buf[size] = '\b';
				++str;
			} else if (*str == 'f') {
				buf[size] = '\f';
				++str;
			} else if (*str == 'x') {
				++str;
				int tmp;
				if (swscanf(str, L"%02X%n", &buf[size], &tmp) < 1 || tmp != 2) {
					free(buf);
					mdea_error(error, L"Expected \\xXX");
					return NULL;
				}
				str += 2;
			} else if (*str == 'u') {
				++str;
				if (*str == '{') {
					int tmp;
					if (swscanf(str, L"{%X}%n", &buf[size], &tmp) < 1) {
						free(buf);
						mdea_error(error, L"Expected \\u{X} ... \\u{XXXXXX}");
						return NULL;
					}
					str += tmp;
				} else {
					int tmp;
					if (swscanf(str, L"%04X%n", &buf[size], &tmp) < 1 || tmp != 4) {
						free(buf);
						mdea_error(error, L"Expected \\uXXXX");
						return NULL;
					}
					str += 4;
				}
			} else {
				free(buf);
				mdea_error(error, L"Unknown escape sequence: \\%lc", *str);
				return NULL;
			}
		}
		++size;
	}
	return buf;
}

void mdea_destroy(struct MdeaNode *node)
{
	switch (node->type) {
		case MDEA_NULL:
		case MDEA_NUMBER:
		case MDEA_BOOLEAN:
			break;
		case MDEA_STRING:
			free(node->string);
			break;
		case MDEA_ARRAY:
			mdea_array_deinit(&node->array);
			break;
		case MDEA_OBJECT:
			mdea_object_deinit(&node->object);
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
	int escaped = 0;
	*rval = NULL;
	for (;;) {
		c = fgetwc(f);
		if (c == WEOF) {
			mdea_error(error, L"Unexpected end of file");
			return -1;
		}
		if (!escaped) {
			if (c == '"')
				c = 0;
			else if (c == '\\')
				escaped = 1;
		} else {
			escaped = 0;
		}
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

int mdea_read(FILE *f, struct MdeaNode **rval, wchar_t **error)
{
	int c;
	c = read_char(f);
	if (c == '[') {
		c = read_char(f);
		*rval = mdea_array();
		if (c == ']')
			return 0;
		ungetwc(c, f);
		struct MdeaArray *array;
		mdea_get_array(*rval, &array);
		for (;;) {
			struct MdeaNode *tmp;
			if (mdea_read(f, &tmp, error) != 0)
				return -1;
			mdea_array_add(array, tmp);
			c = read_char(f);
			if (c == ']')
				return 0;
			if (c != ',') {
				mdea_error(error, L"Expected end of array");
				return -1;
			}
		}
	} else if (c == '{') {
		c = read_char(f);
		*rval = mdea_object();
		if (c == '}')
			return 0;
		ungetwc(c, f);
		struct MdeaObject *obj;
		mdea_get_object(*rval, &obj);
		for (;;) {
			wchar_t *key;
			struct MdeaNode *val;
			c = read_char(f);
			if (c != '"') {
				mdea_error(error, L"Expected string");
				return -1;
			}
			if (read_string(f, &key, error) != 0)
				return -1;
			c = read_char(f);
			if (c != ':') {
				mdea_error(error, L"Expected ':'");
				mdea_object_add(obj, key, mdea_null());
				free(key);
				return -1;
			}
			if (mdea_read(f, &val, error) != 0) {
				mdea_object_add(obj, key, mdea_null());
				free(key);
				return -1;
			}
			mdea_object_add(obj, key, val);
			free(key);
			c = read_char(f);
			if (c == '}')
				return 0;
			if (c != ',') {
				mdea_error(error, L"Expected end of object");
				return -1;
			}
		}
	} else if (c == '"') {
		wchar_t *data;
		if (read_string(f, &data, error) != 0)
			return -1;
		*rval = mdea_string(data);
		return 0;
	} else if (c == 'n') {
		if (fgetwc(f) != 'u' || fgetwc(f) != 'l' || fgetwc(f) != 'l') {
			mdea_error(error, L"Expected null");
			return -1;
		}
		*rval = mdea_null();
		return 0;
	} else if (c == 't') {
		if (fgetwc(f) != 'r' || fgetwc(f) != 'u' || fgetwc(f) != 'e') {
			mdea_error(error, L"Expected true");
			return -1;
		}
		*rval = mdea_boolean(1);
		return 0;
	} else if (c == 'f') {
		if (fgetwc(f) != 'a' || fgetwc(f) != 'l' || fgetwc(f) != 's' || fgetwc(f) != 'e') {
			mdea_error(error, L"Expected false");
			return -1;
		}
		*rval = mdea_boolean(0);
		return 0;
	} else if ((c >= '0' && c <= '9') || c == '.') {
		ungetwc(c, f);
		double data;
		fwscanf(f, L"%lf", &data);
		*rval = mdea_number(data);
		return 0;
	} else {
		if (iswprint(c))
			mdea_error(error, L"Unexpected character: '%lc'", c);
		else
			mdea_error(error, L"Unexpected character: \\u%04X", c);
		return -1;
	}
}

int mdea_write(FILE *f, struct MdeaNode *root, int indent, wchar_t **error)
{
	switch (root->type) {
		case MDEA_NULL:
			fwprintf(f, L"null");
			return 0;
		case MDEA_NUMBER:
			fwprintf(f, L"%g", root->number);
			return 0;
		case MDEA_STRING:
			fwprintf(f, L"\"%ls\"", root->string);
			return 0;
		case MDEA_BOOLEAN:
			fwprintf(f, L"%ls", root->boolean ? L"true" : L"false");
			return 0;
		case MDEA_ARRAY:
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
					mdea_write(f, root->array.vals[i], indent + 1, error);
				}
				fwprintf(f, L"\n");
				for (int i = 0; i < indent; ++i)
					fwprintf(f, L"  ");
				fwprintf(f, L"]");
			}
			return 0;
		case MDEA_OBJECT:
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
					mdea_write(f, root->object.fields[i].val, indent + 1, error);
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

int mdea_get(struct MdeaNode *root, struct MdeaNode **rval, ...)
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
		struct MdeaObject *obj;
		if (mdea_get_object(root, &obj) != 0)
			break;
		if (mdea_object_get(obj, key, (void**)&root) != 0)
			break;
	}
	va_end(ap);
	return found;
}
