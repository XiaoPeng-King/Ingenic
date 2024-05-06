#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include "system_call.h"
#include "dbus/dbus_socket_common.h"

#define DBUS_SYSTEM_DAEMON_NAME "dbus-system.call.daemon"
#define SYSTEM_DAEMON_NAME "system.call.daemon"

typedef struct __main_info {
	SystemCallDbusInfo *dbus_info;
} SystemCallMainInfo;

static SystemCallMainInfo g_mainInfo = {0};
static int g_dbus_register = 0;
static pthread_mutex_t g_dbus_mutex = PTHREAD_MUTEX_INITIALIZER;

static inline int system_call_Lock(void)
{
	pthread_mutex_lock(&g_dbus_mutex);
	return 0;
}

static inline int system_call_unLock(void)
{
	pthread_mutex_unlock(&g_dbus_mutex);
	return 0;
}

static int systemCall_Check_DbusDaemon(void)
{
	int ret = 0;

	DIR *dir;
	struct dirent *dirent;

	dir = opendir("/tmp");
	if(dir == NULL) {
		printf("opendir() error: /tmp/ \n");
		return -1;
	}

	while((dirent = readdir(dir))) {
		if(strncmp(DBUS_SYSTEM_DAEMON_NAME, dirent->d_name, strlen(DBUS_SYSTEM_DAEMON_NAME)) == 0) {
			ret = 1;
			break;
		}
	}

	closedir(dir);
	return ret;
}

int system_call_init(void)
{
	int timeout = 1000;
	int ret = -1;

	system_call_Lock();
	if(g_dbus_register == 0) {
		memset(&g_mainInfo, 0x0, sizeof(g_mainInfo));
		g_mainInfo.dbus_info = SystemCall_DbusInfo_Alloc(0);
		if(g_mainInfo.dbus_info == NULL) {
			printf("SystemCall_DbusInfo_Alloc error\n");
			system_call_unLock();
			return -1;
		}
		sprintf(g_mainInfo.dbus_info->name, "system.call.%d", rand());
		strcpy(g_mainInfo.dbus_info->dst_name, SYSTEM_DAEMON_NAME);

		while(timeout--) {
			if(systemCall_Check_DbusDaemon() == 1) {
				break;
			}
			usleep(10000);
		}

		if(timeout <= 0) {
			printf("Please run system_call_daemon first\n");
			SystemCall_DbusInfo_Free(g_mainInfo.dbus_info);
			system_call_unLock();
			return -1;
		}

		ret = SystemCall_Dbus_Register(g_mainInfo.dbus_info);
		if(ret != 0) {
			printf("SystemCall_Dbus_Register system_call_client error\n");
			SystemCall_DbusInfo_Free(g_mainInfo.dbus_info);
			system_call_unLock();
			return -1;
		}

		g_dbus_register = 1;
		usleep(10000);
	}

	system_call_unLock();
	return 0;
}

int system_call_exit(void)
{
	int ret = -1;

	system_call_Lock();
	if(g_dbus_register == 1) {
		ret = SystemCall_Dbus_UnRegister(g_mainInfo.dbus_info);
		if(ret != 0) {
			printf("SystemCall_Dbus_UnRegister system_call_client error\n");
		}

		SystemCall_DbusInfo_Free(g_mainInfo.dbus_info);

		g_dbus_register = 0;
	}

	system_call_unLock();
	return 0;
}

int system_call(char *cmd, int timeout_ms)
{
	int i = 0;
	int ret = -1;
	char result[256] = "";

	if(g_dbus_register == 0) {
		printf("please run system_call_init first\n");
		return -1;
	}

	ret = SystemCall_Dbus_Call_Method(g_mainInfo.dbus_info, SYSTEM_DAEMON_NAME, "system_call", cmd, strlen(cmd), result, timeout_ms);
	if(ret < 0) {
		printf("SystemCall_Dbus_Call_Method systemcall %s failed\n", cmd);
	} else {
		ret = atoi(result);
	}

	return ret;
}

int popen_call(char *cmd, char *out, int max_size, int timeout_ms)
{
	int i = 0;
	int ret = -1;
	char result[DBUS_MSG_MAX_DATA_LEN] = "";

	if(g_dbus_register == 0) {
		printf("please run system_call_init first\n");
		return -1;
	}

	ret = SystemCall_Dbus_Call_Method(g_mainInfo.dbus_info, SYSTEM_DAEMON_NAME, "popen_call", cmd, strlen(cmd), result, timeout_ms);
	if(ret < 0) {
		printf("SystemCall_Dbus_Call_Method systemcall %s failed\n", cmd);
	} else {
		memcpy(out, result, max_size <= DBUS_MSG_MAX_DATA_LEN ? max_size : DBUS_MSG_MAX_DATA_LEN);
	}

	return ret;
}
