#ifndef IPAM_EA_H
#define IPAM_EA_H

struct  ipam_ea {
	char *name;
	char *value; /* value of EA; MUST be malloc'ed*/
	int len;
};

/* return the malloc'd size of ea*/
int ea_size(const struct ipam_ea *ea);

/* set value of 'ea' to 'value'
 * returns:
 *	-1 if no memory
 *	1  if SUCCESS
 **/
int ea_strdup(struct ipam_ea *ea, const char *value);

void free_ea_array(struct ipam_ea *ea, int n);

/*  alloc_ea_array
 *  alloc an array of Extended Attributes
 *	@n : number of Extended Attributes
 *  returns:
 *	a pointer to a struct ipam_ea
 *	NULL if ENOMEM
 */
struct ipam_ea *alloc_ea_array(int n);

/*
 *  realloc_ea_array
 *  increase size of an EA array; set new members to NULL
 *  @ea    : old array
 *  @old_n : previous array size
 *  @new_n : new array size
 *  returns:
 *	a pointer to a new struct ipam_ea
 *	NULL if ENOMEM
 */
struct ipam_ea *realloc_ea_array(struct ipam_ea *ea, int old_n, int new_n);

/*
 * filter ea array to see if EA with name 'ea_name' matches 'value' using operator 'op'
 * @ea      : the EA array
 * @ea_nr   : length of  EA array
 * @ea_name : the name of the EA to filter on
 * @value   : the value to match
 * @op      : the operator (=, #, <, >, ~)
 * returns:
 *	1  if match
 *	0  if no match
 *	-1 on error
 */
int filter_ea(const struct ipam_ea *ea, int ea_nr, const char *ea_name,
		const char *value, char op);
#else
#endif
