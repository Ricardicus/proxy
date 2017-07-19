#ifdef __cplusplus
extern "C" {
#endif 

#ifndef HTTP_H
#define HTTP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "http_util.h"
#include "hashtable.h"
#include "logger.h"
#include "notif.h"
#include "proxy.h"

#define xstr(a) 		#a
#define str(a)			xstr(a)
#define CGI_PATH		data.cgi

#define BACKEND_MAX_BUFFER_SIZE			300
#define HTTP_MAX_BUFER_SIZE				4096
#define BACKEND_MAX_NBR_OF_ARGS			20
#define BACKEND_MAX_ARRAY_SIZE			100
#define VERSION							1
#define MAXIMUM_READ_SIZE				(1<<22)
#define BUFFER_INCREMENT				(1<<10)
#define STACK_BUFFER_SIZE	(1<<12)
#define MAX_LINE_SIZE		(512)

#define SETTINGS_FILE		"index.html"
#define NOT_FOUND_FILE		"html/404.txt"


typedef struct proxy_domain_t {
	char url[256];
	int port;
} proxy_domain_t;

typedef struct http_data_s {
	int * socket;
	char * client_ip;
	char * accept_time;
	proxy_domain_t * proxy_session;
} http_data_t;

typedef int (*http_header_callback_t) (int);

void free_http_data(http_data_t **);


int http_init(void);
void output_path(int, const char *);
void output_host_and_port(int);
int configure_host_and_port(char *, char *); 
void * http_proxy_callback(void *);
void * http_settings_callback(void *);
void parse_http_get_headers_and_arguments(hashtable_t *, char *, size_t);

/* HTTP-Headers */
void output_file_not_found(int);
int output_internal_server_error(int);
int output_html_headers(int);
int output_css_headers(int);
int output_jpg_headers(int);
int output_json_headers(int);
int output_woff2_headers(int);
void output_file_transfer_headers(int,char*);
int output_js_headers(int);
int output_txt_headers(int);


#endif

#ifdef __cplusplus
}
#endif