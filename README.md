# Tema-PC
Implementarea unui client care interactioneaza cu un REST API.

Clientul creat functioneaza cu ajutorul urmatarelor fisiere:
buffer.c, buffer.h, client.c, helpers.c, helpers.h, Makefile,
parson.c si parson.h, requests.c si requests.h.

La elaborarea acestei teme m-am folosit de laboratorul 10 de pcom
si de fisierele parson.c si parson.h, pe crae le-am descarcat cu
link0ul dat in enuntul temei: "https://github.com/kgabis/parson".

Fisierele: buffer.c, buffer.h le-am preluat din laboratorul 10.
Acestea contin functii utile de care se folosesc celelalte
fisiere.

In fisierul requests.c sunt scrise functiile:

-compute_get_request
-compute_post_request
-compute_delete_request

In fisierul requests.h sunt antetele lor, de crae m-am folosit
pentru a alcatui acest header.

in fisierul client.c, am scris clientul. Acest fisier este alcatuit
dintr-un main, care contine un while in care se citesc de la tastatura comenzi
si in cate un if se verifica daca comanda citita corespunde vreuneia
din comenzile cerute. Cand se citeste comanda "exit", if-ul crespunzator
acesteia contine un break ce face ca programul sa iasa din loop-ul while-ului
si sa se termine rularea. Acest fisier contine cate o functie pentru fiecare din
celelalte comenzi. Cu toate ca aceste functii sun diferite una de alta, acestea
respecta totusi acelasi sablon. Sablonul este urmatorul:

-se deschide conexiune

-se verifica daca se respecta toate constrangerile ca comanda sa poata
fi data iar rezultatul acesteia sa fie cel corespunzator.

-se afiseaza mesaje de eroare daca vreuna din conditiile ca comanda
se se execute nu este indeplinita.

-se prelucreaza informatiile citite de la tastatura; cand este
nevoie, se creeaza cate un obiect JSON corespunzator.

-se creeaza un mesaj ce este trimis la server

-se primeste un raspuns de la server
se prelucreaza sau analizeaza acest raspuns

-se inchide conexiunea

Am ales sa inchid si sa deschid de fiecare data conexiunea, creand un socket,
deoarece numai astfel programul meu avea continuitate, iar utilizatorul
lui putea sa dea un numar variat de comenzi. Initial dechideam in main
socket-ul si il dadeam ca parametru functiilor, insa acesta se inchidea dupa prima
comanda si devenea cu neputinta de folosit de functiile ce urmau sa fie apelate.

registe: se ocupa cu crearea unui user pe baza username-ului si parolei citit

login: se ocupa cu logarea unui user deja existent. Daca acesta inca e logat,
nu se mai poate face alta logare.

logout: deconecteaza user-ul logat; astfel se permite logarea unuia nou

enter_library: intoarce un token JWT, de are user-ul se va folosi ulterior,
acelui user logat care da comanda cu acelasi nume.

get_books: returneaza lista de carti existenta, ce poate fi nula sau
nenula. Rezultatul acestei functii este folosit de get_book si delete_book,
in momentul in care acestea cauta id-ul citit sa vada daca corespunde unei 
carti. Tot rezultatul ei este afisat in main.

get_book: se cauta o carte dupa id si se afiseaza informatiile despre aceasta

add_book: adauga o carte, dupa ce informatiile acesteia se citesc de la tastatura
si se creeaza un obiect JSON cu ele. Acest obiect este pus in lista de carti
ca atunci cand este nevoie, sa poata fi accesat de aici.

delete_book: sterge o cate al carei id este citit de la tastatura, din lista de carti


De asemenea, am facut si un Makefile dupa modelul celui facut la laborator, care
ruleaza fisierele implicate in realizarea temei; aesta totodata are si o regula de
"run" si una de "clean".

Pentru testare, am folosit exemplele din enunt, si diferite situatii
posibile cu care s-ar fi confruntat cineva cer foloseste aceasta aplicatie.
