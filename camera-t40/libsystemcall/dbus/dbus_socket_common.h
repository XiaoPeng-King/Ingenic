#ifndef __DBUS_COMMON_H__
#define __DBUS_COMMON_H__

#include <pthread.h>
#include <sys/prctl.h>

/* Thread name */
#define SET_THREAD_NAME(name) (prctl(PR_SET_NAME, (unsigned long)name)) 

#define TYPE_SIGNAL "signal"
#define TYPE_METHOD "Method"

#define METHOD_RETURN "return"
#define METHOD_SUCCESS "y"
#define METHOD_FAILED  "n"

#define MAX_NAME_SIZE 64
#define MAX_INTERFACE_SIZE 64
#define MAX_INTERFACE_NUM 256
#define MAX_SIGNAL_SIZE 32
#define MAX_METHOD_SIZE 64

#define DEFAULT_METHOD_TIMEOUT 10000 // 10s
#define MAX_LISTEN_NUM 5

#define DBUS_MSG_HEAD 0x5aa55aa5
#define DBUS_MSG_MAX_DATA_LEN 4096

typedef struct __zrt_dbus_msg {
	unsigned int head;
	int rand;
	char src_name[MAX_NAME_SIZE];
	char dst_name[MAX_NAME_SIZE];
	int is_method;
	char interface[MAX_INTERFACE_SIZE];
	int ret;
	int size;
	unsigned char data[DBUS_MSG_MAX_DATA_LEN];
} SystemCallDbusMsg;

typedef struct _method_msg {
	SystemCallDbusMsg *msg;
	SystemCallDbusMsg *result;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	struct _method_msg *next;
} SystemCallDbusMethodMsgNode;

typedef struct __zrt_dbus_interface {
	char interface[MAX_INTERFACE_SIZE];
	char signal[MAX_SIGNAL_SIZE];
	void (*signal_call)(char *args, int size);
	char method[MAX_METHOD_SIZE];
	int (*method_call)(char *args, int size, char *result, int max_size);
} SystemCallDbusInterface;

typedef struct __zrt_dbus_info {
	char name[MAX_NAME_SIZE];
	char dst_name[MAX_NAME_SIZE];
	int interface_num;
	SystemCallDbusInterface *interface[MAX_INTERFACE_NUM];

	SystemCallDbusMethodMsgNode *method_head;
	int socket_server;
	int socket_client;
	char unix_domain[128];

	pthread_t methodThread_t;
	pthread_t eventThread_t;
	pthread_t listenThread_t;

	pthread_mutex_t dbus_mutex;
} SystemCallDbusInfo;

int SystemCall_Dbus_Register(SystemCallDbusInfo *dbusInfo);
int SystemCall_Dbus_UnRegister(SystemCallDbusInfo *dbusInfo);

SystemCallDbusInfo *SystemCall_DbusInfo_Alloc(int interfaceNum);
void SystemCall_DbusInfo_Free(SystemCallDbusInfo *dbusInfo);

int SystemCall_Dbus_Send_Signal(SystemCallDbusInfo *dbusInfo, char *server, char *interface, char * param, int param_size);
int SystemCall_Dbus_Call_Method(SystemCallDbusInfo *dbusInfo, char *server, char *interface, char * param, int param_size, char *result, int timeout); // timeout: -1 is default

#endif
