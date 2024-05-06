#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "system_call.h"

int main(int argc, char **argv)
{
	int ret = 0;
	char buf[1024] = "";

	if(argc != 3) {
		printf("usage: %s -s cmd #system\n", argv[0]);
		printf("usage: %s -p cmd #popen\n", argv[0]);
		return -1;
	}

	ret = system_call_init();
	if(ret < 0) {
		printf("system_call_init failed\n");
		return -1;
	}

	if(strcmp(argv[1], "-s") == 0) {
		printf("exec: %s\n", argv[2]);
		ret = system_call(argv[2], 10000);
		if(ret < 0) {
			printf("system_call failed\n");
			return -1;
		}
		printf("ret = %d\n", ret);
	}

	if(strcmp(argv[1], "-p") == 0) {
		printf("exec: %s\n", argv[2]);
		ret = popen_call(argv[2], buf, sizeof(buf), 10000);
		if(ret < 0) {
			printf("popen_call failed\n");
			return -1;
		}
		printf("ret = %d buf = %s\n", ret, buf);
	}

	ret = system_call_exit();
	if(ret < 0) {
		printf("system_call_exit failed\n");
		return -1;
	}

	return 0;
}
