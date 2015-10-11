#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "st_options.h"
#include "prog-main.h"

extern char *default_fmt;

void usage()
{
	printf("Usage: %s [OPTIONS] COMMAND ARGUMENTS ....\n", PROG_NAME);
	printf("\n");
	printf("\nCOMMAND := \n");
	printf("Subnet arithmetic\n");
	printf("-----------------\n");
	printf("relation IP1 IP2    : prints a relationship between IP1 and IP2\n");
	printf("split S, <l1,l2,..> : split subnet S l1 times, the result l2 times, and so on..\n");
	printf("split2 S, <m1,m2,..>: split subnet S with mask m1, then m2, and so on...\n");
	printf("removesub TYPE O1 S1: remove Subnet S from Object O1; if TYPE=file O1=ile, if TYPE=subnet 01=subnet\n");
	printf("removefile F1 F2    : remove all F2 subnets from F1\n");
	printf("ipinfo IP|all|IPvX  : prints information about IP, or all known subnets (all, IPv4 or IPv6)\n");
	printf("\n");
	printf("Route file simplification\n");
	printf("-------------------------\n");
	printf("sort FILE1          : sort CSV FILE1\n");
	printf("sortby name file    : sort CSV file by (prefix|gw|mask), prefix is always a tie-breaker\n");
	printf("sortby help	    : print available sort options\n");
	printf("subnetagg FILE1     : sort and aggregate subnets in CSV FILE1; GW is not checked\n");
	printf("routeagg  FILE1     : sort and aggregate subnets in CSV FILE1; GW is checked\n");
	printf("simplify1 FILE1     : simplify CSV subnet file FILE1; duplicate or included networks are removed; GW is checked\n");
	printf("simplify2 FILE1     : simplify CSV subnet file FILE1; prints redundant routes that can be removed\n");
	printf("\n");
	printf("Route file comparison\n");
	printf("---------------------\n");
	printf("compare FILE1 FILE2 : compare FILE1 & FILE2, printing subnets in FILE1 INCLUDED in FILE2\n");
	printf("missing FILE1 FILE2 : prints subnets from FILE1 that are not covered by FILE2; GW is not checked\n");
	printf("uniq FILE1 FILE2    : prints unique subnets from FILE1 and FILE2\n");
	printf("common FILE1 FILE2  : merge CSV subnet files FILE1 & FILE2; prints common routes only; GW isn't checked\n");
	printf("addfiles FILE1 FILE2: merge CSV subnet files FILE1 & FILE2; prints the sum of both files\n");
	printf("grep FILE prefix    : grep FILE for prefix/mask\n");
	printf("filter FILE EXPR    : grep netcsv   FILE using regexp EXPR\n");
	printf("filter help         : prints help about bgp filters\n");
	printf("bgpfilter FILE EXPR : grep bgp_file FILE using regexp EXPR\n");
	printf("bgpfilter help      : prints help about bgp filters\n");
	printf("\n");
	printf("IPAM tools\n");
	printf("----------\n");
	printf("ipam <IPAM> FILE1   : load IPAM, and print FILE1 subnets with comment extracted from IPAM\n");
	printf("ipamfilter FILE EXPR: load IPAM, and filter using regexp EXPR\n");
	printf("ipamprint FILE      : print IPAM; use option -ipamea to select Extended Attributes\n");
	printf("getea <IPAM> FILE   : print FILE with Extended Attributes retrieved from IPAM\n");
	printf("\n");
	printf("Miscellaneous route file tools\n");
	printf("------------------------------\n");
	printf("print FILE1         : just read & print FILE1; use a -fmt FMT to print CSV fields you want\n");
	printf("bgpprint FILE1      : just read & print BGP FILE1; use a -fmt FMT to print CSV fields you want\n");
	printf("sum IPv4FILE        : get total number of hosts included in the list of subnets\n");
	printf("sum IPv6FILE        : get total number of /64 subnets included\n");
	printf("\n");
	printf("BGP route file tools\n");
	printf("--------------------\n");
	printf("bgpcmp file1 file2  : show what changed in BGP file file1 & file2\n");
	printf("bgpsortby name file : sort BGP file by (prefix|gw|MED|LOCALPREF), prefix is always a tie-breaker\n");
	printf("bgpsortby help	    : print available sort options\n");
	printf("\n");
	printf("IP route to CSV converters\n");
	printf("--------------------------\n");
	printf("convert PARSER FILE1: convert FILE1 to csv using parser PARSER\n");
	printf("convert help        : use '%s convert help' for available parsers \n", PROG_NAME);
	printf("\n");
	printf("DEBUG and help\n");
	printf("--------------\n");
	printf("echo FMT ARG2       : try to get subnet from ARG2 and echo it according to FMT\n");
	printf("scanf STRING1 FMT   : scan STRING1 according to scanf-like format FMT\n");
	printf("fscanf FILE   FMT   : scan FILE according to scanf-like format FMT\n");
	printf("confdesc            : print a small explanation of %s configuration file\n", PROG_NAME);
	printf("help                : This HELP \n");
	printf("version             : %s version \n", PROG_NAME);
	printf("\nOPTIONS := \n");
	printf("-d <delim>      : change the default field delim (;) \n");
	printf("-ipamea EA1,EA2 : load Extended Attributes in IPAM files; use ',' to select more\n");
	printf("-c <file >      : use config file <file>  instead of st.conf\n");
	printf("-o <file >      : write output in <file> \n");
	printf("-rt		: when converting routing table, set route type as comment\n");
	printf("-ecmp		: when converting routing table, print all routes in case of ECMP\n");
	printf("-noheader|-nh	: dont print netcsv header file\n");
	printf("-grep_field N   : grep field N only\n");
	printf("-D <debug>      : DEBUG MODE ; use '%s -D help' for more info\n", PROG_NAME);
	printf("-fmt            : change the output format (default :%s)\n", default_fmt);
	printf("-V              : verbose mode; same as '-D all:1'\n");
	printf("-VV             : more verbose mode; same as '-D all:1'\n\n");
	printf("INPUT CSV format :=\n");
	printf("- Input subnet/routes files SHOULD have a CSV header describing its structure (prefix, mask,, GW, comment, etc...)\n");
	printf("- Input subnet/routes files without a CSV header are assumed to be : prefix;mask;GW;comment or prefix;mask;comment\n");
	printf("- default CSV header is 'prefix;mask;device;GW;comment'\n");
	printf("- CSV header can be changed by using the configuration file (subnettools confdesc for more info)\n");
	printf("- Input IPAM CSV MUST have a CSV header; there is a defaut header, but it is derived from my company's one\n");
	printf("- So IPAM CSV header MUST be described in the configuration file\n");
}

void debug_usage()
{
	printf("Usage: %s -D Debugs [OPTIONS] COMMAND FILES \n", PROG_NAME);
	printf("Debugs looks like : symbol1:level,symbol2:level;...\n");
	printf("symbol is the  symbol to  debug, increasing level gives more info, ie :\n");
	printf("level 1 : get more infos on errors (badly formatted file, bad IP etc...)\n");
	printf("level 4+ : print  internal program debug info ; wont tell you anything without source code \n");
	printf("level 9 : if implemented, print info for each loop of the program \n");
	printf("Example 1 :\n");
	printf("'%s -D memory:1,parseopts:2,trucmuche:3 simplify1 MYFILE'\n", PROG_NAME);
	printf("Example 2 :\n");
	printf("'%s -D loadcsv:3 sort MYFILE' will tell you every invalid line in your file.\n\n", PROG_NAME);
	printf("Available symbols :\n");
	list_debugs();
}