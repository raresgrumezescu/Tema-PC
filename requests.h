#ifndef _REQUESTS_
#define _REQUESTS_

char *compute_get_request(char *host, char *url, char *query_params,
							char *cookies, char *jwt);


char *compute_post_request(char *host, char *url, char* content_type, char *body_data,
							char* cookies, char *jwt);
							
char *compute_delete_request(char *host, char *url, char *cookie, char* jwt);

#endif
