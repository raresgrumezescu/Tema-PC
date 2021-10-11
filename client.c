#include <stdio.h>     
#include <stdlib.h>     
#include <unistd.h>     
#include <string.h>     
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>      
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"
//macro-uri cu valorile host-ului si
//port-ului date in enuntul temei.
#define HOST "34.118.48.238"
#define PORT 8080
//Aceasta functie primeste ca parametrii ID-ul si cookies
//cookies este nul daca nu exista inca unul, sau va fi
//egal cu cookie-ul user-ului curent daca exista cineva
//logat.
void registe(char *ip, char **cookies) {
	//se deschide conexiunea
	int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
	//variabilele ce urmeaza sa fie folosite
	char username[100], password[20];
	char *payload = NULL, *message, *response;
	memset(username, 0, 100);
	memset(password, 0, 20);
	
	//se citesc de la tastatura username-ul si parola
	//viitorului utilizator.
	printf ("username = ");
	scanf("%s", username);
	
	printf ("password = ");
	scanf("%s", password);
	
	//Se creeaza un obiect JSON care il va reprezenta pe user.
	JSON_Value *root_value = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(root_value);
	json_object_set_string(root_object, "username", username);
	json_object_set_string(root_object, "password", password);
	payload = json_serialize_to_string_pretty(root_value);
	//Se creeaza mesajul ce va fi trimis la server cu ajutorul functiei
	//compute_post_request (Se face POST asa cum se specifica in enunt);
	message = compute_post_request(ip, "/api/v1/tema/auth/register", "application/json", payload, NULL, NULL);
	//Mesajul se trimite la server
	send_to_server (sockfd, message);
	//Se salveaza raspunsul serverului.
	response = receive_from_server(sockfd);
	//Acest mesaj va contine urmatorul string in cazul in care deja exista
	//un user cu numele dat de la tastatura:
	//"error":"The username cont1 is taken!""
	//daca numele este unul valid nu se afiseaza 
	//acest mesaj. Asadar, ma folosesc de el pentru
	//a determina daca user-ul exista deja, iar daca exista
	//afisez mesajul de eroare.
	//Pentru asta, caut string-ul "is taken" in raspuns,
	//subsir constituent in mesajul de eroare dat de server.
	if (strstr(response, "is taken!") != NULL) {
		printf ("Numele de utilizator este deja folosit\n");
		return;
	}
	
	//printf ("%s\n", response);
	//inchid conexiunea si eliberez memoria.
	free(message);
	free(response);
	json_free_serialized_string(payload);
	json_value_free(root_value);
	close_connection(sockfd);
}
//ip si cookie au aceeasi semnificatie cu cele din registe.
void login(char *ip, char **cookies) {
	//deschid conexiunea
	int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
	//variabilele ce vor fi folosite
	char username[100], password[20];
	char *payload = NULL, *message, *response;
	memset(username, 0, 100);
	memset(password, 0, 20);
	//Daca cookie != NULL inseamna ca exista
	//cineva inca conectat
	if (*cookies) {
		printf("Persoana deja logata\n");
		return;
	}
	//Se cistesc username si password
	printf ("username = ");
	scanf("%s", username);
	
	printf ("password = ");
	scanf("%s", password);
	//Se creeaza obiectul JSON
	JSON_Value *root_value = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(root_value);
	json_object_set_string(root_object, "username", username);
	json_object_set_string(root_object, "password", password);
	payload = json_serialize_to_string_pretty(root_value);
	//Se face POST si se creeaza mesajul ce va fi trimis la server. Acest mesaj contine
	//obiectul JSON reprezentand user-ul
	message = compute_post_request(ip, "/api/v1/tema/auth/login", "application/json", payload, NULL, NULL);
	send_to_server (sockfd, message);
	response = receive_from_server(sockfd);
	//logica este identica cu cea de la functia anterioara.
	//"Credentials are not good!" apare in raspuns doar daca
	//credentialele nu se potrivesc
	if (strstr (response, "Credentials are not good!") != NULL) {
		printf ("Informatiile de logare nu sunt valide\n");
		return;
	}
	//daca mesajul a fost trimis cu succes, atunci se scoate
	//cookie-ul din raspuns si se salveaza in variabila cookies
	//data ca parametru.
	if (strstr(response, "Bad Request") == NULL) {
		char *ptr = strstr(response, "Set-Cookie: ");
		char *ptr2;
		ptr2 = ptr + 12;
		char *cookie = strtok(ptr2, "\n");
		*cookies = malloc (1000 * sizeof(char));
		strcpy (*cookies, cookie);
	}
	//se inchide conexiunea si eliberez memoria folosita
	free(message);
	free(response);
	json_free_serialized_string(payload);
	json_value_free(root_value);
	close_connection(sockfd);
}

