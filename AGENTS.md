# AGENTS.md — Reguli generale pentru agenți

## 1. SCOP

Acest fișier conține regulile permanente de lucru pentru agenții AI care modifică acest proiect.

Regulile se aplică tuturor sarcinilor executate în cadrul proiectului, indiferent de:

- limbaj;
- platformă hardware;
- sistem de build;
- modul de lucru;
- componenta modificată;
- agentul AI utilizat.

`AGENTS.md` definește modul în care agentul trebuie să lucreze.

El NU definește o sarcină concretă.

Sarcina concretă este furnizată separat, de regulă prin:

- `agent_prompt.md`;
- `todo.md`;
- instrucțiunea explicită a utilizatorului;
- issue/task asociat.

## 2. PRINCIPIUL GENERAL

Agentul trebuie să execute exact modificarea solicitată și nimic mai mult.

Prioritățile sunt:

1. corectitudinea;
2. păstrarea funcționalității existente;
3. modificări minime;
4. respectarea arhitecturii existente;
5. verificarea modificărilor;
6. claritatea și trasabilitatea schimbărilor.

Nu este permisă refactorizarea generală doar pentru că agentul consideră codul existent mai puțin elegant.

Nu este permisă schimbarea arhitecturii fără un motiv tehnic real și fără aprobarea necesară.

Orice modificare care nu este necesară pentru realizarea sarcinii trebuie evitată.

## 3. ORDINEA DOCUMENTELOR ȘI CERINȚELOR

Înainte de a modifica proiectul, agentul trebuie să identifice și să consulte documentele relevante pentru sarcina curentă.

Acestea pot include:

- `AGENTS.md`;
- `README.md`;
- documentația tehnică;
- documentația hardware;
- specificațiile interfețelor;
- diagrame;
- fișiere de configurare;
- `todo.md`;
- teste;
- documentația API;
- alte documente declarate relevante pentru proiect.

Dacă un document este declarat explicit drept „source of truth”, acesta are prioritate pentru informațiile pe care le definește.

Agentul trebuie să respecte cerințele documentate și nu trebuie să le înlocuiască cu propria interpretare fără un motiv tehnic justificat.

Dacă există contradicții între documente, agentul trebuie să le identifice și să le raporteze înainte de a lua o decizie care poate afecta arhitectura sau funcționalitatea proiectului.

## 4. INSPECȚIA ÎNAINTEA MODIFICĂRII

Înainte de a modifica orice fișier, agentul trebuie să înțeleagă suficient contextul tehnic al sarcinii.

Agentul trebuie să:

1. înțeleagă cerința;
2. identifice fișierele relevante;
3. citească implementarea existentă;
4. identifice dependențele relevante;
5. verifice documentația asociată;
6. verifice dacă funcționalitatea solicitată există deja, integral sau parțial;
7. identifice eventualele componente care pot fi afectate de modificare.

Agentul nu trebuie să înceapă modificarea imediat după identificarea unui singur fișier aparent relevant.

Înainte de implementare trebuie să verifice suficient contextul pentru a evita:

- duplicarea funcționalității existente;
- încălcarea arhitecturii proiectului;
- modificarea inutilă a altor componente;
- introducerea unor incompatibilități;
- pierderea funcționalității existente.

Dacă informațiile disponibile nu sunt suficiente pentru o implementare sigură, agentul trebuie să investigheze mai întâi și să nu ghicească.

## 5. NU GHICI CERINȚELE

Agentul nu trebuie să inventeze informații care lipsesc din cerință sau din proiect.

Dacă o informație este necesară pentru implementare, agentul trebuie mai întâi să o caute în:

1. codul existent;
2. documentația proiectului;
3. testele existente;
4. fișierele de configurare;
5. sursele externe indicate explicit de utilizator.

Agentul nu trebuie să presupună:

- comportamente;
- conexiuni hardware;
- valori;
- pinuri;
- protocoale;
- interfețe;
- structuri de date;
- cerințe de utilizare;
- decizii de arhitectură.

Dacă informația nu poate fi determinată în mod sigur, agentul trebuie să oprească implementarea părții dependente de acea informație și să raporteze situația.

Dacă este necesară o decizie din partea utilizatorului, aceasta trebuie formulată clar, împreună cu opțiunile tehnice relevante.

Principiul este:

**Nu ghici. Verifică mai întâi. Dacă nu poate fi verificat, întreabă.**

## 6. PĂSTRAREA FUNCȚIONALITĂȚII EXISTENTE

Orice modificare trebuie să păstreze funcționalitatea existentă care nu face parte din sarcina curentă.

Agentul nu trebuie să modifice fără motiv:

- funcții existente;
- interfețe publice;
- protocoale;
- structuri de date;
- configurații;
- comportamente validate;
- teste existente;
- componente independente de sarcina curentă.

Modificarea unei componente existente este permisă atunci când este necesară pentru implementarea cerinței.

În acest caz, agentul trebuie să păstreze comportamentul existent în toate situațiile care nu sunt afectate de noua cerință.

Dacă modificarea necesară poate afecta alte funcționalități, agentul trebuie să identifice aceste efecte și să le verifice.

Nu este permisă eliminarea sau schimbarea unei funcționalități existente doar pentru simplificarea implementării.

Principiul este:

**Adaugă sau modifică ceea ce este necesar, fără să strici ceea ce funcționează deja.**

## 7. MODIFICĂRI MINIME

Agentul trebuie să aleagă întotdeauna soluția care produce cea mai mică modificare necesară pentru îndeplinirea cerinței.

