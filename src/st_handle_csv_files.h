#ifndef ST_HANDLE_CSV_FILES_H
#define ST_HANDLE_CSV_FILES_H

#include "st_options.h"

struct subnet_file {
	struct route *routes;
	unsigned long nr;
	unsigned long max_nr; /* the number of routes that has been malloced */
};

struct bgp_file {
	struct bgp_route *routes;
	unsigned long nr;
	unsigned long max_nr; /* the number of routes that has been malloced */
};

int alloc_subnet_file(struct subnet_file *sf, unsigned long n);

int load_netcsv_file(char *name, struct subnet_file *sf, struct st_options *nof);
int load_PAIP(char  *name, struct subnet_file *sf, struct st_options *nof);
int load_bgpcsv(char  *name, struct bgp_file *sf, struct st_options *nof);

void print_subnet_file(const struct subnet_file *sf, int comp_level);
void fprint_subnet_file(FILE *output, const struct subnet_file *sf, int comp_level);
void fprint_subnet_file_fmt(FILE *output, const struct subnet_file *sf, const char *fmt);

void fprint_bgp_file_fmt(FILE *output, const struct bgp_file *sf, const char *fmt);
void print_bgp_file_fmt(const struct bgp_file *sf, const char *fmt);
#else
#endif