//logout deogheaza un user conectat
//ip si cookies au aceeasi semnificatie
//ca la functiile anterioare
void logout(char *ip, char **cookies) {
	int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
	char *message, *response;
	//Nu se poate face deconectarea daca nu esti inca autentificat
	if (*cookies == NULL) {
		printf("Nu exista dovezi ca sunteti autentificati\n");
		return;
	}
	//Am vazut ca fara acest "\n", nu se construieste cum
	//trebuie mesajul spre server, si l-am adaugat pentru
	//a-l construi corect.
	strcat(*cookies, "\n");
	//Se face POST si se creeaza mesajul.
	message = compute_get_request(ip, "/api/v1/tema/auth/logout", NULL, *cookies, NULL);
	//cookies devine nul, deoarece utilizatorul s-a deconectat.
	*cookies = NULL;
	//mesajul se trimite la server
	send_to_server (sockfd, message);
	//se salveaza raspunsul in variabila "response"
	response = receive_from_server(sockfd);
	//se inchide conexiunea si se elibereaza memoria
	close_connection(sockfd);
	free(message);
	free(response);
}
//aceasta este functia de care se va folosi comanda
//enter_library
//ip si cookies cu aceeasi semnificatie ca la celelalte
//functii
char* enter_library (char *ip, char **cookies) {
	//se deschide conexiunea
	int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
	//se vor salva mesajul catre server si raspunsul serverului
	char *message, *response;
	//Daca cookies este nul, atunci nu ai acces al biblioteca
	if (*cookies == NULL) {
		printf ("Nu ati demonstrat ca sunteti autentificat\n");
		return NULL;
	}
	//aceeasi ratiune ca mai devreme
	strcat(*cookies, "\n");
	//se face GET si se creeaza mesajul la server
	message = compute_get_request(ip, "/api/v1/tema/library/access", NULL, *cookies, NULL);
	//se trimite mesajul
	send_to_server (sockfd, message);
	//se salveaza raspunsul
	response = receive_from_server(sockfd);
	//se cauta in raspuns token-ul si se izoleaza
	char *ptr = strstr(response, "{");
	char *ptr2;
	ptr2 = ptr + 10;
	char *jwt = strtok(ptr2, "}");
	char *dest = malloc (strlen(jwt) * sizeof(char));
	strncpy(dest, jwt, strlen(jwt) - 1);
	
	//se inchide conexiunea si se elibereaza memoria
	close_connection(sockfd);
	free(message);
	free(response);
	//se returneaza token-ul cerut, salvat in variabila "dest";
	return dest;
}
//functie ce afiseaza lista de carti
//ip si cookies cu aceeasi semnificatie ca si 
//la celelalte functii, jwt este token-ul JWT
char* get_books (char *ip, char **cookies, char *jwt) {
	//se deschide conexiunea
	int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
	//variabile folosite in continuare
	char *message, *response;
	//daca jwt este nul, atunci inseamna ca nu ai acces la biblioteca
	if (jwt == NULL) {
		printf ("Nu aveti acces la biblioteca\n");
		return NULL;
	}
	//Am explicat mai devreme de ce fac asta
	strcat(*cookies, "\n");
	//Se face GET si se salveaza mesajul ce va fi trimis la server
	message = compute_get_request(ip, "/api/v1/tema/library/books", NULL, *cookies, jwt);
	//se trimite mesajul la server
	send_to_server (sockfd, message);
	//se salveaza raspunsul serverului.
	response = receive_from_server(sockfd);
	//se cauta lista de carti in raspuns, deoarece este continuta de acesta
	//Se izoleazadupa acelasi mecanism ca atunci cand se izoleaza token-ul
	char *ptr = strstr(response, "[");
	char *ptr2;
	ptr2 = ptr + 1;
	char *lista = strtok(ptr2, "]");
	free(message);
	free(response);
	//inchid conexiunea si se returneaza lista.
	//Aceasta va fi afisata in main si folosita de get_book
	//si delete_book
	close_connection(sockfd);
	return lista;
}
//fuctia cauta o carete dupa id-ul citit si stabileste
//daca exista sau nu cartea. Daca exista, o afiseaza
//ip, cookies si jwt au aceeasi semnificatie
//ca mai sus
void get_book(char *ip, char **cookies, char *jwt) {
	//se deschide conexiunea
	int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
	char id[20];
	//in address se tine minte ruta de acees, a cre sa va concatena ulterior id-ul
	char address[200] = "/api/v1/tema/library/books/", *message, *response;
	memset(id, 0, 20);
	//daca jwt este nul, atunci nu poti demonstra ca ai acees la biblioteca
	if (jwt == NULL) {
		printf ("Nu aveti acces la biblioteca\n");
		return;
	}
	
	printf ("id = ");
	scanf("%s", id);
	//se apeleaza get_books si se salveaza lista de carti intr-o variabila
	//numita "lista".
	char *lista = get_books(ip, cookies, jwt);
	//daca lista este nula (fac acest test pentru a evita Segmentation
	//Fault)
	if (lista == NULL) {
		printf("Lista de carti este goala\n");
		return;
	}
	
	//Daca id nu se gaseste in lista
	if (strstr(lista, id) == NULL) {
		printf ("ID-ul introdus este invalid\n");
		return;
	}
	//se concateneaza la ruta id-ul.
	//daca programul a ajuns pana in acest punct, inseamna
	//ca id-ul este valid.
	strcat(address, id);
	//se face GET si se creeaza mesajul
	message = compute_get_request(ip, address, NULL, *cookies, jwt);
	//se trimite la server
	send_to_server (sockfd, message);
	//raspunsul serverului
	response = receive_from_server(sockfd);
	//se cauta si se selecteaza cartea
	char *ptr = strstr(response, "[");
	ptr = ptr + 1;
	char *ptr2 = strtok(ptr, "]");
	//se afiseaza cartea
	printf ("%s\n", ptr2);
	//se elibereaza memoria si se inchide conexiunea.
	free(message);
	free(response);
	close_connection(sockfd);
}