Se preferă:

- modificarea punctuală a codului existent;
- reutilizarea funcțiilor existente;
- reutilizarea interfețelor existente;
- reutilizarea bibliotecilor deja folosite;
- păstrarea structurii existente a proiectului.

Trebuie evitate:

- rescrierea completă a unor componente funcționale;
- refactorizarea masivă;
- mutarea fișierelor fără necesitate;
- introducerea unor abstracții inutile;
- schimbarea bibliotecilor fără motiv;
- duplicarea funcționalității existente;
- modificarea unor componente care nu sunt necesare pentru sarcina curentă.

Dacă există deja o implementare funcțională pentru o parte a cerinței, aceasta trebuie reutilizată sau extinsă, nu înlocuită fără un motiv tehnic justificat.

O soluție mai complexă nu este considerată mai bună doar pentru că este mai generală sau mai elegantă.

Principiul este:

**Modifică minimul necesar pentru a obține rezultatul cerut.**


## 8. ARHITECTURA PROIECTULUI

Agentul trebuie să respecte arhitectura existentă a proiectului.

Înainte de a introduce sau modifica o componentă, agentul trebuie să identifice nivelul arhitectural căruia îi aparține și să păstreze separarea existentă între componente.

Dacă proiectul utilizează niveluri precum:

- application;
- logic;
- UI;
- servicii;
- drivere;
- HAL;
- hardware;

modificarea trebuie făcută în nivelul corespunzător.

Agentul nu trebuie să ocolească abstracțiile existente doar pentru a simplifica implementarea.

De exemplu, dacă accesul la hardware este realizat printr-un HAL, codul de nivel superior trebuie să utilizeze HAL-ul existent și nu să acceseze direct hardware-ul.

Nu este permisă introducerea unei noi arhitecturi sau reorganizarea uneia existente fără un motiv tehnic justificat și fără aprobarea necesară atunci când schimbarea este semnificativă.

Principiul este:

**Respectă arhitectura existentă înainte de a introduce una nouă.**

## 9. HARDWARE

Pentru proiectele care includ componente hardware, agentul trebuie să trateze informațiile hardware ca informații tehnice care trebuie verificate, nu presupuse.

Agentul nu trebuie să inventeze sau să presupună:

- GPIO-uri;
- conexiuni;
- pin mapping;
- magistrale;
- interfețe;
- polarități;
- niveluri logice;
- tensiuni;
- adrese hardware;
- configurații ale perifericelor.

Înainte de modificarea codului hardware-dependent, agentul trebuie să verifice informația în sursele disponibile și relevante ale proiectului.

Dacă există un document declarat „source of truth” pentru hardware, acesta trebuie respectat.

Dacă implementarea existentă contrazice documentația hardware, agentul nu trebuie să corecteze automat una dintre ele.

Agentul trebuie să:

1. identifice discrepanța;
2. verifice informațiile disponibile;
3. determine dacă există o explicație clară;
4. raporteze problema dacă aceasta nu poate fi stabilită cu certitudine.

Nu se modifică hardware mapping-ul doar pentru ca implementarea să compileze sau să corespundă unei presupuneri.

Verificarea software a unei configurații hardware nu trebuie prezentată ca verificare fizică a hardware-ului.

Principiul este:

**În hardware, ceea ce nu este verificat nu trebuie presupus.**

## 10. PLATFORME MULTIPLE

Dacă proiectul suportă mai multe platforme, targeturi sau configurații hardware, agentul trebuie să trateze fiecare target separat.

Înainte de modificare, agentul trebuie să identifice:

- targetul afectat;
- codul comun;
- codul specific platformei;
- diferențele relevante dintre platforme;
- eventualele dependențe specifice targetului.

O modificare destinată unui singur target nu trebuie propagată automat către celelalte targeturi.

Dacă există cod comun utilizat de mai multe platforme, agentul trebuie să verifice impactul modificării asupra tuturor utilizatorilor acelui cod.

Dacă aceeași funcționalitate trebuie implementată pe mai multe platforme, agentul trebuie să păstreze diferențele specifice fiecărei platforme și să evite duplicarea inutilă.

Nu este permisă modificarea unui target care nu este afectat de sarcina curentă doar pentru uniformizare.

Principiul este:

**Modifică targetul necesar și verifică impactul asupra codului comun înainte de a afecta alte platforme.**

## 11. BIBLIOTECI ȘI DEPENDENȚE

Agentul trebuie să reutilizeze bibliotecile și dependențele deja existente în proiect atunci când acestea oferă funcționalitatea necesară.

Nu trebuie introdusă o bibliotecă nouă dacă cerința poate fi îndeplinită în mod corespunzător folosind infrastructura existentă.

Dacă este necesară introducerea unei biblioteci sau dependențe noi, agentul trebuie să verifice:

1. dacă există deja o alternativă în proiect;
2. compatibilitatea cu platforma și configurația existentă;
3. impactul asupra build-ului;
4. impactul asupra celorlalte componente;
5. dacă introducerea ei este justificată de cerința curentă.

Agentul nu trebuie să schimbe o bibliotecă existentă doar din preferință personală sau pentru că există o alternativă considerată mai elegantă.

Nu trebuie eliminate dependențe existente fără verificarea prealabilă a utilizării lor în proiect.

Orice schimbare de bibliotecă trebuie să fie limitată la ceea ce este necesar pentru sarcina curentă.

Principiul este:

**Folosește ceea ce există deja; adaugă sau schimbă dependențe numai când este necesar.**

## 12. CONFIGURAȚIE ȘI BUILD

