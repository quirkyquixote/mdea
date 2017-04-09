Mdea - Yet Another JSON Library
===============================

This library provides utilities to parse JSON files into C data structures and
viceversa.

Requirements
------------

Just C.

Installation
------------

mdea provices a simple Makefile; to build from scratch:

    make all
    make install

Usage
-----

The src/mdea-json.c file, compiled into the mdea-json binary provides an usage
example of the API.

The basic idea is that different implementations of JSON (text files, strings,
node trees) can be converted to and parsed from token streams with emitters and
parsers, respectively.

	char *error;
	struct mdea_parser *p = mdea_<foo>_parser(<foo object>);
	struct mdea_emitter *e = mdea_<bar>_emitter(<bar object>);
	if (mdea_parse(p, e, &error) != 0) {
		fprintf(stderr, "ERROR: %s\n", error);
		free(error);
	}
	mdea_parser_destroy(p);
	mdea_emitter_destroy(e);

It is possible to implement new parser and emitter types and pass them to the
`mdea_parse` function.

