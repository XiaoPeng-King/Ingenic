#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "dbus/dbus_socket_common.h"

typedef struct __main_info {
	SystemCallDbusInfo *dbus_info;
} SystemCallMainInfo;

static SystemCallMainInfo g_mainInfo = {0};

static int system_call(char *args, int size, char *result, int max_size)
{
	int ret = 0;

	if(args != NULL) {
		printf("exec: %s\n", args);
		ret = system(args);
	}

	if(result != NULL) {
		sprintf(result, "%d", ret);
	}

	return strlen(result);
}

static int popen_call(char *args, int size, char *result, int max_size)
{
	int ret = 0;
	int nReadLen = 0;

	if(args != NULL) {
		FILE *pFile = NULL;

		printf("exec: %s\n", args);
		pFile = popen(args, "r");

		if (pFile != NULL) {
			nReadLen = fread(result, 1, max_size, pFile);
			pclose(pFile);
		} else {
			printf("popen error:%s\n", args);
			return -1;
		}

	}

	return nReadLen;
}

int main()
{
	int i = 0;
	int ret = 0;

	memset(&g_mainInfo, 0x0, sizeof(g_mainInfo));
	g_mainInfo.dbus_info = SystemCall_DbusInfo_Alloc(2);
	if(g_mainInfo.dbus_info == NULL) {
		printf("SystemCall_DbusInfo_Alloc error\n");
		return -1;
	}
	sprintf(g_mainInfo.dbus_info->name, "system.call.daemon");

	sprintf(g_mainInfo.dbus_info->interface[0]->interface, "system_call");
	sprintf(g_mainInfo.dbus_info->interface[0]->method, TYPE_METHOD);
	g_mainInfo.dbus_info->interface[0]->method_call = system_call;

	sprintf(g_mainInfo.dbus_info->interface[1]->interface, "popen_call");
	sprintf(g_mainInfo.dbus_info->interface[1]->method, TYPE_METHOD);
	g_mainInfo.dbus_info->interface[1]->method_call = popen_call;

	ret = SystemCall_Dbus_Register(g_mainInfo.dbus_info);
	if(ret != 0) {
		printf("SystemCall_Dbus_Register system_call_daemon error\n");
		return -1;
	}

	while(1)
		sleep(1);

	ret = SystemCall_Dbus_UnRegister(g_mainInfo.dbus_info);
	if(ret != 0) {
		printf("SystemCall_Dbus_UnRegister system_call_daemon error\n");
		return -1;
	}

	SystemCall_DbusInfo_Free(g_mainInfo.dbus_info);
	return 0;
}
