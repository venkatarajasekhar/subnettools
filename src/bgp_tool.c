/*
 * bgp_tool : tools for manipulating BGP tables
 *
 * Copyright (C) 2015 Etienne Basset <etienne POINT basset AT ensta POINT org>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "debug.h"
#include "iptools.h"
#include "utils.h"
#include "generic_csv.h"
#include "heap.h"
#include "subnet_tool.h"
#include "st_printf.h"
#include "bgp_tool.h"

#define SIZE_T_MAX ((size_t)0 - 1)

void fprint_bgp_route(FILE *output, struct bgp_route *route) {
	st_fprintf(output, "%d;%s;%s;%16P;%16I;%10d;%10d;%10d;     %c;%s\n",
			route->valid,
			(route->type == 'i' ? " iBGP" : " eBGP"),
			(route->best == 1 ? "Best" : "  No"),
			route->subnet, route->gw, route->MED,
			route->LOCAL_PREF, route->weight,
			route->origin,
			route->AS_PATH);
}

void fprint_bgp_file(FILE *output, struct bgp_file *bf) {
	int i = 0;

	for (i = 0; i < bf->nr; i++)
		fprint_bgp_route(output, &bf->routes[i]);
}
void fprint_bgp_file_header(FILE *out) {
	fprintf(out, "V;Proto;BEST;          prefix;              GW;       MED;LOCAL_PREF;    WEIGHT;ORIGIN;AS_PATH;\n");
}

void copy_bgproute(struct bgp_route *a, const struct bgp_route *b) {
	memcpy(a, b, sizeof(struct bgp_route));
}

int alloc_bgp_file(struct bgp_file *sf, unsigned long n) {
	if (n > SIZE_T_MAX / sizeof(struct bgp_route)) { /* being paranoid */
		fprintf(stderr, "error: too much memory requested for struct route\n");
		return -1;
	}
	sf->routes = malloc(sizeof(struct bgp_route) * n);
	debug(MEMORY, 2, "trying to alloc %lu bytes\n",  sizeof(struct bgp_route) * n);
	if (sf->routes == NULL) {
		fprintf(stderr, "error: cannot alloc  memory for sf->routes\n");
		return -1;
	}
	sf->nr = 0;
	sf->max_nr = n;
	return 0;
}

static int bgpcsv_prefix_handle(char *s, void *data, struct csv_state *state) {
	struct bgp_file *sf = data;
	int res, i = 0;
	struct subnet subnet;

	while (isspace(s[i]))
		i++;
	res = get_subnet_or_ip(s + i, &subnet);
	if (res > 1000) {
		debug(LOAD_CSV, 3, "invalid IP %s line %lu\n", s, state->line);
		return CSV_INVALID_FIELD_BREAK;
	}
	copy_subnet(&sf->routes[sf->nr].subnet,  &subnet);

	return CSV_VALID_FIELD;
}

static int bgpcsv_GW_handle(char *s, void *data, struct csv_state *state) {
	struct bgp_file *sf = data;
	struct ip_addr addr;
	int res, i = 0;

	while (isspace(s[i]))
		i++;
	res = string2addr(s + i, &addr, 41);
	if (res == sf->routes[sf->nr].subnet.ip_ver) {/* does the gw have same IPversion*/
		copy_ipaddr(&sf->routes[sf->nr].gw, &addr);
	} else {
		memset(&sf->routes[sf->nr].gw, 0, sizeof(struct ip_addr));
		debug(LOAD_CSV, 3, "invalid GW %s line %lu\n", s, state->line);
	}
	return CSV_VALID_FIELD;
}

static int bgpcsv_med_handle(char *s, void *data, struct csv_state *state) {
	struct bgp_file *sf = data;
	int res = 0;
	int i = 0;

	while (isspace(s[i]))
		i++;
	while (isdigit(s[i])) {
		res *= 10;
		res += s[i];
		i++;
	}
	if (s[i] != '\0') {
                debug(LOAD_CSV, 1, "line %lu MED '%s' is not an INT\n", state->line, s);
		return CSV_INVALID_FIELD_BREAK;
	}
	sf->routes[sf->nr].MED = res;
	return CSV_VALID_FIELD;
}

