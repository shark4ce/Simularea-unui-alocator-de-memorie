# Simularea-unui-alocator-de-memorie in C
In acest proiect am simulat un alocator simplu de memorie similiar sistemului implicit malloc/calloc/realloc/free
(folosindu-ne de acestea).
--- Implemenetarea am realizato in C--
Arena - bloc de memorie compact pe care il vom administra aplicand unele comenzi descrie mai jos.
--Structura arenei: consideram arena un vector de N oceti (unsigned char).Fiecare octet poate fi accesat prin indexul sau (de la 0 la N-1). Un index este un intreg fara semn pe 32 biti. Uneori va trebui sa consideram 4 octeti succesici din arena ca reprezentand valoarea unui index, astfel, vom considera ca acesta este reprezentat in format little-endian.

Programul primeste la intrare comnezi de alocare, alterare, afisare si eliberare de memorie, dupa care furnizeaza la iesire reazultatul fiecarei comenzi.
Formatul comnezilor:
1. INITIALIZE <N> - aceasta comanda initializeaza o arena de memorie de dimnesiunea N octeti setati pe 0.
2. FINALIZE - aceasta comanda elibereaza memoria alocata la initializare.
3. DUMP - comanda care afiseaza intreaga harta de memorie(arena) asa cum se gaseste la momentul dat, cotec, cu octec. Vor fi afisati cate 16 octeti pe fiecare linie.
4. ALLOC <SIZE> - aceasta comanda aloca SIZE octeti de memorie din arena. Va folosi prima zona libera valida intr-o cautare de a stanga la dreapta.
5.FREE <INDEX> - aceasta comnada va elibera blocul de memorie al carei sectiuni de date incepe la pozitia INDEX in arena.
6.FILL <INDEX> <SIZE> <VALUE> - aceasta comanda va seta SIZE octeti din arena la valorea VALUE (cuprinsa intre 0 si 255) si va modifica octetii blocurilot incepand cu blocul cu indexul INDEX. Se vor seta octeti de date pana cand s-au setat SIZE octeti sau pana cand s-au parcrus toate blocurile de dupa blocul INDEX si s-au setat toti octetii de date ale acestora.
 
 
 -----Detalii despre implementare-----
 
 1.Initial am folosit Structs, pentru a crea o coletie de valori stocate intr-o zona compacte de memorie pe care le putem apela ori de cate ori avem nevoie, in cazul nostru, pentru a crea si gestiona arena creeata, realazand acest lucru mult mai usor.

2. De asemenea, pentru a optimiza codul si al face cat mai clar am creeat pentru fiecare comanda input functii aparte, pe care le vom apela in fuctia data de schelt parse_command, pe care la randul sau o vom apela in main.

3.In functia initialization initializam o arena de dimensiunea size_arena octeti prin intermediul alocarii dinamice cu calloc. Dupa initializare arena nu va contine nici un bloc, va fi ,,goala", astfel am adaugat variabilele start_bloc care indica pozitia primului bloc care urmeaza sa fie inserat si isEmpty care am initializato cu 1(true), adica ca este adevarata, arena este goala.Valoarea pentru size_arena o primim din functia parse_command.

4.Dupa am creat 2 functii getNUM si inNUM pentru a converti numerele (X) primite ca index la ALLOC,FREE,FILL, astfel evitand scrierea acestui algoritm ori de cate ori avem nevoie de a ,,introduce" sau ,,scoate" un numar doar prin apelarea acestei functii.

5.In functia finalize eliberam memoria alocata la initializare prin intermediul functiei free().

6.Functia DUMP afiseaza harta memoriei initializate conform cerintelor indicate in tema,in format hexazecimal, fapt ce l-am impletat cu o instructiunile for si if, cu  printf(”%02X”) si printf(”%08X”).