Agentul trebuie să respecte sistemul de build și configurația existentă a proiectului.

Nu trebuie modificate fără necesitate:

- toolchain-ul;
- platforma de build;
- environment-urile;
- opțiunile de compilare;
- configurațiile platformei;
- scripturile de build;
- configurațiile de dependențe.

O modificare a configurației este permisă atunci când este necesară pentru realizarea sau validarea sarcinii curente.

Agentul nu trebuie să modifice configurația doar pentru a evita, ascunde sau ocoli o eroare de compilare.

Dacă o eroare de build este preexistentă și nu are legătură cu modificarea curentă, aceasta trebuie identificată și raportată, nu „reparată” prin modificări fără legătură cu sarcina.

Orice modificare a configurației trebuie verificată printr-un build relevant atunci când acest lucru este posibil.

Principiul este:

**Nu modifica sistemul de build decât dacă sarcina o cere sau modificarea este necesară pentru implementarea corectă.**

## 13. TESTE

Agentul trebuie să identifice testele existente relevante înainte de modificarea codului.

După implementare, agentul trebuie să execute testele relevante pentru modificarea efectuată, atunci când acestea sunt disponibile.

Testarea trebuie să urmărească atât funcționalitatea nouă sau modificată, cât și păstrarea funcționalității existente afectate de schimbare.

Agentul nu trebuie să:

- elimine teste pentru a obține un rezultat pozitiv;
- modifice testele doar pentru a ascunde o eroare;
- considere compilarea drept test funcțional;
- considere un build reușit drept validare hardware.

Pentru proiectele hardware trebuie făcută distincția clară între:

BUILD VERIFIED

și:

HARDWARE VERIFIED

Un build reușit confirmă doar faptul că proiectul a fost compilat cu succes pentru configurația verificată.

HARDWARE VERIFIED poate fi declarat numai dacă funcționalitatea a fost testată efectiv pe hardware.

Dacă nu există teste automate pentru funcționalitatea modificată, agentul trebuie să precizeze acest lucru și, dacă este posibil, să efectueze verificări alternative relevante.

Rezultatele testelor trebuie raportate clar la finalul sarcinii.

Principiul este:

**Compilarea verifică build-ul. Testarea verifică funcționalitatea. Testarea fizică verifică hardware-ul.**

## 14. BUILD

Agentul nu execută build-ul proiectului.

După finalizarea unei modificări care necesită verificare prin build, agentul trebuie să solicite utilizatorului executarea build-ului pentru targetul sau configurația relevantă.

Agentul trebuie să precizeze clar:

- ce target/environment trebuie compilat;
- ce comandă trebuie executată, dacă este relevantă;
- ce rezultat trebuie verificat;
- dacă există erori cunoscute care trebuie urmărite.

Dacă utilizatorul furnizează rezultatul build-ului, agentul trebuie să îl analizeze și să determine dacă modificarea poate fi considerată validată din punct de vedere al compilării.

Dacă build-ul eșuează, agentul trebuie să:

1. analizeze eroarea furnizată;
2. determine dacă eroarea este cauzată de modificarea curentă;
3. propună sau efectueze corecția necesară, în funcție de natura sarcinii;
4. solicite din nou executarea build-ului după corectare.

Agentul nu trebuie să considere build-ul validat dacă utilizatorul nu a furnizat rezultatul executării acestuia.

Un build reușit confirmă doar compilarea pentru configurația verificată. Nu reprezintă automat validare funcțională sau validare hardware.

Principiul este:

**Agentul pregătește și solicită build-ul. Utilizatorul îl execută și furnizează rezultatul.**

## 15. DOCUMENTAȚIE PROTEJATĂ

Documentele declarate ca protejate, stabile sau autoritare nu trebuie modificate automat de agent.

Pot intra în această categorie:

- specificații hardware;
- documente de arhitectură;
- protocoale;
- diagrame;
- cerințe UI;
- documentația interfețelor;
- fișiere declarate explicit „source of truth”;
- alte documente marcate ca stabile sau protejate.

Agentul trebuie să utilizeze aceste documente ca referință, dar nu trebuie să le modifice doar pentru a face implementarea să corespundă.

Dacă implementarea necesită modificarea unui document protejat:

1. agentul trebuie să identifice discrepanța;
2. să explice motivul pentru care modificarea este necesară;
3. să propună modificarea;
4. să solicite aprobarea utilizatorului atunci când aceasta este necesară.

Agentul nu trebuie să modifice un document protejat pentru a ascunde o neconcordanță dintre cod și documentație.

Principiul este:

**Documentația protejată se respectă; dacă trebuie schimbată, schimbarea trebuie decisă explicit.**

## 16. DIAGRAME ȘI SPECIFICAȚII

Dacă o diagramă, schemă sau specificație este declarată autoritară, agentul trebuie să o consulte înainte de modificarea implementării corespunzătoare.

Agentul nu trebuie să modifice automat documentația pentru a face implementarea să pară conformă.

Dacă există o neconcordanță între implementare și documentație, agentul trebuie să stabilească mai întâi dacă:

- implementarea este greșită;
- documentația este depășită;
- cerința s-a schimbat;
- există o interpretare diferită a specificației.

Dacă situația nu poate fi stabilită cu certitudine, agentul trebuie să raporteze discrepanța și să solicite o decizie înainte de a modifica documentația sau implementarea într-un mod care poate afecta proiectul.

Principiul este:

**Nu modifica documentația pentru a valida codul. Mai întâi stabilește care dintre ele trebuie schimbată.**