static int bgpcsv_localpref_handle(char *s, void *data, struct csv_state *state) {
	struct bgp_file *sf = data;
	int res = 0;
	int i = 0;

	while (isspace(s[i]))
		i++;
	while (isdigit(s[i])) {
		res *= 10;
		res += s[i];
		i++;
	}
	if (s[i] != '\0') {
                debug(LOAD_CSV, 1, "line %lu LOCAL_PREF '%s' is not an INT\n", state->line, s);
		return CSV_INVALID_FIELD_BREAK;
	}
	sf->routes[sf->nr].LOCAL_PREF = res;
	return CSV_VALID_FIELD;
}

static int bgpcsv_weight_handle(char *s, void *data, struct csv_state *state) {
	struct bgp_file *sf = data;
	int res = 0;
	int i = 0;

	while (isspace(s[i]))
		i++;
	while (isdigit(s[i])) {
		res *= 10;
		res += s[i];
		i++;
	}
	if (s[i] != '\0') {
                debug(LOAD_CSV, 1, "line %lu WEIGHT '%s' is not an INT\n", state->line, s);
		return CSV_INVALID_FIELD_BREAK;
	}
	sf->routes[sf->nr].weight = res;
	return CSV_VALID_FIELD;
}

static int bgpcsv_aspath_handle(char *s, void *data, struct csv_state *state) {
	struct bgp_file *sf = data;

	strxcpy(sf->routes[sf->nr].AS_PATH, s, sizeof(sf->routes[sf->nr].AS_PATH));
	return CSV_VALID_FIELD;
}

static int bgpcsv_best_handle(char *s, void *data, struct csv_state *state) {
	struct bgp_file *sf = data;

	if (!strcasecmp(s, "BEST"))
		sf->routes[sf->nr].best = 1;
	else
		sf->routes[sf->nr].best = 0;
	return CSV_VALID_FIELD;
}

static int bgpcsv_origin_handle(char *s, void *data, struct csv_state *state) {
	struct bgp_file *sf = data;
	int i;

	while (isspace(s[i]))
		i++;
	if (s[i] == 'e' || s[i] == 'i' || s[i] == '?') {
		sf->routes[sf->nr].origin = s[i];
		return CSV_VALID_FIELD;
	} else {
                debug(LOAD_CSV, 1, "line %lu ORIGIN CODE '%c' is invalid\n", state->line, s[i]);
		return CSV_INVALID_FIELD_BREAK;
	}
}

static int bgpcsv_valid_handle(char *s, void *data, struct csv_state *state) {
	struct bgp_file *sf = data;

	if (!strcmp(s, "1"))
		sf->routes[sf->nr].valid = 1;
	else
		sf->routes[sf->nr].valid = 0;
	return CSV_VALID_FIELD;
}

static int bgpcsv_endofline_callback(struct csv_state *state, void *data) {
	struct bgp_file *sf = data;
	struct bgp_route *new_r;

	if (state->badline) {
		debug(LOAD_CSV, 3, "Invalid line %lu\n", state->line);
		return -1;
	}
	sf->nr++;
	if  (sf->nr == sf->max_nr) {
		sf->max_nr *= 2;
		debug(MEMORY, 2, "need to reallocate %lu bytes\n", sf->max_nr * sizeof(struct bgp_route));
		if (sf->max_nr > SIZE_T_MAX / sizeof(struct route)) {
			debug(MEMORY, 1, "cannot allocate %llu bytes for struct route, too big\n", (unsigned long long)sf->max_nr * sizeof(struct bgp_route));
			return CSV_CATASTROPHIC_FAILURE;
		}
		new_r = realloc(sf->routes,  sizeof(struct bgp_route) * sf->max_nr);
		if (new_r == NULL) {
			fprintf(stderr, "unable to reallocate, need to abort\n");
			return  CSV_CATASTROPHIC_FAILURE;
		}
		sf->routes = new_r;
	}
	memset(&sf->routes[sf->nr], 0, sizeof(struct bgp_route));
	return CSV_CONTINUE;
}

