#include <stdio.h>
#include <unistd.h>
#include "debug.h"
#include "iptools.h"
#include "st_printf.h"

int main(int argc, char **argv)
{
	int 		a1 = 1000;
	unsigned 	a2 = 4000000000UL;
	int 		a3 = 4000000000UL;
	int 		a4 = -123;
	unsigned short b = 65535;
	/* long result will vary */
	long		l1 = 200000000;
	long		l2 = 300000000;
	long		l3 = -1;
	unsigned long   l4 = -1;
	unsigned long   l5 = 200000000;
	short 		sh1 = 32767;
	short 		sh2 = 32768;
	unsigned short  sh3 = 0xffff;
	struct subnet s1, s2;
	char *string = "STRING1";
	char buffer1[6];
	char buffer2[18];

	get_subnet_or_ip("2.2.2.2/24", &s1);
	get_subnet_or_ip("2001:3:4::1/64", &s2);

	st_printf("TEST SHORT  : %hd %hd %hu\n", sh1, sh2, sh3);
	printf("TEST SHORT  : %hd %hd %hu\n", sh1, sh2, sh3);
	st_printf("TEST SHORT2 : '%06hd' '%20hd' '%-20hu'\n", sh1, sh2, sh3);
	printf("TEST SHORT2 : '%06hd' '%20hd' '%-20hu'\n", sh1, sh2, sh3);
	st_printf("TEST INT    : %d %u %d %d\n", a1, a2, a3, a4);
	printf("TEST INT    : %d %u %d %d\n", a1, a2, a3, a4);
	st_printf("TEST INT2   : '%06d' '%20u' '%-20d'\n", a1, a2, a3);
	printf("TEST INT2   : '%06d' '%20u' '%-20d'\n", a1, a2, a3);
	st_printf("TEST LONG   : %ld %ld %ld %lu %lu\n", l1, l2, l3, l4, l5);
	printf("TEST LONG   : %ld %ld %ld %lu %lu\n", l1, l2, l3, l4, l5);
	st_printf("TEST LONG2  : '%013ld' '%13ld' '%-4ld' '%lu' '%12lu'\n", l1, l2, l3, l4, l5);
	printf("TEST LONG2  : '%013ld' '%13ld' '%-4ld' '%lu' '%12lu'\n", l1, l2, l3, l4, l5);
	st_printf("TEST HEX1 : %x %hx %lx\n", a2, b, l5);
	printf("TEST HEX1 : %x %hx %lx\n", a2, b, l5);
	st_printf("TEST HEX2 : '%10x' %06hx %hx '%-20lx'\n", a2, sh1, sh2, l5);
	printf("TEST HEX2 : '%10x' %06hx %hx '%-20lx'\n", a2, sh1, sh2, l5);
	
	st_printf("TEST STRING :'%s';'%10s';'%-10s'\n", string, string, string);
	printf("TEST STRING :'%s';'%10s';'%-10s'\n", string, string, string);
	st_printf("TEST IP1    : %I-%10I : [%-20N - %20B]\n", s1, s2, s2, s2);
	st_printf("TEST IP2    : %P-%20P : [%-20N - %20B]\n", s2, s2, s2, s2);
	st_printf("TEST MASK   : %N : %m - %M \n", s1, s1, s1);
	
	st_snprintf(buffer1, sizeof(buffer1), "abcdefghijk");
	printf("TRUNC1 : '%s'\n", buffer1);
	snprintf(buffer1, sizeof(buffer1), "abcdefghijk");
	printf("TRUNC1 : '%s'\n", buffer1);
	st_snprintf(buffer2, sizeof(buffer2), "abcdefghijk %040d", a1);
	printf("TRUNC2 : '%s'\n", buffer2);
	snprintf(buffer2, sizeof(buffer2), "abcdefghijk %040d", a1);
	printf("TRUNC2 : '%s'\n", buffer2);
	st_snprintf(buffer2, sizeof(buffer2), "abcdefghijk %-40d", a1);
	printf("TRUNC3 : '%s'\n", buffer2);
	snprintf(buffer2, sizeof(buffer2), "abcdefghijk %-40d", a1);
	printf("TRUNC3 : '%s'\n", buffer2);
	return 1;
}
