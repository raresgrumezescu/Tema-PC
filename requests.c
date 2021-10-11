#include <stdlib.h>     
#include <stdio.h>
#include <unistd.h>    
#include <string.h>     
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>      
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

//Functiile acestea sunt gandite dupa modelul celor
//din laboartorul 10.

//compute_get_request creeaza mesajul care va fi trimis la server
char *compute_get_request(char *host, char *url, char *query_params,
                            char *cookies, char *jwt)
{
    	char *message = calloc(BUFLEN, sizeof(char));
    	char *line = calloc(LINELEN, sizeof(char));
	//se pune in mesaj metoda, parametrii si tipul protocolului
    	if (query_params != NULL) {
        	sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    	} else {
    	    	sprintf(line, "GET %s HTTP/1.1", url);
    	}

    	compute_message(message, line);
	//Se pune host-ul
    	sprintf(line, "Host: %s", host);
    	compute_message(message, line);
	//Se pun headerele si cookie-urile daca exista
	if (jwt != NULL) {
        	sprintf(line, "Authorization: Bearer %s", jwt);
        	compute_message(message, line);
    	}

	if (cookies != NULL) {
    		sprintf(line, "Cookie: %s", cookies);
    		compute_message(message, line);
    	}
	//Se pune new line
    	compute_message(message, "");
    	//se returneaza mesajul
    	return message;
}
//compute_post_request creeaza mesajul care va fi trimis la server
char *compute_post_request(char *host, char *url, char* content_type, char *body_data,
                            char *cookies, char *jwt)
{
	
	char *message = calloc(BUFLEN, sizeof(char));
    	char *line = calloc(LINELEN, sizeof(char));
    	char *body_data_buffer = calloc(LINELEN, sizeof(char));
	//se pune in mesaj metoda, parametrii si tipul protocolului
    	sprintf(line, "POST %s HTTP/1.1", url);
    	compute_message(message, line);
 	//Se pune host-ul
    	sprintf(line, "Host: %s", host);
    	compute_message(message, line);
	//se pun headerele, dintre care inclusiv
	//Content-Type si Content-Length.
    	sprintf(line, "Content-Type: %s", content_type);
    	compute_message(message, line);

    	sprintf(line, "Content-Length: %ld", strlen(body_data));
    	compute_message(message, line);
    	
    	if (jwt != NULL) {
        	sprintf(line, "Authorization: Bearer %s", jwt);
        	compute_message(message, line);
    	}
    	//Se pun cookie-urile daca exista
    	if (cookies != NULL) {
    		sprintf(line, "Cookie: %s", cookies);
    		//strcat(line, cookies);
    		compute_message(message, line);
    	}
    	//se pune new-line si apoi continutul
    	compute_message(message, "");
    	memset(line, 0, LINELEN);
    	strcat(body_data_buffer, body_data);
    	compute_message(message, body_data_buffer);

    	free(line);
    	//se returneaza mesajul
    	return message;
    
}

//compute_delete_request creeaza mesajul care va fi trimis la server
char *compute_delete_request(char *host, char *url, char *cookie, char* jwt)
{
	char *message = calloc(BUFLEN, sizeof(char));
	char *line = calloc(LINELEN, sizeof(char));
	//se pune numele metodei, url-ul si numele metodei
	sprintf(line, "DELETE %s HTTP/1.1", url);
	compute_message(message, line);
	//pune host-ul
	sprintf(line, "Host: %s", host);
	compute_message(message, line);
	//se pun head-erele si cookie-urile
	if (jwt != NULL) {
		sprintf(line, "Authorization: Bearer %s", jwt);
		compute_message(message, line);
	}
	
	if (cookie != NULL) {
		sprintf(line, "Cookie: %s", cookie);
		compute_message(message, line);
	}
	//se pune new-line
	compute_message(message, "");
	free(line);
	//se returneaza mesajul
	return message;
}