static int bgp_field_compare(const char *s1, const char *s2) {
	int i = 0;

	while (isspace(s1[i]))
		i++;
	return strcmp(s1 + i, s2);
}

int load_bgpcsv(char  *name, struct bgp_file *sf, struct st_options *nof) {
	struct csv_field csv_field[] = {
		{ "prefix"	, 0, 0, 1, &bgpcsv_prefix_handle },
		{ "GW"		, 0, 0, 1, &bgpcsv_GW_handle },
		{ "LOCAL_PREF"	, 0, 0, 1, &bgpcsv_localpref_handle },
		{ "MED"		, 0, 0, 1, &bgpcsv_med_handle },
		{ "WEIGHT"	, 0, 0, 1, &bgpcsv_weight_handle },
		{ "AS_PATH"	, 0, 0, 0, &bgpcsv_aspath_handle },
		{ "BEST"	, 0, 0, 1, &bgpcsv_best_handle },
		{ "ORIGIN"	, 0, 0, 1, &bgpcsv_origin_handle },
		{ "V"		, 0, 0, 1, &bgpcsv_valid_handle },
		{ NULL, 0,0,0, NULL }
	};
	struct csv_file cf;
	struct csv_state state;

        cf.is_header = NULL;
	init_csv_file(&cf, csv_field, nof->delim, &strtok_r);
	cf.endofline_callback   = bgpcsv_endofline_callback;
	cf.header_field_compare = bgp_field_compare;

	if (alloc_bgp_file(sf, 16192) < 0)
		return -2;
	return generic_load_csv(name, &cf, &state, sf);
}

int compare_bgp_file(const struct bgp_file *sf1, const struct bgp_file *sf2, struct st_options *o) {
	int i, j;
	int found, changed, changed_j;

	debug(BGPCMP, 6, "file1 : %ld routes, file2 : %ld routes\n", sf1->nr, sf2->nr);
	for (i = 0; i < sf1->nr; i++) {
		st_debug(BGPCMP, 9, "testing %P via %I\n", sf1->routes[i].subnet, 
					sf1->routes[i].gw);
		if (sf1->routes[i].best == 0 || sf1->routes[i].valid != 1) {
			st_debug(BGPCMP, 5, "%P via %I is not a best route, skipping\n", sf1->routes[i].subnet, 
					sf1->routes[i].gw);
			continue;
		}
		found   = 0;
		changed_j = -1;
		for (j = 0; j < sf2->nr; j++) {
			changed = 0;
			if (sf2->routes[j].best == 0 || sf2->routes[j].valid != 1)
				continue;
			if (subnet_compare(&sf1->routes[i].subnet, &sf2->routes[j].subnet) != EQUALS)
				continue;
			found = 1;
			if (is_equal_ip(&sf1->routes[i].gw, &sf2->routes[j].gw) == 0)
				changed++;
			if (sf1->routes[i].MED != sf2->routes[j].MED)
				changed++;
			if (sf1->routes[i].LOCAL_PREF != sf2->routes[j].LOCAL_PREF)
				changed++;
			if (sf1->routes[i].weight != sf2->routes[j].weight)
				changed++;
			if (sf1->routes[i].type != sf2->routes[j].type)
				changed++;
			if (sf1->routes[i].origin != sf2->routes[j].origin)
				changed++;
			if (strcmp(sf1->routes[i].AS_PATH, sf2->routes[j].AS_PATH))
				changed++;
			if (changed != 0)
				changed_j = j;
			else {
				changed_j = -1;
				break;
			}
		}
		if (found == 0) {
			st_fprintf(o->output_file, "WITHDRAWN;"); 
			fprint_bgp_route(o->output_file, &sf1->routes[i]);
			continue;
		}
		if (changed_j == -1) {
			fprintf(o->output_file, "UNCHANGED;");
			fprint_bgp_route(o->output_file, &sf1->routes[i]);
			continue;
		}
		fprintf(o->output_file, "CHANGED  ;");
		fprint_bgp_route(o->output_file, &sf2->routes[changed_j]);
		fprintf(o->output_file, "WAS      ;");
		fprint_bgp_route(o->output_file, &sf1->routes[i]);
	}
	return 1;
}

