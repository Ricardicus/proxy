#include "http.h"

static hashtable_t *headers_callback;

/*
* A bunch of functions that output meta data
*/ 
// ----------------------------------------- //
int output_js_headers(int socket)
{
	size_t ctrl;
	char *msg;
	size_t sz;

	msg = "HTTP/1.1 200 OK\r\n\
Cache-Control: no-cache, no-store, must-revalidate\r\n\
Pragma: no-cache\r\n\
Expires: 0\r\n\
Content-Type: application/javascript; charset=utf-8\r\n\r\n";
	sz = strlen(msg);

   	ctrl = write(socket,msg, sz);
   	
   	return  ctrl != sz;
}

int output_authenticate_headers(int socket)
{

	size_t ctrl;
	char *msg;
	size_t sz;

	msg = "HTTP/1.1 401 Unauthorized\r\n\
Cache-Control: no-cache, no-store, must-revalidate\r\n\
Pragma: no-cache\r\n\
Expires: 0\r\n\
WWW-Authenticate: Basic realm=\"private\"\r\n\
Content-Type: text/html; charset=utf-8\r\n\r\n\
<!DOCTYPE html><body><pre>Authorization required.</pre></body></html>";
	sz = strlen(msg);

   	ctrl = write(socket,msg, sz);
   	
   	return  ctrl != sz;

}

void output_file_transfer_headers(int socket, char *file)
{
	/*
	* Important: the file udner path "file" must be a regular file and exist!
	*/
	unsigned sz;
	struct stat st;
	char http_line[200], *msg, *ch, *slash;
	
	msg = "HTTP/1.1 200 OK\r\n\
Cache-Control: no-cache, no-store, must-revalidate\r\n\
Pragma: no-cache\r\n\
Expires: 0\r\n\
Content-Type: application/octet-stream\r\n";
   	write(socket,msg, strlen(msg));

   	stat(file, &st);
   	sz = st.st_size;

	memset(http_line, '\0', sizeof http_line);
	snprintf(http_line, sizeof(http_line) - 1, "Content-Length: %u\r\n", sz);

   	write(socket,http_line, strlen(http_line));

	memset(http_line, '\0', sizeof http_line);

	ch = file;
	while ( *ch ) {
		if ( *ch == '/' )
			slash = ch;
		++ch;
	}

	file = slash+1;
	*slash = '\0';

	snprintf(http_line, sizeof(http_line) - 1, "Content-Disposition: attachment; filename=\"%s\"\r\n\r\n", file);
   	write(socket,http_line, strlen(http_line));
}

int output_txt_headers(int socket)
{
	size_t ctrl;
	char *msg;
	size_t sz;

	msg = "HTTP/1.1 200 OK\r\n\
Cache-Control: no-cache, no-store, must-revalidate\r\n\
Pragma: no-cache\r\n\
Expires: 0\r\n\
Content-Type: text/plain; charset=utf-8\r\n\r\n";
	sz = strlen(msg);

   	ctrl = write(socket,msg, sz);
   	
   	return  ctrl != sz;
}

int output_json_headers(int socket)
{
	size_t ctrl;
	char *msg;
	size_t sz;

	msg = "HTTP/1.1 200 OK\r\n\
Cache-Control: no-cache, no-store, must-revalidate\r\n\
Pragma: no-cache\r\n\
Expires: 0\r\n\
Content-Type: application/json; charset=utf-8\r\n\r\n";
	sz = strlen(msg);

   	ctrl = write(socket,msg, sz);
   	
   	return  ctrl != sz;
}

int output_jpg_headers(int socket)
{
	ssize_t ctrl;
	char *msg;
	size_t sz;

	msg = "HTTP/1.1 200 OK\r\n\
Content-Description: File Transfer\r\n\
Content-Transfer-Encoding: binary\r\n\
Content-Disposition: attachment; filename=\"image.jpg\"\r\n\
Content-Type: image/jpeg\r\n\r\n";
	sz = strlen(msg);

   	ctrl = write(socket,msg, sz);
   	
   	return  ctrl != sz;
}

