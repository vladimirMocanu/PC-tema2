Mocanu-Prejma Vladimir-Georgian
325CB
					Tema2 PC


	Pentru rezolvarea temei am folosit scheletul/codul din laboratorul 8
si putin din laboratorul 6 pentru implementare UDP.

subscriber.c: 
	
	Pentru inceput initializam tot ce avem nevoie pentru a creea legatura dintre
server si client TCP. Am folosit functiile socket,inet_aton, connect, send,recv,
select. Aici verificam corectitudinea comenzilor primite de la tastatura si cand
trimitem la server comenziile stim ca sunt corecte. Prima data trimitem id_client
catre server, dupa asteptam comenzi de la tastatura, subscribe, unsubscribe sau exit
. Dupa asteptam sa primim raspuns de la server si afisam corespunzator. 
	Optional, am lasat si printf-uri prin care sa vedem daca comanda este greista
sau nu.

\5+

server.c
	
	La fel ca la subscriber.c initializam tot ce avem nevoie. Aici o sa avem 
o conextiune TCP si una UDP. Am folosit o strucutra de data definita de mine pentru 
stocarea datelor. Am mers pe varianta cu vector, dar cred ca o implementare mai
buna si mai eficienta era cu liste. Folosesc un vector de topic, care fiecare topic
contine un vector de id_client. Programul ruleaza pana il oprim noi. Am verificat
sa vedem pe ce conexiune vine mesajul, adica pe TCP sau UDP si actionam in cauza.
Avem siguranta ca comenziile sunt corecte, si doar impartim string-ul dupa " "(pauza)
Daca comanda este subscribe, bagam la topic-ul respectiv id_client, daca este unsubscribe
il scoatem de acolo. 



Probleme + imbunatatiri:

	Principala problema este modul de abordare. Am folosit vector si pe parcurs
mi-am dat seama ca este mult eficient si usor cu liste. Doar ca nu am mai avut timp
sa schimb.

	Pentru conexiune UDP, mesajele nu se primesc cum trebuie. Am incercat mai
multe variante, nu imi dau seama unde am gresit sau cum ar trebui sa implementez.
Primesc doar topic-ul, adica pana la \0 ,nu primesc si restul mesajului. In rest
este implementat tot pentru clientul UDP.

	Nu am implementat S&F. Ca idee, dupa ce s a dezabonat un client care are S&F 
setat pe 1, scriu totul intr-un fisier si cand se aboneaza la loc, trimit si ce
este scris in fisier.
	