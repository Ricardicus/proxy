#include "proxy.h"

static proxy_domain_t * proxy_session = NULL;
static int proxy_port = 0;

int read_and_configure_proxy(int *proxy, int *setting) {
	char buffer[256];
	FILE * fp;
	int proxy_set = 0, setting_set = 0;

	fp = fopen(".config", "r");

	if ( fp == NULL )
		return NO_CONFIG_FILE;

	memset(buffer, '\0', sizeof buffer);

	while ( fgets(buffer, sizeof buffer , fp) != NULL ) {
		if ( ! strncmp(buffer, CONFIG_PROXY, strlen(CONFIG_PROXY)) ) {
			proxy_set = atoi(buffer + strlen(CONFIG_PROXY) );
			if ( proxy_set < WELL_KNOWN_LIMIT ) 
				return INVALID_PROXY_PORT;
		} else if ( ! strncmp(buffer, CONFIG_SETTING, strlen(CONFIG_SETTING)) ) {
			setting_set = atoi(buffer + strlen(CONFIG_SETTING) );
			if ( setting_set < WELL_KNOWN_LIMIT ) 
				return INVALID_PROXY_PORT;
		}
		memset(buffer, '\0', sizeof buffer);
	}

	fclose(fp);

	if ( setting_set < WELL_KNOWN_LIMIT || proxy_set < WELL_KNOWN_LIMIT )
		return INVALID_CONFIG_FILE;

	*proxy = proxy_set;
	*setting = setting_set;

	return 0; // marks success;
}

int configure_host_and_port(char* host, char * port_ch ) 
{
	if ( proxy_session == NULL ) 
		proxy_session = malloc(sizeof(proxy_session));

	snprintf(proxy_session->url, sizeof(proxy_session->url), "%s", host);
	proxy_session->port = atoi(port_ch);

	return 0;
}

void output_proxy_configuration_as_json(int socket) 
{
	char buffer[524], *msg;

	if ( proxy_session == NULL ) {
		msg = "{\"Host\":\"None configured\", \"Port\":\"None configured\", \"Proxy_Port\":\"None configured\"}";
		write(socket, msg, strlen(msg));
	}

	memset(buffer, '\0', sizeof buffer);

	snprintf(buffer, sizeof buffer, "{\"Host\":\"%s\",\"Port\":\"%d\",  \"Proxy_Port\": \"%d\"}", proxy_session->url, proxy_session->port, proxy_port);

	write(socket, buffer, strlen(buffer));
}

void * handle_incoming(void * data) 
{
	int sockfd, portno, newsockfd_stack;
	socklen_t clilen; 
	time_t raw_time;
	struct tm * time_info;
	char client_IP[INET_ADDRSTRLEN];
	struct sockaddr_in serv_addr, cli_addr;
	char * time_heap, * client_ip_heap, *time_c, *c_ptr;
	int * newsockfd;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){ 
		log_error("%s.%d ERROR opening socket", __func__, __LINE__);
		return NULL;
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));

	server_course_of_action_t * action = (server_course_of_action_t*) data;

	if ( action->type == PROXY )
		proxy_port = action->port;

	portno = action->port;

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		log_error("%s.%d ERROR on binding\n", __func__, __LINE__);
		return NULL;
	}

	clilen = sizeof(cli_addr);

	while ( 1 ){
		pthread_t callback_thread;
		listen(sockfd,20);

		newsockfd_stack = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

		// Getting time info
		time ( &raw_time);
		time_info = localtime( &raw_time );
		time_c = asctime(time_info);
		c_ptr = strchr(time_c, '\n');
		/* the time contains a '\n'.. */
		if (c_ptr != NULL )
			*c_ptr = '\0';

		// get client IP
		inet_ntop(AF_INET, &cli_addr.sin_addr ,client_IP, INET_ADDRSTRLEN);

		// heap allocated time
		time_heap = calloc(strlen(time_c)+1, 1);
		strcpy(time_heap, time_c);
		// heap allocated client ip
		client_ip_heap = calloc(strlen(client_IP)+1, 1);
		strcpy(client_ip_heap, client_IP);
		// sending socket info to newsockfd;
		newsockfd = malloc(sizeof(int));
		*newsockfd = newsockfd_stack;

		http_data_t * http_data = calloc(1, sizeof(http_data_t));
		http_data->client_ip = client_ip_heap;
		http_data->accept_time = time_heap;
		http_data->socket = newsockfd;

		if ( proxy_session == NULL ) {
			// None has been configured yet.
			proxy_session = malloc(sizeof(proxy_domain_t));

			if ( proxy_session == NULL ) {
				printf("%s.%d malloc failed", __func__, __LINE__);
				free_http_data(&http_data);
				return NULL;
			}

			snprintf(proxy_session->url, sizeof proxy_session->url, "%s", STD_PROXY_HOST);	
			proxy_session->port = STD_PROXY_PORT;

		} 
		
		http_data->proxy_session = proxy_session;

		pthread_create(&callback_thread, NULL, action->callback, (void*)http_data);

	}

	close(sockfd);

	return NULL;

}