int output_png_headers(int socket)
{
	ssize_t ctrl;
	char *msg;
	size_t sz;

	msg = "HTTP/1.1 200 OK\r\n\
Content-Description: File Transfer\r\n\
Content-Transfer-Encoding: binary\r\n\
Content-Disposition: attachment; filename=\"image.jpg\"\r\n\
Content-Type: image/png\r\n\r\n";
	sz = strlen(msg);

   	ctrl = write(socket,msg, sz);
   	
   	return  ctrl != sz;
}

int output_css_headers(int socket)
{
	ssize_t ctrl;
	char *msg;
	size_t sz;

	msg = "HTTP/1.1 200 OK\r\n\
Cache-Control: no-cache, no-store, must-revalidate\r\n\
Pragma: no-cache\r\n\
Expires: 0\r\n\
Content-Type: text/css; charset=utf-8\r\n\r\n";
	sz = strlen(msg);

   	ctrl = write(socket,msg, sz);
   	
   	return  ctrl != sz;
}

int output_woff2_headers(int socket)
{
	ssize_t ctrl;
	char *msg;
	size_t sz;

	msg = "HTTP/1.1 200 OK\r\n\
Cache-Control: no-cache, no-store, must-revalidate\r\n\
Pragma: no-cache\r\n\
Expires: 0\r\n\
Content-Type: application/font-woff2; charset=utf-8\r\n\r\n";
	sz = strlen(msg);

   	ctrl = write(socket,msg, sz);
   	
   	return  ctrl != sz;
}

int output_html_headers(int socket)
{
	ssize_t ctrl;
	char *msg;
	size_t sz;

	msg = "HTTP/1.1 200 OK\r\n\
Cache-Control: no-cache, no-store, must-revalidate\r\n\
Pragma: no-cache\r\n\
Expires: 0\r\n\
Content-Type: text/html; charset=utf-8\r\n\r\n";
	sz = strlen(msg);

   	ctrl = write(socket,msg, sz);

   	return  ctrl != sz;
}

/*
* The file requested was not found on the server.. Outputs some fantastic ASCII art.
*/
void output_file_not_found(int socket)
{
	unsigned sz;
	ssize_t ctrl;
	size_t send_sz;
	int fd;
	char *data;
	struct stat st;

	char * msg = "HTTP/1.1 404 Not Found\r\n\
Cache-Control: no-cache, no-store, must-revalidate\r\n\
Pragma: no-cache\r\n\
Expires: 0\r\n\
Content-Type: text/html; charset=utf-8\r\n\r\n\
<!DOCTYPE html><body><pre>";
	send_sz = strlen(msg);

   	ctrl = write(socket,msg, send_sz);
   
   	if ( send_sz != ctrl ){
   		return;
   	}

   	fd = open(NOT_FOUND_FILE, O_RDONLY);

	stat(NOT_FOUND_FILE, &st);
	sz = st.st_size;

   	fd = open(NOT_FOUND_FILE, O_RDONLY);
   	
   	if ( fd < 0 ){
		// sending a signal that it is no file with that name present.
		msg = "ERROR!!!?!!! (no std 404 text).";
   		write(socket,msg, strlen(msg));
		return;
	}

	data = malloc(sz+1);
	if ( data == NULL ){
		msg = "Memory error + Bad path.";
		send_sz = strlen(msg);
   		ctrl = write(socket,msg, send_sz);
		close(fd);

		if ( ctrl == send_sz ){
			// send the rest also..
			msg = "</pre></body></html>";
   			write(socket,msg, strlen(msg));
		}

		return;
	}

	data[sz] = '\0';

	read( fd, data, sz );
	ctrl = write(socket, data, sz);

	close(fd);
	free(data);

	if ( ctrl == sz ) {
		msg = "</pre></body></html>";
   		write(socket,msg, strlen(msg));
	}

	return;

}

int output_internal_server_error(int socket)
{
	int ctrl;
	char *msg;
	size_t sz;

	msg = "HTTP/1.1 500 Internal server error\r\n\
Cache-Control: no-cache, no-store, must-revalidate\r\n\
Pragma: no-cache\r\n\
Expires: 0\r\n\
Content-Type: text/html; charset=utf-8\r\n\r\n\
<!DOCTYPE html><body><pre>\
Internal server error.\
</pre></body></html>";
	sz = strlen(msg);

   	ctrl = write(socket,msg, sz);

   	return  ctrl != sz;
}

