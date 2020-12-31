#!/bin/bash

RESET="\033[22;49;39m"
RED="\033[0;31m"
GREEN="\033[1;32m"
BLUE="\033[1;34m"

TEST_BINARY="../target/run_test"
CONFIG_FILE=${1:-"/etc/host2ips"}
hosts_list=()   
tests_list=(
	# parse_test
	library_test
	# ssh_test
	# curl_test
	getent_test
	ping_test
)


parse_test(){
	echo -e "${BLUE}Running parse test...${RESET}"
	$TEST_BINARY parse "$CONFIG_FILE"
}
library_test(){
	echo -e "${BLUE}Running library test...${RESET}"

	ret_val=0
	for host in "${hosts_list[@]}"; do
		echo "$host: "
		"$TEST_BINARY" library "$host"
		local ret="$?"
		if [[ $ret -ne 0 ]]; then
			ret_val="$ret"
			break; 
		fi
	done
}
ssh_test() {
	:
}
curl_test() {
	:
}
getent_test() {
	echo -e "${BLUE}Running getent test...${RESET}"

	ret_val=0
	for host in "${hosts_list[@]}"; do
		getent hosts "$host"
		local ret="$?"
		if [[ $ret -ne 0 ]]; then
			ret_val="$ret"
			break; 
		fi
	done
}
ping_test() {
	echo -e "${BLUE}Running ping test...${RESET}"

	ret_val=0
	for host in "${hosts_list[@]}"; do
		ping -c 1 "$host" | head -n 1 | awk '{print $2 $3}'
		local ret="$?"
		if [[ $ret -ne 0 ]]; then
			ret_val="$ret"
			break; 
		fi
	done
}

main() {
	readarray -t hosts_list < <(cat "$CONFIG_FILE" | grep -v '^#' | grep host | awk '{print $2}')
	for func in "${tests_list[@]}"; do
		$func
	    if [[ $ret_val -eq 0 ]]; then
        	echo -e "${GREEN}Success!${RESET}"
			echo ""
        else
        	echo -e "${RED}Failed!${RESET}"
        	exit 1
	    fi
    done
    exit 0
}

main "$@"