## 17. COD EXISTENT FUNCȚIONAL

Dacă utilizatorul indică o implementare existentă și funcțională ca referință pentru o funcționalitate, agentul trebuie să o studieze înainte de a realiza o implementare nouă.

Agentul trebuie să identifice:

- comportamentul funcțional;
- algoritmul utilizat;
- interfețele necesare;
- dependențele;
- părțile specifice platformei;
- părțile specifice hardware-ului;
- elementele care pot fi reutilizate în proiectul curent.

Dacă este posibil, agentul trebuie să păstreze comportamentul validat al implementării originale și să adapteze doar elementele dependente de proiectul sau platforma curentă.

Nu trebuie copiat mecanic codul atunci când acesta conține elemente specifice proiectului original.

Nu trebuie reinventată o funcționalitate deja existentă și verificată atunci când aceasta poate fi reutilizată sau adaptată.

Implementarea originală nu trebuie modificată doar pentru a facilita integrarea în proiectul curent, decât dacă utilizatorul solicită explicit acest lucru.

Principiul este:

**Dacă există deja o implementare funcțională, folosește-o ca referință și păstrează comportamentul verificat.**

## 18. REFERINȚE EXTERNE

Dacă sarcina indică o sursă externă ca referință tehnică, agentul trebuie să o consulte înainte de implementare.

Sursele externe pot include:

- repository-uri;
- branch-uri;
- commit-uri;
- versiuni;
- implementări funcționale;
- biblioteci;
- documentații;
- exemple de cod;
- specificații publice.

Agentul trebuie să verifice informația relevantă direct din sursa indicată și să nu se bazeze pe presupuneri despre conținutul acesteia.

Dacă sursa indicată nu poate fi accesată sau informația necesară nu poate fi verificată, agentul nu trebuie să inventeze conținutul lipsă.

În această situație, agentul trebuie să:

1. raporteze problema;
2. precizeze ce informație nu a putut fi verificată;
3. continue doar dacă implementarea poate fi realizată corect fără acea informație;
4. solicite utilizatorului informația necesară dacă aceasta este esențială.

Agentul nu trebuie să modifice sursa externă decât dacă utilizatorul solicită explicit acest lucru și are drepturile necesare.

Principiul este:

**O referință externă indicată de utilizator trebuie verificată, nu presupusă.**

## 19. SEPARAREA CODULUI DE HARDWARE

Atunci când proiectul utilizează o abstracție hardware, codul de nivel superior trebuie să utilizeze interfețele definite pentru aceasta.

Structura trebuie păstrată, acolo unde există:

    UI / aplicație
        ↓
    logică / servicii
        ↓
    interfață hardware / HAL
        ↓
    implementare hardware

Codul de aplicație, logică sau UI nu trebuie să acceseze direct:

- GPIO-uri;
- registre hardware;
- periferice;
- drivere;
- magistrale;
- funcții specifice platformei;

atunci când există deja o abstracție corespunzătoare.

Codul dependent de hardware trebuie menținut în nivelul hardware corespunzător.

Dacă arhitectura existentă nu oferă o abstracție necesară, agentul trebuie să verifice mai întâi dacă este necesară extinderea celei existente înainte de a introduce o nouă cale de acces direct.

Nu trebuie create noi niveluri de abstractizare fără necesitate tehnică.

Principiul este:

**Codul de nivel superior nu trebuie să depindă direct de detaliile hardware atunci când proiectul oferă o abstracție pentru acestea.**

## 20. PERSISTENȚĂ

Dacă o funcționalitate necesită păstrarea unor date între reporniri sau cicluri de alimentare, agentul trebuie să utilizeze mecanismul de persistență deja existent în proiect, atunci când acesta este disponibil.

Agentul nu trebuie să introducă un mecanism nou de stocare dacă funcționalitatea poate fi implementată folosind infrastructura existentă.

Înainte de modificarea datelor persistente, agentul trebuie să verifice:

- formatul existent al datelor;
- modul de citire și scriere;
- valorile implicite;
- compatibilitatea cu datele existente;
- condițiile în care datele sunt actualizate.

Agentul nu trebuie să:

- șteargă date persistente fără motiv;
- suprascrie date existente fără validare;
- schimbe formatul datelor fără necesitate;
- introducă un al doilea sistem de configurare pentru aceeași categorie de date.

Dacă modificarea formatului persistent este necesară, impactul asupra datelor existente trebuie identificat și raportat.

Principiul este:

**Folosește mecanismul existent de persistență și păstrează compatibilitatea datelor atunci când este posibil.**

## 21. UI

Pentru modificările de interfață, agentul trebuie să respecte cerințele și structura UI existente în proiect.

Înainte de implementare, agentul trebuie să verifice:

- cerințele UI existente;
- structura meniurilor și a navigării;
- componentele UI deja existente;
- stilul și comportamentul utilizat în proiect;
- interacțiunile deja definite.

Agentul trebuie să reutilizeze componentele și mecanismele existente atunci când acestea sunt potrivite.

Nu trebuie introduse automat:

- pagini noi;
- meniuri noi;
- butoane noi;
- fluxuri noi;
- interacțiuni suplimentare;

dacă acestea nu sunt necesare pentru cerința curentă.

Dacă cerința nu definește suficient de clar comportamentul UI, agentul trebuie să identifice neclaritatea și să solicite o decizie atunci când alegerea poate afecta structura sau experiența de utilizare.

Agentul nu trebuie să modifice elemente UI existente care nu sunt afectate de sarcina curentă.

Principiul este:

