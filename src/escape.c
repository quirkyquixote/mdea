

#ifndef MDEA_ESCAPE_H_
#define MDEA_ESCAPE_H_

#include "escape.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

char *mdea_escape(const char *str, char **error)
{
	size_t size = 0;
	char *buf = calloc(strlen(str), 1);
	int val;
	for (;;) {
		if (*str != '\\') {
			val = *str;
			++str;
		} else {
			++str;
			if (*str == 0) {
				free(buf);
				mdea_error(error, "Unexpected end of string");
				return NULL;
			}
			if (*str == '0') {
				val = 0;
				++str;
			} else if (*str == '\'') {
				val = '\'';
				++str;
			} else if (*str == '"') {
				val = '"';
				++str;
			} else if (*str == '\\') {
				val = '\\';
				++str;
			} else if (*str == 'n') {
				val = '\n';
				++str;
			} else if (*str == 'r') {
				val = '\r';
				++str;
			} else if (*str == 'v') {
				val = '\v';
				++str;
			} else if (*str == 't') {
				val = '\t';
				++str;
			} else if (*str == 'b') {
				val = '\b';
				++str;
			} else if (*str == 'f') {
				val = '\f';
				++str;
			} else if (*str == 'x') {
				++str;
				int tmp;
				if (sscanf(str, "%02X%n", &val, &tmp) < 1 || tmp != 2) {
					free(buf);
					mdea_error(error, "Expected \\xXX");
					return NULL;
				}
				str += tmp;
			} else if (*str == 'u') {
				++str;
				if (*str == '{') {
					int tmp;
					if (sscanf(str, "{%X}%n", &val, &tmp) < 1) {
						free(buf);
						mdea_error(error, "Expected \\u{X} ... \\u{XXXXXX}");
						return NULL;
					}
					str += tmp;
				} else {
					int tmp;
					if (sscanf(str, "%04X%n", &val, &tmp) < 1 || tmp != 4) {
						free(buf);
						mdea_error(error, "Expected \\uXXXX");
						return NULL;
					}
					str += tmp;
				}
			} else {
				free(buf);
				mdea_error(error, "Unknown escape sequence: \\%lc", *str);
				return NULL;
			}
		}
		if (val <= 0x7F) {
			buf[size++] = val;
			if (val == 0)
				break;
		} else if (val <= 0x7FF) {
			buf[size++] = 0xC0 + ((val >> 6) & 0x1F);
			buf[size++] = 0x80 + (val & 0x3F);
		} else if (val <= 0xFFFF) {
			buf[size++] = 0xE0 + ((val >> 12) & 0x0F);
			buf[size++] = 0x80 + ((val >> 6) & 0x3F);
			buf[size++] = 0x80 + (val & 0x3F);
		} else if (val <= 0x10FFFF) {
			buf[size++] = 0xF0 + ((val >> 18) & 0x07);
			buf[size++] = 0x80 + ((val >> 12) & 0x3F);
			buf[size++] = 0x80 + ((val >> 6) & 0x3F);
			buf[size++] = 0x80 + (val & 0x3F);
		}
	}
	return buf;
}

#endif  // MDEA_ESCAPE_H_