//functie ce adauga o carte.
//id, cookies si jwt cu aceeasi semnificatie ca la functiile
//anterioare.
void add_book(char *ip, char **cookies, char *jwt) {
	//se deschide conexiune
	int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
	//string-uri in care se vor salva informatiile 
	//despre carte.
	char title[50], author[50], genre[30], publisher[50];
	int page_count = 0;
	char *payload = NULL, *message, *response;
	
	memset(title, 0, 50);
	memset(author, 0, 50);
	memset(genre, 0, 30);
	memset(publisher, 0, 50);
	//daca jwt este nul, nu ai demonstrat ca ai acces la biblioteca
	if (jwt == NULL) {
		printf ("Nu aveti acces la biblioteca\n");
		return;
	}
	//se citesc informatiile despre carte
	printf ("title = ");
	scanf ("%s", title);
	
	printf ("author = ");
	scanf ("%s", author);
	
	printf ("genre = ");
	scanf ("%s", genre);
	
	printf ("page_count = ");
	scanf ("%d", &page_count);
	
	printf ("publisher = ");
	scanf ("%s", publisher);
	//daca s-a sarit peste cel putin o informatie (adica daca a ramas una neinitializata corespunzator)
	//se afiseaza un mesaj de eroare
	if (strcmp (title, "\n") == 0 || strcmp (author, "\n") == 0 || strcmp (genre, "\n") == 0 || strcmp (publisher, "\n") == 0 || page_count == 0) {
		printf ("Informatiile despre carte sunt incomplete\n");
		return;
	}
	//se creeaza obiectul carte pe baza informatiilor citite.
	//Daca proramul a ajuns in acest punct, inseamna ca informatiile sunt valide.
	JSON_Value *root_value = json_value_init_object();
	JSON_Object *root_object = json_value_get_object(root_value);
	json_object_set_string(root_object, "title", title);
	json_object_set_string(root_object, "author", author);
	json_object_set_string(root_object, "genre", genre);
	json_object_set_number(root_object, "page_count", page_count);
	json_object_set_string(root_object, "publisher", publisher);
	payload = json_serialize_to_string_pretty(root_value);
	
	//daca pun cookie != NULL, imi da Bad Request.
	//Obtin mesajul dupa ce fac POST
	message = compute_post_request(ip, "/api/v1/tema/library/books", "application/json", payload, NULL, jwt);
	//se trimite mesajul la server
	send_to_server (sockfd, message);
	//raspunsul server-ului
	response = receive_from_server(sockfd);
	
	
	free(message);
	free(response);
	//inchid conexiunea
	close_connection(sockfd);
}
//functie ce sterge o carte din lista.
//ip, cookies si jwt au aceeasi semnificatie ca mai sus.
void delete_book (char *ip, char **cookies, char *jwt) {
	//se deschide conexiunea
	int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
	char id[20];
	//se salveaza in address ruta la care se va concatena id-ul daca acesta
	//este valid, pentru a se putea face stergerea ceruta
	char address[200] = "/api/v1/tema/library/books/", *message, *response;
	memset(id, 0, 20);
	//Trebuie sa demos=nstrezi ca ai acees la biblioteca
	if (jwt == NULL) {
		printf ("Nu aveti acces la biblioteca\n");
		return;
	}
	//se citeste id-ul
	printf ("id = ");
	scanf("%s", id);
	//se salveaza lista de carti, in care se va cauta id-ul
	char *lista = get_books(ip, cookies, jwt);
	//Daca lista este nula nu are sens cautarea
	if (lista == NULL) {
		printf("Lista de carti este goala\n");
		return;
	}
	//daca id-ul nu exista, inseamna ca nu exista carte cu id-ul acela
	if (strstr(lista, id) == NULL) {
		printf ("ID-ul introdus este invalid\n");
		return;
	}
	//se adauga in ruta id-ul
	strcat(address, id);
	//se face DELETE si se formeaza mesajul catre server
	message = compute_delete_request(ip, address, *cookies, jwt);
	//se trimite mesajul la server
	send_to_server (sockfd, message);
	//raspunsul serverului.
	response = receive_from_server(sockfd);
	//se inchide conexiunea si se elibereaza memoria folosita pentru
	//a retine mesajul si raspunsul.
	free(message);
	free(response);
	close_connection(sockfd);
}

