#ifndef PROXY_H
#define PROXY_H 

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#include "backend.h"

#define NO_CONFIG_FILE			1
#define INVALID_PROXY_PORT		2
#define INVALID_SETTINGS_PORT	3
#define INVALID_CONFIG_FILE		4

#define SETTING 				1
#define PROXY 					2

#define WELL_KNOWN_LIMIT		1024

#define STD_PROXY_HOST			"www.dn.se"
#define STD_PROXY_PORT			80

#define CONFIG_FILE 			".config"

typedef struct server_course_of_action_t {
	void *(*callback)(void*);
	int port;
	int type;
} server_course_of_action_t;

int read_and_configure_proxy(int *, int *);
void *handle_incoming(void *); 
void output_proxy_configuration_as_json(int);
int configure_host_and_port(char*, char*);

#ifdef __cpluspluss
}
#endif

#endif