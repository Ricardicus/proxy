/* Server in the internet domain using TCP
   Port passed as an argument */
#include "backend.h"

volatile bool run_this_server_please_mister;

void * input_reader_callback(void * data)
{
	int c;
	bool keep_going = true;
	while ( ((c=fgetc(stdin)) != EOF) && keep_going){
		if ( c == (int) 'q' )
			keep_going = false;
	}

	run_this_server_please_mister = false;
	return NULL;
}

int main(int argc, char *argv[])
{
	int sockfd, proxy_port, settings_port, newsockfd_stack;
	socklen_t clilen; 
	time_t raw_time;
	struct tm * time_info;
	char client_IP[INET_ADDRSTRLEN];
	pthread_t proxy_callback_thread, proxy_settings_thread;
	struct sockaddr_in serv_addr, cli_addr;
	char * time_heap, * client_ip_heap, *time_c, *c_ptr;
	int * newsockfd;

	server_course_of_action_t proxy_action;
	server_course_of_action_t setting_action;

	run_this_server_please_mister = true;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0){ 
		log_error("ERROR opening socket");
		return EXIT_FAILURE;
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));

	if ( read_and_configure_proxy(&proxy_port, &settings_port) ) {
		// Read the configuration file
 		// Set the assigned ports
		// If this function returns a non-zero then there was no configuration
		// file
		printf("Please write a .config file specifying the ports to be used by this service.\n\
The file should be structured as follows:\n\
PROXY:[desired port for proxy handling]\n\
SETTING:[desired port for setting proxy domain over http]\n");
		return -1;
 	}

 	if ( http_init() < 0 ){
 		printf("Error: Failed to initiate the http handler. Program will not launch. Sorry!\n");
 		return -1;
 	}

	log("Proxy v.%s, c. %s %s\n",str(VERSION),__DATE__,__TIME__);

	proxy_action.callback = http_proxy_callback;
	proxy_action.port = proxy_port;
	proxy_action.type = PROXY;

	setting_action.callback = http_settings_callback;
	setting_action.port = settings_port;
	setting_action.type = SETTING;

 	pthread_create(&proxy_callback_thread, NULL, handle_incoming, (void*)&proxy_action);
 	pthread_create(&proxy_settings_thread, NULL, handle_incoming, (void*)&setting_action);

 	pthread_join(proxy_callback_thread, NULL);
 	pthread_kill(proxy_settings_thread, SIGINT);

	return EXIT_SUCCESS; 
}