static int __heap_subnet_is_superior(void *v1, void *v2) {
	struct subnet *s1 = &((struct bgp_route *)v1)->subnet;
	struct subnet *s2 = &((struct bgp_route *)v2)->subnet;
	return subnet_is_superior(s1, s2);
}

static int __heap_gw_is_superior(void *v1, void *v2) {
	struct subnet *s1 = &((struct bgp_route *)v1)->subnet;
	struct subnet *s2 = &((struct bgp_route *)v2)->subnet;
	struct ip_addr *gw1 = &((struct bgp_route *)v1)->gw;
	struct ip_addr *gw2 = &((struct bgp_route *)v2)->gw;

	if (is_equal_ip(gw1, gw2))
		return subnet_is_superior(s1, s2);
	else
		return addr_is_superior(gw1, gw2);
}

static int __heap_med_is_superior(void *v1, void *v2) {
	struct subnet *s1 = &((struct bgp_route *)v1)->subnet;
	struct subnet *s2 = &((struct bgp_route *)v2)->subnet;
	int MED1 = ((struct bgp_route *)v1)->MED;
	int MED2 = ((struct bgp_route *)v2)->MED;

	if (MED1 == MED2)
		return subnet_is_superior(s1, s2);
	else
		return (MED1 < MED2);
}

static int __heap_localpref_is_superior(void *v1, void *v2) {
	struct subnet *s1 = &((struct bgp_route *)v1)->subnet;
	struct subnet *s2 = &((struct bgp_route *)v2)->subnet;
	int LOCAL_PREF1 = ((struct bgp_route *)v1)->MED;
	int LOCAL_PREF2 = ((struct bgp_route *)v2)->MED;

	if (LOCAL_PREF1 == LOCAL_PREF2)
		return subnet_is_superior(s1, s2);
	else
		return (LOCAL_PREF1 < LOCAL_PREF2);
}

static int __bgp_sort_by(struct bgp_file *sf, int cmpfunc(void *v1, void *v2)) {
	unsigned long i;
	TAS tas;
	struct bgp_route *new_r, *r;

	if (sf->nr == 0)
		return 0;
	debug_timing_start(2);
	alloc_tas(&tas, sf->nr, cmpfunc);

	new_r = malloc(sf->max_nr * sizeof(struct bgp_route));

	if (tas.tab == NULL || new_r == NULL) {
		fprintf(stderr, "%s : no memory\n", __FUNCTION__);
		debug_timing_end(2);
		return -1;
	}
	debug(MEMORY, 2, "Allocated %lu bytes for new struct bgp_route\n", sf->max_nr * sizeof(struct bgp_route));
	/* basic heapsort */
	for (i = 0 ; i < sf->nr; i++)
		addTAS(&tas, &(sf->routes[i]));
	for (i = 0 ; i < sf->nr; i++) {
		r = popTAS(&tas);
		copy_bgproute(&new_r[i], r);
	}
	free(tas.tab);
	free(sf->routes);
	sf->routes = new_r;
	debug_timing_end(2);
	return 0;
}

struct bgpsort {
	char *name;
	int (*cmpfunc)(void *v1, void *v2);
};

static const struct bgpsort bgpsort[] = {
	{ "prefix",	&__heap_subnet_is_superior },
	{ "gw",		&__heap_gw_is_superior },
	{ "med",	&__heap_med_is_superior },
	{ "localpref",	&__heap_localpref_is_superior },
	{NULL,		NULL}
};

void bgp_available_cmpfunc(FILE *out) {
	int i = 0;

	while (1) {
		if (bgpsort[i].name == NULL)
			break;
		fprintf(out, " %s\n", bgpsort[i].name);
		i++;
	}
}

int bgp_sort_by(struct bgp_file *sf, char *name) {
	int i = 0;

	while (1) {
		if (bgpsort[i].name == NULL)
			break;
		if (!strncasecmp(name, bgpsort[i].name, strlen(name)))
			return __bgp_sort_by(sf, bgpsort[i].cmpfunc);
		i++;
	}
	return -1664;
}