int main(int argc, char *argv[]) {
	
	char buffer[100];
	//se va pastra un cookie al utilizatorului.
	char *cookies = NULL;
	//se va retine token-ul JWT cand va fi necesar
	char *jwt;

	//While-ul este responsabil pentru citirrea comnzilor de la tastatura.
	//Aceste comenzi se retin in variabila buffer, iar de fiecare data cand
	//o comanda este scrisa, continutul din buffer este comparat cu posibilele
	//comenzi pe care le-ar dori utilizatorul.

	//Pentru fiecare comanda, folosesc cate o functie
	while (1) {
		memset(buffer, 0, 100);
		scanf("%s", buffer);
		//daca continutul buffer-ului se potirveste cu unul
		//din urmatoarele string-uri (adica nume de comenzi),
		//atunci se apeleaza functia corespunzatpare
		//fiecareia din comenzi.
		if (strcmp (buffer, "register") == 0) {
			registe(HOST, &cookies);
		}
		
		if (strcmp (buffer, "login") == 0) {
			login(HOST, &cookies);
		}
		
		if (strcmp (buffer, "enter_library") == 0) {
			jwt = enter_library(HOST, &cookies);
		}
		
		if (strcmp (buffer, "logout") == 0) {
			logout (HOST, &cookies);
		}
		
		if (strcmp (buffer, "get_books") == 0) {
			char *lista = get_books (HOST, &cookies, jwt);
			if (lista != NULL) printf ("%s\n", lista);
		}
		
		if (strcmp (buffer, "get_book") == 0) {
			get_book (HOST, &cookies, jwt);
		}
		
		if (strcmp (buffer, "add_book") == 0) {
			add_book (HOST, &cookies, jwt);
		}
		
		if (strcmp (buffer, "delete_book") == 0) {
			delete_book (HOST, &cookies, jwt);
		}
		//"exit" e singura comanda care nu are functie
		//de care sa se foloseasca. Cand se citeste "exit",
		//se iese cu un break din while si se termina programul.		
		if (strcmp (buffer, "exit") == 0) {
			break;
		}
	}	
	
	return 0;
}