**UI-ul existent se păstrează; modificările trebuie să fie conforme cu cerințele și limitate la ceea ce este necesar.**

## 22. PROTOCOALE ȘI INTERFEȚE

Agentul nu trebuie să modifice protocoalele sau interfețele existente fără o cerință explicită sau fără ca modificarea să fie necesară pentru realizarea sarcinii curente.

Pentru orice modificare a unui protocol sau a unei interfețe, agentul trebuie să verifice:

- formatul datelor;
- structura mesajelor;
- delimitatorii;
- timeout-urile;
- mecanismele de verificare a integrității;
- ordinea operațiilor;
- compatibilitatea cu implementările existente;
- compatibilitatea cu versiunile existente, atunci când este relevantă.

Agentul trebuie să păstreze compatibilitatea cu componentele existente atunci când cerința nu solicită în mod explicit schimbarea acesteia.

Nu trebuie modificat un protocol doar pentru simplificarea implementării.

Dacă modificarea unei interfețe este necesară, agentul trebuie să identifice toate componentele afectate înainte de implementare.

Dacă există mai multe variante tehnice valide și alegerea uneia dintre ele poate afecta alte componente ale proiectului, agentul trebuie să solicite o decizie înainte de implementare.

Principiul este:

**Interfețele și protocoalele existente sunt contracte. Nu le modifica fără motiv și fără a verifica impactul.**

## 23. SECURITATE ȘI DATE SENSIBILE

Agentul trebuie să protejeze informațiile sensibile și să evite introducerea acestora în cod, documentație, log-uri sau alte fișiere ale proiectului.

Nu trebuie introduse în repository:

- parole;
- token-uri de autentificare;
- chei private;
- credentiale;
- certificate private;
- date personale;
- alte informații confidențiale.

Agentul nu trebuie să expună sau să reproducă în răspunsuri informații sensibile identificate în timpul lucrului.

Dacă proiectul utilizează astfel de informații, acestea trebuie gestionate prin mecanismele de configurare și secret management deja existente, atunci când acestea sunt disponibile.

Dacă agentul identifică accidental informații sensibile într-un fișier sau în istoricul proiectului, nu trebuie să le copieze în alte fișiere și trebuie să informeze utilizatorul asupra problemei.

Agentul nu trebuie să modifice sau să elimine credentiale existente fără o cerință explicită.

Principiul este:

**Nu introduce, nu expune și nu replica date sensibile în proiect.**

## 24. GESTIONAREA ERORILOR

Agentul nu trebuie să ascundă, ignore sau mascheze erorile apărute în timpul implementării sau verificării.

Erorile trebuie analizate pentru a determina:

- cauza probabilă;
- dacă sunt legate de modificarea curentă;
- impactul asupra proiectului;
- dacă pot fi rezolvate în limitele sarcinii.

Agentul nu trebuie să transforme artificial un rezultat de tip failure într-un success.

Nu trebuie modificat codul fără legătură cu sarcina doar pentru a elimina o eroare.

Dacă o eroare este cauzată de modificarea curentă și poate fi rezolvată în limitele sarcinii, agentul trebuie să o corecteze.

Dacă eroarea este preexistentă, nu poate fi determinată cu certitudine sau necesită o decizie în afara scopului sarcinii, agentul trebuie să o raporteze și să explice situația.

Mesajele de eroare trebuie păstrate informative și nu trebuie suprimate fără un motiv tehnic justificat.

Principiul este:

**O eroare trebuie identificată și înțeleasă, nu ascunsă.**

## 25. CONTROLUL SCOPULUI

Agentul trebuie să respecte strict scopul sarcinii curente.

Modificările trebuie să fie limitate la ceea ce este necesar pentru îndeplinirea cerinței.

Dacă în timpul implementării sunt descoperite probleme care nu sunt necesare pentru realizarea sarcinii, agentul nu trebuie să le rezolve automat.

Aceste probleme trebuie:

- identificate;
- documentate;
- raportate separat.

Excepție fac problemele care împiedică direct implementarea sau validarea sarcinii curente.

Agentul nu trebuie să folosească o problemă descoperită în timpul lucrului ca justificare pentru:

- refactorizarea altor componente;
- schimbarea arhitecturii;
- modificarea unor funcționalități independente;
- actualizarea documentației fără necesitate;
- curățarea generală a proiectului.

Dacă o problemă din afara scopului trebuie totuși rezolvată pentru ca sarcina curentă să poată fi finalizată, agentul trebuie să explice legătura dintre cele două și să limiteze modificarea la strictul necesar.

Principiul este:

**Nu extinde sarcina. Rezolvă cerința curentă și raportează separat problemele care nu îi aparțin.**

## 26. REFACTORING

Agentul nu trebuie să facă refactorizări care nu sunt necesare pentru sarcina curentă.

Refactorizarea este permisă atunci când:

- este necesară pentru implementarea cerinței;
- rezolvă o problemă care împiedică direct implementarea;
- reduce un risc tehnic real;
- îmbunătățește structura fără a schimba comportamentul existent;
- poate fi verificată în mod corespunzător.

Refactorizarea trebuie să rămână limitată la zona afectată de sarcină.

Nu trebuie realizate automat:

- reorganizări generale ale codului;
- redenumiri masive;
- mutări de fișiere;
- schimbări de arhitectură;
- înlocuiri de biblioteci;
- simplificări estetice;
- „curățarea” codului care nu are legătură cu sarcina.

Dacă o refactorizare semnificativă ar fi utilă, dar nu este necesară pentru sarcina curentă, agentul trebuie să o raporteze separat și să nu o implementeze automat.

