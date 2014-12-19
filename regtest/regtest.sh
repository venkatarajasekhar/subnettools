#! /bin/bash


PROG=../subnet-tools

reg_test() {
	local output_file
	local ref_file
	local conf_name

	if [ $1 = "-c" ]; then
		conf_name=$2
		shift
		shift
	else
		conf_name=st.conf
	fi
	output_file=`echo $@| sed s/\ /_/g | tr / -`
	echo -n "reg test [$@] : "
	if [ ! -f ref/$output_file ]; then
		echo "No ref file found for this test, creating it '$ref_file'"
		$PROG -c $conf_name $1 $2 $3 $4 > ref/$output_file	
		return
	fi
	$PROG -c $conf_name $1 $2 $3 $4 > res/$output_file
	diff res/$output_file ref/$output_file > /dev/null
	if [ $? -eq 0 ]; then
		echo -e "\033[32mOK\033[0m"
	else
		echo -e "\033[31mKO\033[0m"
	fi
}


# a CSV with strange fields names :)
reg_test -c st-bizarr.conf sort bizar.csv 
# a CSV with strange fields names, output more strange 
reg_test -c st-bizarr2.conf sort bizar2.csv
reg_test missing  BURP2 BURP
reg_test common  BURP2 BURP
reg_test subnetagg aggipv4
# this one should test enough IPv6 functionnality
reg_test subnetagg aggipv6
reg_test subnetagg bigcsv
reg_test routeagg route_aggipv6
reg_test routeagg route_aggipv6-2
reg_test routeagg route_aggipv4
reg_test simplify1 BURP
reg_test simplify2 BURP
reg_test simplify1 simple
reg_test simplify2 simple
reg_test sort aggipv4
reg_test simplify1 simplify1
reg_test simplify2 simplify1
reg_test sort aggipv6
reg_test sort sort1
reg_test sort sort1-ipv6
# removal
reg_test remove subnet 10.1.1.2/16  10.1.2.0/24
reg_test remove subnet 10.1.0.0/16  10.1.0.0/28
reg_test remove subnet 10.1.0.0/16  10.1.255.240/28
reg_test remove subnet 2001:db8::/32 2001:db8::/64
reg_test remove subnet 2001:db8::/32 2001:db8:a::/64
reg_test remove subnet 2001:db8::/32 2001:db8:ffff:ffff::/64
reg_test remove file route_aggipv6-2 2001:dbb::/64
reg_test remove file route_aggipv4 10.1.4.0/32
# converter
reg_test convert CiscoRouter iproute_cisco
reg_test convert CiscoRouter iproutecisco_ECMP 
reg_test convert IPSO iproute_nokia
reg_test convert CiscoNexus iproute_nexus
reg_test convert CiscoNexus iproutecisconexus_ECMP
reg_test convert CiscoRouter ipv6route2
reg_test grep mergeipv6 2001:db8::