7.In functia alocare, alocam alloc_size octeti in arena noastra, astfel a fost necesara tratarea fiecarui caz posibil in parte:

   I. Atunci cand arena este goala, mai intai verificam daca putem aloca alloc_size biti in arena folosind instructiune if,astfel daca marimea arenei - 12( acesta indica spatiul ocupat de catre sectiunea de gestiune) este mai mic ca alloc_size ne nu putem aloca alloc_size octeti de memorie din arena, respectiv ne returneaza 0, insa daca acesta conditie e falsa, putem aloca numarul de octeti dorit,asfel folosind functia inNUM alocam primul bloc.Prin urmare isEMPTY va deveni 0 (falsa) iar star_bloc=0, deoarece aceasta indica inceputul primului bloc.

   II.Verificam daca putem adauga un bloc inainte de primul , astfel daca start_bloc e mai mare sau egal ca alloc_size+12(sectiunea de gestiune), putem adauga.Prin urmare, primul bloc devine urmatorul.Dupa introducem datele necesare in sectiunea de gestiune,respectiv daca acesta a devenit primul bloc , in prima parte a sectiunii de gestiune ce ne indica pozitia blocului precedent vom insera 0, deoarece acesta nu exista.

   III.In cel de-al 3 caz cercetam spatiul dintre fiecare 2 blocuri. Introducem variabilele curent si next care vor indica pozitiile blocului curent si respectiv celui urmator si av_size care ne va indica marimea disponibila intre blocuri in urma cercetarii.Astfel av_size va fi egal cu diferenta dintre pozitiile blocului urmator si curent.Daca alloc_size +12 (sectiunea de gestiune) va fi mai mica si egala cu av_size , atunci puem introduce noul bloc. Dupa va trebui sa introducem datele in sectiunea de gestiune cu ajutorul functiei inNUM si sa facem update la valorile din sectiunile de gestiune a blocului curent si cel urmator.In caz ca marimea dintre blocuri av_size e mai mica ca alloc_size, cautam in continuare un spatiu destul de mare pentru a introduce noul bloc dorit,astfel blocul curent devenind urmatorul.

  IV.In caz ca nu reusim sa gasim un spatiu din memoria arenei necesar intre blocuri, incercam sa adaugam blocul la sfirsitul arenei, atunci cand next==0, adica urmatorul bloc nu exista si blocul curent eultimul.Respectiv verificam daca putem introduce un nou bloc dupa ultimul cu ajutorul instructiunii if unde, daca diferenta dintre marimea arenei si pozitia blocului curent + 12 (S.de gestiune) si insasi marimea blocului e mai mare ca marimea noului bloc alloc_size+12, atunci putem introduce noul bloc.Acesta prin urmare devenind ultimul bloc, astfel, urmatorul bloc la radul sa devine nul, adica n exista.

6.Functia Free va elibera blocul de memorie al care sectiuni de date incepe la pozitia index.Pentru inceput creem variabila succ si pred a caror valori vor reprezenta coordonatele blocurilor urmator si precedent. Si aici vom avea mai multe cazuri:
  I. In caz ca stergem primul bloc, urmatorul va deveni primul, astfel start_bloc=succ.Dar daca in arena noastra am avut doar un bloc, pe care dupa l-am sters cu functia Free, succ va deveni automat 0 si arena noastra va fi goala, adica isEMPTY=1.
  II. In caz ca stergem un alt bloc diferit de primul,spre exemplu un bloc(y) dintre 2 blocuri ( blocul x si blocul z), atunci pred lui z va deveni blocul x si succ blocului x va deveni blocul z.
Astfel in urma eliberarii oricarui bloc, vom face update la toate sectiunile de gestionare ale blocurilor respective care necesita schimbari.

7.Functia Fill conform cerintelor va seta cnt octeti din arena la valoarea val si va modifca octetii blocurilor incepand cu indexul index, fapt ce va fi realizat cu functia for. Iar in caz ca numarul de cnt octeti depaseste numarul de octeti a unui bloc, va trece la urmatorul bloc, pas setat prin instructiune if.
 
  