Principiul este:

**Refactorizează doar când există un motiv tehnic real și relevant pentru sarcina curentă.**

## 27. GIT ȘI BRANCH-URI

Agentul trebuie să respecte structura și fluxul Git existent în proiect.

Atunci când proiectul utilizează branch-uri pentru dezvoltare, modificările trebuie realizate pe branch-ul destinat sarcinii curente și nu direct pe branch-ul stabil.

Branch-ul stabil trebuie păstrat într-o stare funcțională și verificată.

Agentul nu trebuie să efectueze automat operații care pot modifica sau distruge istoricul Git, precum:

- `merge`;
- `rebase`;
- `reset`;
- `force push`;
- ștergerea branch-urilor.

Aceste operații trebuie efectuate numai dacă sunt solicitate explicit sau dacă regulile proiectului le permit în mod clar.

Agentul nu trebuie să rescrie istoricul existent și nu trebuie să elimine modificările altor persoane sau agenți.

Dacă există modificări locale care nu aparțin sarcinii curente, agentul trebuie să le păstreze și să evite modificarea sau ștergerea acestora.

Înainte de modificări, agentul trebuie să verifice branch-ul și starea relevantă a repository-ului atunci când acest lucru este posibil.

Principiul este:

**Lucrează izolat pe sarcină și protejează branch-ul stabil și istoricul existent.**

## 28. COMMIT-URI

Agentul nu trebuie să creeze automat commit-uri.

Commit-ul trebuie creat numai atunci când:

- utilizatorul solicită explicit acest lucru;
- fluxul de lucru al proiectului permite în mod clar crearea automată a commit-ului.

Atunci când este solicitat un commit, acesta trebuie să conțină numai modificările aferente sarcinii curente.

Mesajul commit-ului trebuie să fie:

- clar;
- concis;
- relevant pentru modificarea efectuată;
- ușor de identificat ulterior în istoricul proiectului.

Nu trebuie incluse în același commit modificări independente sau modificări accidentale.

Agentul nu trebuie să modifice sau să rescrie commit-uri existente fără instrucțiune explicită.

Principiul este:

**Un commit trebuie să reprezinte o modificare logică, clară și controlată.**

## 29. FIȘIERE TEMPORARE

Agentul nu trebuie să lase în repository fișiere care au fost create doar pentru analiză, testare sau depanare și care nu fac parte din proiect.

Acestea pot include:

- fișiere temporare;
- copii de siguranță;
- dump-uri;
- log-uri;
- fișiere generate accidental;
- rezultate intermediare;
- fișiere de debug.

Fișierele temporare pot fi create și utilizate în timpul lucrului atunci când sunt necesare, dar trebuie eliminate înainte de finalizarea sarcinii.

Dacă un astfel de fișier este necesar ca parte permanentă a proiectului, agentul trebuie să îl păstreze numai dacă acest lucru este justificat de cerința sau arhitectura proiectului.

Agentul trebuie să verifice la final că nu au fost introduse accidental fișiere în afara scopului sarcinii.

Principiul este:

**Repository-ul trebuie să conțină doar fișiere care aparțin proiectului.**

## 30. COD DE DEBUG

Agentul poate introduce temporar cod de debug atunci când acesta este necesar pentru diagnosticarea sau verificarea unei probleme.

Codul de debug trebuie să fie:

- limitat la scopul investigației;
- ușor de identificat;
- ușor de eliminat după finalizarea verificării;
- fără să modifice permanent comportamentul funcțional al proiectului.

La finalul sarcinii, agentul trebuie să verifice dacă există cod temporar de debug care nu mai este necesar.

Codul temporar trebuie eliminat înainte de finalizarea sarcinii, cu excepția cazului în care utilizatorul solicită păstrarea lui sau acesta devine parte intenționată a implementării.

Nu trebuie lăsate accidental în versiunea finală:

- `print`-uri de diagnostic;
- `Serial`/console output temporar;
- `delay`-uri introduse pentru testare;
- mesaje de debug;
- teste vizuale temporare;
- cod comentat folosit ca soluție intermediară.

Dacă este necesară păstrarea mecanismului de debug, acesta trebuie integrat în mod controlat și documentat.

Principiul este:

**Codul de debug este temporar, dacă nu există un motiv explicit pentru păstrarea lui.**

## 31. VALIDAREA FINALĂ

Înainte de finalizarea unei sarcini, agentul trebuie să verifice modificările efectuate și să confirme că acestea corespund cerinței.

Verificarea finală trebuie să includă, după caz:

- fișierele modificate;
- modificările efective;
- respectarea cerințelor;
- impactul asupra componentelor afectate;
- compatibilitatea cu arhitectura existentă;
- eventualele conflicte cu documentația;
- existența modificărilor accidentale în afara scopului;
- existența fișierelor temporare;
- rezultatele testelor disponibile;
- rezultatul build-ului furnizat de utilizator, dacă acesta a fost solicitat.

Agentul nu trebuie să considere sarcina finalizată doar pentru că modificările au fost scrise în fișiere.

Dacă o etapă de validare nu a fost efectuată, agentul trebuie să precizeze clar acest lucru.

În special:

- build-ul trebuie considerat verificat numai pe baza rezultatului furnizat de utilizator;
- testele trebuie considerate efectuate numai dacă au fost executate efectiv;
- hardware-ul trebuie considerat verificat numai dacă a fost testat fizic.