/*
* Says goodbye to some heap allocated memory 
*/
void free_http_data(http_data_t ** http_data)
{
	free((*http_data)->client_ip);
	free((*http_data)->accept_time);
	free((*http_data)->socket);
	free((*http_data));
	*http_data = NULL;
}


int http_init()
{
	headers_callback = new_hashtable(15, 0.75);

	if ( headers_callback == NULL )
		return -1;

	put(headers_callback, "js", output_js_headers);
	put(headers_callback, "css", output_css_headers);
	put(headers_callback, "json", output_json_headers);
	put(headers_callback, "html", output_html_headers);
	// Map all kinds of things to jpg for this server..
	put(headers_callback, "jpg", output_jpg_headers);
	put(headers_callback, "png", output_png_headers);
	put(headers_callback, "jpeg", output_jpg_headers);
	put(headers_callback, "gif", output_jpg_headers);
	put(headers_callback, "ico", output_jpg_headers);
	put(headers_callback, "woff2", output_woff2_headers);

	return 0;
}

/*
* Parsing the data from a http request
*
* params - hash containg all keys relevant for the request
* buffer - data to the http datagram
*/
void parse_http_get_headers_and_arguments(hashtable_t * params, char * buffer, size_t size)
{
	char line_buffer[MAX_LINE_SIZE], *end, *start, *c, *url, *path,
			*args_start, *args_end, *version;
	int first = 1;

	memset(line_buffer, '\0', MAX_LINE_SIZE);
	start = buffer;
	end = strchr(start, '\n');

	while ( end != NULL ) {
		*end = '\0';

		snprintf(line_buffer, sizeof(line_buffer) - 1, "%s", start);

		if ( first ) {
			// Assumes: GET /resource.type?arg1=value1&arg2=value2 HTTP/1.1 
			// The command (GET/POST/HEAD) will get passed env. variable: COMMAND
			// The URL to the env. variable: URL
			// Arguments will be passed as is, but with capital letters and they will be
			// expanded to their values respectively, e.g. ARG1=value1.. 
			// HTTP/1.1 will be loaded to VERSION, e.g. VERSION=HTTP/1.1
			// * Exception:
			//		when path only is '/' as URL, it will be translated into 'index.html'.

			c = strchr(line_buffer, ' ');
			if ( c != NULL ) {
				*c = '\0';
				put(params, strdup("COMMAND"), strdup(line_buffer));

				*c = ' ';
				++c;

				url = c;
				version = strchr(c, ' ');

				if ( *c != '\0' && (c = strchr(c, ' ')) != NULL ) {

					*c = '\0';

					put(params, strdup("URL"), strdup(url));

					*c = ' ';
					++c;

					path = url;

					if ( (c = strchr(path, '?')) != NULL ) {

						*c = '\0';

						if ( !strncmp(path, "/ ", 2) ) {
							put(params, strdup("PATH"), strdup("index.html"));
						} else {
							put(params, strdup("PATH"), strdup(path+1));
						}

						// Continue with parsing the arguments
						*c = '?';
						c++; // now it points to the start of arg=value[&args=values..]

						args_start = c;
						args_end = strchr(args_start, '&');

						if ( args_end != NULL ) {

							while ( args_end != NULL ) {

								*args_end = '\0';

								c = strchr(args_start, '=');
								if ( c != NULL ) {
									*c = '\0';

									put( params, strdup(args_start), strdup(c+1) );

								}

								*args_end = '&';

								args_start = args_end+1;
								args_end = strchr(args_start, '&');

								if ( args_end == NULL ) {

									if ( (c = strchr(args_start, '=')) != NULL ) {

										*c = '\0';

										char * p = strchr(c+1, ' ');
										if ( p != NULL )
											*p = '\0';

										put(params, strdup(args_start), strdup(c+1) );

										if ( p != NULL )
											*p = ' ';

									}	

								} 

							}

						} else if ( (c = strchr(args_start, '=')) != NULL ) {

							*c = '\0';
							char * p = strchr(c+1, ' ');
							if ( p != NULL )
								*p = '\0';

							put(params, strdup(args_start), strdup(c+1) );

							if ( p != NULL )
								*p = ' ';

						}


					} else if ( (c = strchr(path, ' ')) != NULL ) {

						*c = '\0';
						if ( !strncmp(path, "/", 2) ) {
							put(params, strdup("PATH"), strdup("index.html"));
						} else {
							put(params, strdup("PATH"), strdup(path+1));
						}

					}

				}

				if ( version != NULL ) {

					++version;
					if ( ( c = strchr(version, '\r') ) != NULL )
						*c = '\0';
					
					put(params, strdup("VERSION"), strdup(version));

				}

			}

			first = 0;
		
		} else {

			if ( ( c = strchr(line_buffer, ':') ) != NULL ) {
				*c = '\0';
				char * clean;
				if ( (clean=strchr(c+2, '\r')) != NULL )
					*clean = '\0';
				if ( (clean=strchr(c+2, '\n')) != NULL )
					*clean = '\0';

				put( params, strdup(line_buffer), strdup(c+2) );
			}

		}

		end++;
		start = end;
		end = strchr(end, '\n');

		memset(line_buffer, '\0', sizeof line_buffer);
	}
}


