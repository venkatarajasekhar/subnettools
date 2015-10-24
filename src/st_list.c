/*
 * generic double linked-list manipulation functions
 * some parts are inspired from Linux Kernel by Linus Torvalds
 *
 * Copyright (C) 2015 Etienne Basset <etienne POINT basset AT ensta POINT org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License
 * as published by the Free Software Foundation.
 */
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include "st_list.h"
#include "time.h"


void init_list(st_list *list)
{
	list->next = list;
	list->prev = list;
}

int list_empty(st_list *list)
{
	return list == list->next;
}

int list_length(st_list *list)
{
	st_list *l;
	int i = 0;

	LIST_FOR_EACH(l, list)
		i++;
	return i;
}
/*
 * insert a nex entry after head
 */
void list_add(st_list *new, st_list *head)
{
	head->next->prev = new;
	new->next  = head->next;
	new->prev  = head;
	head->next = new;
}

/*
 * insert a nex entry at the end
 */
void list_add_tail(st_list *new, st_list *head)
{
	new->next = head;
	new->prev = head->prev;
	head->prev->next = new;
	head->prev = new;
}

void list_del(st_list *list)
{
	list->prev->next = list->next;
	list->next->prev = list->prev;
}

/*
 * merges two already sorted lists
 */
void list_merge(st_list *l1, st_list *l2, st_list *res,
	int (*cmp)(st_list *, st_list *))
{
	st_list *cur_l1, *cur_l2, *next;

	cur_l1 = l1->next;
	cur_l2 = l2->next;
	init_list(res);
	while (1) {
		/* list l1 empty, finish depiling list l2 */
		if (cur_l1 == l1) {
			while (cur_l2 != l2) {
				next = cur_l2->next;
				list_add_tail(cur_l2, res);
				cur_l2 = next;
			}
			return;
		}
		/* list l2 empty, finish depiling list l1 */
		if (cur_l2 == l2) {
			while (cur_l1 != l1) {
				next = cur_l1->next;
				list_add_tail(cur_l1, res);
				cur_l1 = next;
			}
			return;
		}
		if (cmp(cur_l1, cur_l2) > 0) {
			next = cur_l1->next;
			list_add_tail(cur_l1, res);
			cur_l1 = next;
		} else {
			next = cur_l2->next;
			list_add_tail(cur_l2, res);
			cur_l2 = next;
		}
	}
}

void list_sort(st_list *list, int (*cmp)(st_list *, st_list *))
{
	st_list right, left, *r, *l, *next;
	/* zero or one elem, return */
	if (list->next == list || list->next->next == list)
		return;
	/* split list in right and left lists
	* we do not bother updating ->prev pointers, list_merge will
	**/
	l = list->next;
	r = l->next;
	left.next  = l;
	right.next = r;

	while (1) {
		if (r->next == list) {
			r->next = &right;
			l->next = &left;
			break;
		}
		l->next = r->next;
		l = l->next;
		if (l->next == list) {
			r->next = &right;
			l->next = &left;
			break;
		}
		r->next = l->next;
		r = r->next;
	}
	/* sort recursively; we don't care about the stack :) */
	list_sort(&right, cmp);
	list_sort(&left, cmp);

	list_merge(&right, &left, list, cmp);
}


struct ab {
	int j;
	int k;
	st_list list;
	int a;
};

int ab_cmp(st_list *l1, st_list *l2)
{
	struct ab *a1, *a2;

	a1 = container_of(l1, struct ab, list);
	a2 = container_of(l2, struct ab, list);
	return a1->a - a2->a;

}

void print_list(st_list *head) {
	struct st_list *l;
	struct ab *a;

	LIST_FOR_EACH(l, head) {
		a = container_of(l, struct ab, list);
		printf("%d ", a->a);
	}
	printf("\n");
}

void test_sort_one(int n) {
	struct ab *a = malloc(n * sizeof(struct ab));
	struct ab *c;
	st_list head, *l;
	int i, prev;

	init_list(&head);
	for (i = 0; i < n; i++) {
		a[i].a = rand() %1000;
		list_add(&a[i].list, &head);
	}
	list_sort(&head, &ab_cmp);
	prev = 10000;
	LIST_FOR_EACH(l, &head) {
		c = container_of(l, struct ab, list);
		if (prev < c->a) {
			printf("FAIL %d < %d\n", prev, c->a);
			print_list(&head);
			return;
		}
		prev = c->a;
	}
	free(a);
	printf("PASS %d\n", n);
}

void test_sort(int n)
{
	int toto;

	srand(time(NULL));
	for (toto = 0; toto < n; toto++)
		test_sort_one(rand()%1000000);
}

int main(int argc, char **argv)
{
	struct ab a, b, c,d;
	struct ab e, f, g,h, i;
	int toto;

	st_list list, list2, res;
	init_list(&list);
	init_list(&list2);
	a.a = 10111;
	b.a = 1664;
	c.a = 3;
	e.a = 6;
	f.a = 11;
	g.a = 124;
	h.a = 1379;
	i.a = 1664;
	list_add_tail(&a.list, &list);
	list_add(&b.list, &list);
	list_add(&c.list, &list);
	list_add(&d.list, &list);
	list_add(&e.list, &list);
	list_add(&f.list, &list);
	list_add(&g.list, &list);
	list_add(&h.list, &list);
	list_add(&i.list, &list);
	print_list(&list2);
	printf("len=%d\n", list_length(&list));
	test_sort(1000);
}