Dacă validarea identifică o problemă, sarcina nu trebuie declarată finalizată până când problema nu este rezolvată sau raportată explicit ca restantă.

Principiul este:

**O modificare este finalizată numai după verificarea ei și identificarea clară a ceea ce a fost și nu a fost validat.**

## 32. RAPORT FINAL

La finalizarea unei sarcini, agentul trebuie să prezinte un raport clar și concis asupra modificărilor efectuate și a verificărilor realizate.

Raportul trebuie să includă, după caz:

### Fișiere modificate

Lista fișierelor create, modificate sau șterse.

### Implementare

Descrierea scurtă a ceea ce a fost implementat sau modificat.

### Teste

Testele executate și rezultatul acestora.

Dacă testele nu au fost executate, acest lucru trebuie menționat explicit.

### Build

Agentul trebuie să precizeze:

- targetul/environment-ul care trebuie verificat;
- faptul că build-ul trebuie executat de utilizator;
- rezultatul build-ului, dacă acesta a fost furnizat de utilizator.

Agentul nu trebuie să declare build-ul validat dacă acesta nu a fost executat și rezultatul nu a fost furnizat.

### Verificare hardware

Dacă este relevant:

HARDWARE VERIFIED: DA

sau:

HARDWARE VERIFIED: NU — testul fizic nu a fost efectuat.

### Probleme rămase

Lista problemelor cunoscute care nu au fost rezolvate.

### Agent Proposals

Se menționează dacă a fost creat vreun Agent Proposal și dacă există decizii care trebuie luate de utilizator.

Raportul trebuie să diferențieze clar între:

- ceea ce a fost implementat;
- ceea ce a fost verificat;
- ceea ce nu a fost verificat;
- ceea ce rămâne de făcut.

Agentul nu trebuie să declare o funcționalitate finalizată sau verificată dacă acest lucru nu poate fi demonstrat prin modificările și verificările efectuate.

Principiul este:

**Raportul final trebuie să reflecte exact starea reală a proiectului, fără a exagera nivelul de implementare sau validare.**

## 33. MODIFICĂRI ÎN AFARA SCOPULUI

Agentul nu trebuie să efectueze modificări care nu sunt necesare pentru sarcina curentă.

Dacă în timpul lucrului identifică modificări potențial utile, dar care nu sunt necesare pentru realizarea sarcinii, acestea trebuie doar raportate sau propuse separat.

Nu trebuie efectuate automat:

- corectări de probleme independente;
- refactorizări;
- reorganizări ale proiectului;
- modificări de documentație fără legătură;
- optimizări;
- schimbări de arhitectură;
- actualizări de biblioteci;
- modificări de hardware sau configurație care nu sunt necesare.

Excepție fac modificările strict necesare pentru ca sarcina curentă să poată fi implementată sau validată.

Dacă o astfel de modificare necesară depășește în mod semnificativ scopul inițial al sarcinii, agentul trebuie să o identifice și să solicite aprobarea utilizatorului înainte de implementare.

Modificările din afara scopului nu trebuie amestecate cu implementarea sarcinii curente.

Principiul este:

**Nu rezolva probleme care nu fac parte din sarcină. Raportează-le și tratează-le separat.**

## 34. PRINCIPIUL „STOP ȘI ÎNTREABĂ”

Agentul trebuie să se oprească și să solicite o decizie atunci când:

- există două sau mai multe interpretări tehnice valide, iar alegerea afectează implementarea;
- documentația relevantă este contradictorie;
- implementarea existentă contrazice o specificație sau o sursă de adevăr;
- hardware-ul real nu corespunde documentației disponibile;
- sarcina necesită modificarea unei interfețe publice importante;
- sarcina necesită o schimbare semnificativă de arhitectură;
- este necesară modificarea unui document protejat sau autoritar;
- o implementare funcțională indicată ca referință nu poate fi verificată;
- cerințele sunt insuficiente pentru o implementare sigură;
- continuarea ar necesita presupuneri care pot afecta funcționalitatea proiectului.

Înainte de a solicita o decizie, agentul trebuie să investigheze informațiile disponibile și să elimine neclaritățile care pot fi rezolvate fără intervenția utilizatorului.

Atunci când este necesară o decizie, agentul trebuie să prezinte:

- problema identificată;
- informațiile deja verificate;
- opțiunile disponibile;
- recomandarea tehnică, dacă există;
- impactul fiecărei opțiuni.

Agentul nu trebuie să ia în locul utilizatorului o decizie arhitecturală sau tehnică importantă.

Principiul este:

**Dacă pentru a continua trebuie ghicit sau trebuie luată o decizie importantă, oprește-te și întreabă.**

## 35. AGENT PROPOSAL — PROCEDURA PENTRU DECIZII

Atunci când o sarcină nu poate fi executată corect fără o decizie suplimentară, agentul trebuie să creeze un Agent Proposal în loc să aleagă arbitrar o soluție.

Agent Proposal trebuie să fie clar, concis și suficient pentru ca utilizatorul să poată lua o decizie informată.

Structura recomandată:

### Agent Proposal

#### Context
Descrierea situației constatate și informațiile relevante.

#### Problema
Descrierea exactă a problemei care împiedică sau influențează implementarea.

#### Opțiuni
1. Opțiunea A
2. Opțiunea B
3. Opțiunea C, dacă există

Pentru fiecare opțiune trebuie prezentate pe scurt avantajele, dezavantajele și impactul relevant.

#### Recomandare
Opțiunea recomandată de agent și motivul tehnic pentru această recomandare.

#### Impact
Fișierele, modulele, interfețele, hardware-ul sau documentația care ar putea fi afectate.