/*
* When a get request contain an URL that is a file in the 
* accessible file system, this function is responsible for 
* outputting the file.
* 
* params:
* 	@socket - tcp/ip api to write data to
* 	@path - URL of the get request
*/
void output_path(int socket, const char * path)
{
	http_header_callback_t callback;
	char *msg, *data; 
	long int sz;
	size_t msg_sz;
	const char * file_type;
	int fd, ctrl;
	struct stat st;

	// Mapping '/' to the file 'index.html'
 	if ( ! strncmp(path, "/", 2) ){

		output_html_headers(socket);
		stat("index.html", &st);
		sz = st.st_size;

	   	fd = open("index.html", O_RDONLY);
	   	
	   	if ( fd < 0 ){
			// sending a signal that it is no file with that name present.
			output_file_not_found(socket);
			return;
		}

		data = malloc(sz+1);
		if ( data == NULL ){
			printf("NO DATA!!!!!!\n");
			output_file_not_found(socket);
			close(fd);
			return;
		}

		data[sz] = '\0';

		read( fd, data, sz );
   		write(socket, data, sz);

		close(fd);
		free(data);
		return;
	}

	// if someone asks for the icon, i placed it under etc 
	// so I need to add this little exception..
	if ( strstr(path, "favicon.ico") != NULL ){

		output_jpg_headers(socket);
		stat("etc/favicon.ico", &st);
		sz = st.st_size;

	   	fd = open("etc/favicon.ico", O_RDONLY);
	   	
	   	if ( fd < 0 ){
			// sending a signal that it is no file with that name present.
			output_file_not_found(socket);
			return;
		}

		data = malloc(sz+1);
		if ( data == NULL ){
			output_file_not_found(socket);
			close(fd);
			return;
		}

		data[sz] = '\0';

		read( fd, data, sz );
   		write(socket, data, sz);

		close(fd);
		free(data);
		return;
	}

	// hiding some "sensitive" information
	if ( (strstr(path, "/etc/") != NULL) ||( strstr(path, "/src/") != NULL )
			||(strstr(path, "/log/") != NULL) ||(strstr(path, "~") != NULL) 
				|| (strstr(path, "..") != NULL)) {
		
		output_file_not_found(socket);
		
		return;
	} 

	/*
	* Outputs file if it exists
	*/

	fd = open(path, O_RDONLY);
	if ( fd < 0 ){
		// sending a signal that it is no file with that name present.
		output_file_not_found(socket);
		return;
	}
	
	file_type = path;
	
	while (*file_type && *file_type != '.'){
		file_type++;
	}

	if ( !*file_type ){
		// if the file type_was not found.. 
		log("error: file type of requested resource %s was not found.\n", path);
		
		output_file_not_found(socket);
		close(fd);
   		return;
	}

	file_type++;

	callback = (http_header_callback_t) get(headers_callback, file_type);

	stat(path, &st);
	sz = st.st_size;

	data = malloc(sz+1);
	if ( data == NULL ){
		output_file_not_found(socket);
		close(fd);
		return;
	}

	data[sz] = '\0';

	read( fd, data, sz );

	if ( callback != NULL ){
		if ( callback(socket) ) {
			// Something went wrong, not all bytes were sent.
			close(fd);
			free(data);
			return;
		}
   		write(socket, data, sz);
   		close(fd);
	} else {
		// Outputting as a simple html page ( the extension of the file is not represented in the table )
		((http_header_callback_t) get(headers_callback, "html"))(socket);

		msg = "<!DOCTYPE html><body><pre>";
		msg_sz = strlen(msg);

		ctrl = write(socket,msg, msg_sz);
		if ( ctrl != msg_sz ){
			close(fd);
			free(data);
			return;
		}
		
		ctrl = write(socket, data, sz);
		if ( ctrl != sz ) {
			close(fd);
			free(data);
			return;
		}

		close(fd);
		msg = "</pre></body></html>";
		write(socket,msg, strlen(msg));
	}
	free(data);
}

