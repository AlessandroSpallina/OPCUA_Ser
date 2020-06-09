# OPC_UA_Server

## Overview

Il progetto consiste nella realizzazione di un custom server, secondo le specifiche imposte dallo  standard IEC 62541, tramite l'utilizzo della libreria ANSI C Open62541 v1.1.
Il server espone nodi personalizzati nell'AddressSpace. Implementa i meccanismi standard di sicurezza per la cifratura e la firma digitale.
Implementa inoltre il meccanismo PubSub *brokerless* utilizzando il profilo di trasporto default UDP UADP, avendo comunque la possibilità di adottare il profilo Eth UADP. 

Maggiori dettagli sulla libreria sono disponibili  
* [Open62541](https://open62541.org/) 
* [Open62541 - Docs](https://open62541.org/doc/current/)
* [Open62541 - Github](https://github.com/open62541/open62541)

Maggiori dettagli sui profili di trasporto sono disponibili 
* [UDP UADP](http://opcfoundation-onlineapplications.org/ProfileReporting/index.htm?ModifyProfile.aspx?ProfileID=2faacf36-fea4-4004-be6e-89456642e831)
* [ETH UADP](http://opcfoundation-onlineapplications.org/ProfileReporting/index.htm?ModifyProfile.aspx?ProfileID=3195952f-498d-4c3b-94bd-6b8e4fd33cc7)

Il progetto è stato sviluppato su piattforma Windows, vista la limitazione dell'esecuzione del meccanismo PubSub esclusivamente su ambiente linux.
La funzionalità di Publisher è stata comunque implementata e testata su linux. Il codice prodotto è comunque eseguibile su Windows a meno del meccanismo PubSub

Di seguito è riportata la procedura per build delle librerie necessarie e build ed esecuzione del progetto dovuta alla carenza di informazioni reperibili sulla documentazione per lo sviluppo 
in ambiente windows. 

E' fortemente consigliato l'IDE Microsoft Visual Studio 2019, strumento utilizzato per lo sviluppo. 
Per maggiori specifiche sulle funzionalita si rimanda a [link prelazione](vattelapesca)

## Getting Started

Tutti i progetti, librerie comprese, sono stati compilati in modalità win32 (x86).
Compilazioni con configurazioni diverse non assicura il successo della compilazione.

### Prerequisites
La configurazione delle librerie e l'abilitazione dei relativi flag necessari per le diverse funzionalità 
è effettuata tramite *Cmake* disponibile al seguente [link](https://cmake.org/download/).

Per abilitare i meccanismi di sicurezza della libreria è necessario scaricare ed installare la libreria **mbedtls** disponibile [qui](https://tls.mbed.org/download).
La procedura per l'installazione è disponibile al paragrafo seguente. 

E' possibile seguire i passi descritti in questa guida anche per l'installazione in ambiente linux, modificando opportunamente le istruzioni di sistema.
 
##### Mbedtls

Mbedtls fornisce gli strumenti necessari per i meccanismi di sicurezza quali gestione delle chiavi, gestione dei certificati e degli algoritmi di  firma e cifratura.
Una volta scaricato il file dal link al paragrafo precedente, è necessario estrarre il file .tgz che conterra a sua volta uns econdo .tgz anch'ssso da estrarre.
 Creare una cartella **build** all'interno del path
```
C:\path\to\mbedtls-2.16.6-apache\mbedtls-2.16.6
```
ottenendo il full path 
```
C:\path\to\mbedtls-2.16.6-apache\mbedtls-2.16.6\build
```
Aprire Cmake-gui e selezionare nel campo *source* il primo path e come *build* path il secondo path.
Successivamente dare il comando *Configure* seguito da *Generate*. 

**N.B.** Assicurarsi che nella finestra CmakeSetup sia selezionato come **Generator** Visual Studio 16 2019 (l'ambiente utilizzato è comunque personalizzabile)
e la piattaforma di generazione **win32**
Verificata l'assenza di errori della configurazione, si ottiene come risultato la soluzione Visual Studio al path

```
C:\path\to\tombedtls-2.16.6-apache\mbedtls-2.16.6\visualc\VS2010
```
Aprire la soluzione **mbedtls.sln**, con Visual Studio in modalità **_amministratore_** e fare build dell'intera soluzione.
Successivamente selezionare il progetto *INSTALL* e compilare in modalità *Project Only* 

Il risultato dell'intera procedura è l'installazione della libreria sul sistema. 

##### Open62541

Per la compilazione di Open62541, su una directory arbitraria
```
git clone https://github.com/open62541/open62541.git
git submodule update --init --recursive
cd open62541
mkdir build
``` 
Aprire Cmake-gui e selezionare come *source* il path
``` 
C:\path\to\open62541
```
  e come *build* il path 
```
C:\path\to\open62541\build
```
Cliccare su Configure per generare la lista di parametri da abilitare necessari alle funzionalità del server.
Il risultato dell'operazione sarà il seguente
```
inserire immagine Cmake con gli Enable
```
La prima cosa da fare è abilitare il flag 
```
UA_ENABLE_ENCRYPTION
UA_NAMESPACE_ZERO FULL
```
**N.B.** su Windows non è possibile usufruire del meccanismo PubSub quindi è possibile non abilitare i seguenti flag. Su Linux invece è necessario abilitarli per testare PubSub
```
UA_ENABLE_PUBSUB
UA_ENABLE_PUBSUB_DELTAFRAMES
UA_ENABLE_PUBSUB_INFORMATIONMODEL
UA_ENABLE_PUBSUB_ETH_UADP
```
Abilitati i flag è necessario cliccare su *configure* prima di generare la soluzione. Ciò renderà disponibili flag aggiuntivi nel quale inserire le dipendenze di *mbedtls*
```
MBEDTLS_INCLUDE_DIRS --> C:\Program Files (x86)\mbed TLS\include
MBEDTLS_LIBRARY      --> C:\Program Files (x86)\mbed TLS\lib\mbedtls.lib
MBEDCRYPTO_LIBRARY   --> C:\Program Files (x86)\mbed TLS\lib\mbedcrypto.lib
MBEDX509_LIBRARY     --> C:\Program Files (x86)\mbed TLS\lib\mbedx509.lib
```
Adesso è possibile cliccare *Generate* per generare la soluzione **open62541.sln** sul path 
```
 C:\path\to\open62541\build
```

Aprire la soluzione con Visual Studio in modalità **_amministratore_** e fare build dell'intera soluzione.
Successivamente selezionare il progetto *INSTALL* e compilare in modalità *Project Only* 

Come risultato si otterrà l'installazione della libreria sul path 
```
C:\Program Files (x86)\open62541
```
All'interno saranno disponibili i sorgenti sulla cartella `include`  e le librerie  ` .lib`  e ` .dll`


Copiare e incollare la libreria *open625421.dll* sui path *C:\Windows\SysWOW64* e *C:\Windows\System32*
Successivamente da prompt dei comandi 
```
cd C:\Windows\SysWOW64
regsvr32 open62541.dll
cd C:\Windows\System32
regsvr32 open62541.dll
```

### Configuration IDE
Installata la libreria, per il debug dell'applicazione è necessario configurare l'IDE. Nello specifico
è necessario linkare la libreria open62541.lib e aggiungere certificato e chiave privata come argomenti per abilitare la cifratura e la firma. 

La seguente procedura è stata utilizzata per l'esecuzione su Visual Studio.
```
git clone https://github.com/massimo-gollo/OPC_UA_Project.git
``` 
Aprire la solutione  `OPC_UA_SERVER` e successivamente cliccare su 
```
Project > OPC_UA_SERVER Properties > C/C++ > General > Additional Include Directories > Edit >  C:\Program Files(x86)\open62541\include
Project > OPC_UA_SERVER Properties > Linker > Input > Additional Dependencies > Edit >          C:\Program Files(x86)\open62541\lib\open62541.lib
``` 

#### Build and Running

Da questo momento è possibile compilare ed eseguire l'applicazione. 
Il server accetta argomenti da riga di comando che influenzano il suo comportamento.
In particolare, se vengono passati come command argument:

* Nessun argomento -> Si otterra un simple server senza alcun meccanismo di sicurezza (Basic - None). Se eseguito su Linux, il meccanismo PubSub è implementato di default con UrlAddress Multicast `224.0.0.22:4840` con TrasportProfile PubSub UDP UADP
* Certificato e chiave privata -> Il server implementa tutti i meccanismi di sicurezza per firma e cifratura. Esporrà diversi endpoints, uno per ogni meccanismo richiesto
* CustomUrl -> per meccanismo PubSub sotto UDP UADP 
* CustomUrl Ethernet -> per meccanismo PubSub sotto ETH UADP

Da editor, l'inserimento dei command argument si ottiene sotto le opzioni 
```
Project > OPC_UA_SERVER Properties > General > Command Arguments > Edit > --cert [Path to cert] --key [Path to key]
``` 
Risultato analogo si ottiene con la compilazione della soluzione e l'esecuzione da terminale 

```
cd path/to/Debug 
OPC_UA_Server.exe --cert path/to/Cert.der --key path/to/key .der
``` 

Per la generazione del certificato e della chiave (file .der) si rimanda alle istruzioni fonite nella sezione [Cert Generation](https://github.com/open62541/open62541/tree/master/tools/certs)
## Authors

* **Alessandro Spallina** 
* **Massimo Gollo** 

### TODO SECTION 
#### Connection Handling
Specifica i parametri di connessione per la creazione e la cancellazione a runtime. Viene configurato il TransportLayer. 
Si definisce la Struct di configurazione per Pubsub **UA_PubSubConnectionConfig** 
di cui in seguito si farà l'attach all'entità server. Tra i campi della struct si settta il nome della configurazione 
(dato che è possibile definire più configurazioni per il transport layer), il profilo del livello di trasporto, l'addressUrl
 e il publisherId (statico) per recuperarlo poi nel subscriber.

#### PublishedDataSet handling
PublishedDataset (PDS) e PubSubConnection sono entità toplevel standalone. PDS è il modulo incaricato di collezionare **Dataset**, 
payload dei messaggi forniti dal publisher e consumati dal subscriber. 

### Overoll objective 


E' stato realizzato un Custom ObjectType per tipizzare l'oggetto *WeatherType* per modellare tre stazioni meteo. L'oggetto weather è composto da 3 variabili built-in: 
* CityName - Stringa per il nome della città/stazione meteo
* Humidity - Float per l'umidità
* Temperature - Float per la temperatura

```
qui immagine 
```
Per ogni variabile eccetto città, si specificano le proprietà *Range* e *UnitOfMeasurement*.
E' stata realizzata una callback per l'aggiornamento periodico dei parametri. Idealmente sarebbe un API che raccoglie dati da una sorgente e li rende disponibili sul server. 
Inizialmente la sorgente prevista era OpenWeatherMap, API che offre dati dalle stazioni meteo dislocate in tutto il mondo. Ai fini del progetto non è stato conveniente a causa della frequenza di aggiornamento lenta con conseguente
aggiornamento del dato sul server lento. Inoltre è presente un limite fisico alle chiamate da poter effettuare in un arco di tempo e non sarebbe stato comunque evidente
una variazione significativa del dato. Per questo è stato realizzato un dato di prova al fine di verificare il funzionamento del meccanismo di Subscribe e del meccanismo PubSub.

```
parlare della sicurezza 
```