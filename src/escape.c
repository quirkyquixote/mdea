

#ifndef MDEA_ESCAPE_H_
#define MDEA_ESCAPE_H_

#include "escape.h"

#include <stdio.h>
#include <stdlib.h>

#include "error.h"

char *mdea_escape(const char *str, char **error)
{
	size_t alloc = 0;
	size_t size = 0;
	char *buf = NULL;
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
				mdea_error(error, "Unexpected end of string");
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
				int val, tmp;
				if (sscanf(str, "%02X%n", &val, &tmp) < 1 || tmp != 2) {
					free(buf);
					mdea_error(error, "Expected \\xXX");
					return NULL;
				}
				buf[size] = val;
				str += 2;
			} else if (*str == 'u') {
				++str;
				if (*str == '{') {
					int val, tmp;
					if (sscanf(str, "{%X}%n", &val, &tmp) < 1) {
						free(buf);
						mdea_error(error, "Expected \\u{X} ... \\u{XXXXXX}");
						return NULL;
					}
					buf[size] = val;
					str += tmp;
				} else {
					int val, tmp;
					if (sscanf(str, "%04X%n", &val, &tmp) < 1 || tmp != 4) {
						free(buf);
						mdea_error(error, "Expected \\uXXXX");
						return NULL;
					}
					buf[size] = val;
					str += 4;
				}
			} else {
				free(buf);
				mdea_error(error, "Unknown escape sequence: \\%lc", *str);
				return NULL;
			}
		}
		++size;
	}
	return buf;
}

#endif  // MDEA_ESCAPE_H_