void * http_settings_callback(void * http_data_ptr) {

	http_data_t * http_data = (http_data_t *) http_data_ptr;
	int socket = (int) *http_data->socket, status;
	hashtable_t * params;
	char buffer[1024], *path, *file_type, *msg, 
		*action, *host, *port; // Will be sufficient, for most cases.. Cookies 
	http_header_callback_t callback;

	params = new_hashtable(30, 0.8);

	if ( params == NULL ) {
		log_error("%s.%d new_hashtable failed\n", __func__, __LINE__);
		output_internal_server_error(socket);
		close(socket);
		return NULL;
	}

	params->data_also = 1;

	read(socket, buffer, sizeof (buffer) - 5 );

	// Just some additional "safety" before parsing
	snprintf(buffer + sizeof(buffer) - 5, 4, "\r\n\r\n");
	buffer[sizeof(buffer) - 1] = '\0';

	parse_http_get_headers_and_arguments(params, buffer, sizeof (buffer) - 1);

	action = get(params, "action");
	host = get(params, "host");
	port = get(params, "port");

	if ( action != NULL ) {

		if ( ! strcmp(action, "set") ) {

			if ( host != NULL && port != NULL ) {

				status = configure_host_and_port(host, port);

				output_txt_headers(socket);
				
				if ( status == 0 ) {
					msg = "Success: Host and port is configured";
					write(socket, msg, strlen(msg));
				} else {
					msg = "Error: Failed to configure host and port";
					write(socket, msg, strlen(msg));	
				}

				close(socket);
				return NULL;
			} else {

				output_txt_headers(socket);	
				msg = "Error: Failed to configure host and port, no host and port arguments given";
				write(socket, msg, strlen(msg));
				close(socket);

			}

		} else if ( !strcmp(action,"get") ) {
			output_json_headers(socket);	
			output_proxy_configuration_as_json(socket);
			close(socket);
		}

		return NULL;
	}

	path = get(params, "PATH");

	if ( path != NULL ){
		output_path(socket, path);
	} else {
		output_file_not_found(socket);
	}

	close(socket);

	return NULL;
}

static int client_socket = 0;
static int remote_socket = 0;

int fd(void)
{
	unsigned int fd = client_socket;
	if (fd < remote_socket)
	{
		fd = remote_socket;
	}
	return fd + 1;
}

void * tunnel_callback(void * http_data_ptr)
{
	http_data_t * http_data = (http_data_t *) http_data_ptr;
	int socket = (int) *http_data->socket, proxy_socket;
	proxy_domain_t * domain = http_data->proxy_session;
	char *client_ip = http_data->client_ip, *time = http_data->accept_time,
		tmp_buffer[128];
	size_t read_sz = 0, buffer_size = BUFFER_INCREMENT;

	fd_set io;
	char * buffer;

	buffer = calloc(BUFFER_INCREMENT,1);

	if ( buffer == NULL ) {
		printf("CALLOC FAIL\n");
		return NULL;
	}

	// look at the first line of 'buffer' and do what you got to do..
	proxy_socket = make_contact(domain->url, domain->port);
	
	if ( proxy_socket < 0 ) {
		printf("Host: %s:%d unreachable. Please reconfigure proxy domain.\n", domain->url, domain->port);
		return NULL;
	}

	client_socket = socket;
	remote_socket = proxy_socket;

	int latest_print = 0;

	for (;;)
	{
		printf("In tunnel for loop..\n");
		FD_ZERO(&io);
		FD_SET(socket, &io);
		FD_SET(proxy_socket, &io);

		memset(buffer, 0, BUFFER_INCREMENT);

		if (select(fd(), &io, NULL, NULL, NULL) < 0)
		{
			perror("use_tunnel: select()");
			break;
		}

		if (FD_ISSET(client_socket, &io))
		{
			if ( latest_print !=  1)
				printf("1\n");
			latest_print = 1;
			int count = recv(client_socket, buffer, sizeof(buffer), 0);
			if (count < 0)
			{
				perror("use_tunnel: recv(rc.client_socket)");
				close(client_socket);
				close(remote_socket);
				free(buffer);
				return NULL;
			}

			if (count == 0)
			{
				close(client_socket);
				close(remote_socket);
				free(buffer);
				return NULL;
			}

			send(remote_socket, buffer, count, 0);

		}

		if (FD_ISSET(remote_socket, &io))
		{
			if ( latest_print !=  2)
				printf("2\n");
			latest_print = 2;
			int count = recv(remote_socket, buffer, sizeof(buffer), 0);
			if (count < 0)
			{
				perror("use_tunnel: recv(rc.remote_socket)");
				close(client_socket);
				close(remote_socket);
				free(buffer);
				return NULL;
			}

			if (count == 0)
			{
				close(client_socket);
				close(remote_socket);
				free(buffer);
				return NULL;
			}

			send(client_socket, buffer, count, 0);
		}
	}
	printf("Bye\n");

	free(buffer);

	return NULL;

}