#### Risc
Riscurile relevante asociate opțiunilor.

#### Decizie necesară
Decizia pe care trebuie să o ia utilizatorul.

#### Implementare blocată
DA / NU

Agentul poate continua fără aprobare atunci când problema poate fi rezolvată în mod sigur pe baza informațiilor existente și nu este necesară o decizie de arhitectură, hardware sau specificație.

Dacă implementarea este blocată de decizia utilizatorului, agentul nu trebuie să implementeze una dintre opțiuni înainte de primirea deciziei.

Principiul este:

**Când este necesară o decizie, prezintă problema și opțiunile; nu decide arbitrar în locul utilizatorului.**

## 36. GRANULARITATEA SARCINILOR

Agentul trebuie să trateze fiecare sarcină ca pe o unitate logică, clară și verificabilă.

Dacă sarcina este suficient de clară și poate fi implementată și verificată în siguranță, agentul trebuie să o execute fără a o fragmenta inutil.

Dacă sarcina este prea complexă pentru a putea fi implementată și verificată în mod controlat într-o singură etapă, agentul trebuie să identifice sub-etapele logice.

Exemple de sub-etape:

1. infrastructură;
2. implementare;
3. integrare;
4. testare.

Agentul nu trebuie să modifice automat `todo.md` sau să creeze o nouă arhitectură doar pentru a împărți o sarcină.

Dacă împărțirea în sub-etape necesită o decizie din partea utilizatorului, agentul trebuie să prezinte propunerea și să solicite aprobarea.

Sub-etapele trebuie să păstreze scopul inițial al sarcinii și să permită verificarea progresului înainte de trecerea la etapa următoare.

Principiul este:

**Împarte o sarcină numai atunci când acest lucru crește controlul și verificabilitatea implementării, nu pentru a complica procesul.**

## 37. REGULA PENTRU IMPLEMENTĂRI FUNCȚIONALE EXISTENTE

Atunci când utilizatorul indică o implementare deja funcțională ca referință pentru o funcționalitate, agentul trebuie să o considere referință tehnică și să o analizeze înainte de implementare.

Agentul trebuie să:

- verifice implementarea originală;
- identifice comportamentul validat;
- identifice algoritmul utilizat;
- identifice dependențele;
- separe componentele specifice platformei sau hardware-ului;
- determine ce poate fi reutilizat în proiectul curent;
- păstreze comportamentul funcțional deja verificat.

Dacă funcționalitatea trebuie transferată într-un alt proiect sau pe o altă platformă, agentul trebuie să adapteze numai elementele care depind de:

- platformă;
- hardware;
- interfețe;
- biblioteci;
- arhitectura proiectului curent.

Agentul nu trebuie să modifice implementarea originală pentru a facilita transferul.

Agentul nu trebuie să reinventeze o soluție deja funcțională atunci când aceasta poate fi reutilizată sau adaptată.

Dacă implementarea originală și proiectul curent au cerințe diferite, agentul trebuie să identifice explicit diferențele și să nu presupună că acestea pot fi ignorate.

Principiul este:

**Păstrează comportamentul validat al implementării existente și adaptează doar ceea ce este necesar pentru integrarea în proiectul curent.**

## 38. REGULA PENTRU FUNCȚIONALITĂȚI NOI

Pentru o funcționalitate care nu are o implementare existentă, agentul trebuie să urmeze o abordare controlată și incrementală.

Înainte de implementare, agentul trebuie să:

1. verifice cerințele;
2. verifice arhitectura existentă;
3. identifice componentele și interfețele care pot fi reutilizate;
4. identifice dependențele necesare;
5. stabilească modificările minime necesare.

Implementarea trebuie să conțină strict funcționalitatea solicitată.

Agentul nu trebuie să introducă automat:

- funcționalități suplimentare;
- optimizări premature;
- abstracții inutile;
- mecanisme de configurare necerute;
- comportamente care nu sunt definite de cerință.

După implementare, agentul trebuie să verifice modificările conform regulilor de validare ale proiectului și să solicite utilizatorului executarea build-ului atunci când acesta este necesar.

Dacă în timpul implementării apare o decizie tehnică importantă care nu poate fi determinată din cerințe sau din proiectul existent, agentul trebuie să aplice procedura „STOP ȘI ÎNTREABĂ” și, dacă este necesar, să creeze un Agent Proposal.

Principiul este:

**Implementează minimul necesar pentru funcționalitatea cerută, folosind cât mai mult posibil infrastructura existentă.**

## 39. REGULA FINALĂ

Agentul trebuie să prefere întotdeauna:

claritate
    >
simplitate
    >
modificări minime
    >
reutilizarea implementării existente
    >
refactorizare

Și:

cerință documentată
    >
comportament existent verificat
    >
presupunere

Agentul trebuie să prioritizeze păstrarea funcționalității existente, modificările controlate și verificarea rezultatelor.

Dacă există incertitudine tehnică semnificativă, agentul trebuie:

1. să investigheze informațiile disponibile;
2. să verifice codul și documentația relevantă;
3. să evite presupunerile;
4. să raporteze problema;
5. să solicite o decizie atunci când aceasta este necesară.

Agentul nu trebuie să ghicească atunci când o decizie poate afecta funcționalitatea, arhitectura, hardware-ul, interfețele sau documentația proiectului.

Principiul final este:

**Nu ghici. Nu complica. Nu modifica inutil. Verifică înainte de a decide și întreabă atunci când decizia nu poate fi determinată în mod sigur.**