void * http_proxy_callback(void * http_data_ptr)
{

	http_data_t * http_data = (http_data_t *) http_data_ptr;
	int socket = (int) *http_data->socket, proxy_socket;
	proxy_domain_t * domain = http_data->proxy_session;
	char *client_ip = http_data->client_ip, *time = http_data->accept_time,
		tmp_buffer[128];
	size_t read_sz = 0, buffer_size = BUFFER_INCREMENT;

	char * buffer = calloc(buffer_size, 1), *first_line, *c, *fix;

	if ( buffer == NULL ){
		log_error("%s.%d Failed to allocate %zu bytes.\n", __func__, __LINE__, buffer_size);
		goto end2;
	}

	read_sz = read(socket, buffer, buffer_size);

	if ( read_sz == 0 )
		goto end1;

	while ( read_sz == buffer_size && read_sz < MAXIMUM_READ_SIZE ) {

		buffer_size += BUFFER_INCREMENT;

		buffer = realloc(buffer, buffer_size);

		memset(buffer + read_sz, '\0', BUFFER_INCREMENT);

		if ( buffer == NULL ){
			log_error("%s.%d Failed to allocate %zu bytes.\n", __func__, __LINE__, buffer_size);
			goto end2;
		}

		read_sz += read(socket, buffer + read_sz, BUFFER_INCREMENT);

	}

	c = strchr(buffer, '\r');
	if ( c != NULL ) 
		*c = '\0';

	first_line = buffer;

	log("[%s] %s: %s\n", time, client_ip, first_line);

	if ( c != NULL )
		*c = '\r';

	// look at the first line of 'buffer' and do what you got to do..
	proxy_socket = make_contact(domain->url, domain->port);
	
	if ( proxy_socket < 0 ) {
		printf("Host: %s:%d unreachable. Please recongifure proxy domain.\n", domain->url, domain->port);
		return NULL;
	}

	if ( ( fix = strstr(buffer, "Host: ") ) != NULL ) {
		
		*fix = '\0';
		write(proxy_socket, buffer, strlen(buffer)); // writing the content there

		memset(tmp_buffer, '\0', sizeof tmp_buffer);
		snprintf(tmp_buffer, sizeof tmp_buffer, "Host: %s:%d", domain->url, domain->port);
		write(proxy_socket, tmp_buffer, strlen(tmp_buffer));

		*fix = 'H';
		c = strstr(fix, "\r\n");

		if ( c != NULL ) {
			c += 2;
			write(proxy_socket, c, strlen(c));
		}


	}

	memset(buffer, '\0', buffer_size);

	int read_c;

	do {

		read_c = read(proxy_socket, buffer, buffer_size - 1 );
		//printf("read: %s", buffer);
		write(socket, buffer, read_c);
		memset(buffer, '\0', buffer_size);
	
	} while ( read_c > 0 );



end1:
   	free(buffer);
end2:  
   	close(socket);

   	free_http_data(&http_data);

   	return NULL;

}

