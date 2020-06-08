# OPC_UA_Server

## Overview

Il progetto consiste nella realizzazione di un custom server, seguendo lo standard IEC 62541, tramite l'utilizzo della libreria ANSI C Open62541 v1.1.
Il server espone nodi personalizzati nell'AddressSpace. Implementa i meccanismi basilari di sicurezza per la cifratura e la firma digitale.
Implementa inoltre il meccanismo PubSub brokerless. Nello specifico agisce da publisher sotto Eth UADP o UDP UADP

Maggiori dettagli sulla libreria sono disponibili su  
* [Open62541](https://open62541.org/) 
* [Open62541 - Docs](https://open62541.org/doc/current/)
* [Open62541 - Github](https://github.com/open62541/open62541)

Il progetto è stato sviluppato su piattforma Windows, vista la limitazione dell'esecuzione del meccanismo PubSub esclusiva su ambiente linux.
La funzionalità di Publisher è stata comunque implementata e testata in ambiente linux. 
Il codice è stato prodotto in modo tale da eseguire su entrambe le piattaforme senza conflitti.
Si riporta di seguito la procedura per build ed esecuzione su ambiente Windows, ma è possibile 

## Getting Started

Tutti i progetti, librerie comprese, sono stati compilati in modalità win32 (x86).
Compilare con configurazioni diverse non assicura il successo della compilazione.
La configurazione delle librerie è effettuata tramite *Cmake* disponibile [qui](https://cmake.org/download/).
E' consigliato (e utilizzato per il seguente progetto) l'IDE Visual Studio 2019.
### Prerequisites

Le seguenti istruzioni sono necessarie per compilare il progetto OPC_UA_Server in ambiente windows in quanto la documetazione non offre istruzioni specifiche. Per la compilazione delle librerie su linux si rimanda alla docs ritenuta esaustiva. 
Qualora si eseguisse su linux, si puo' comunque utilizzare la presente come guida per abilitare correttamente i flag al fine di accedere alle funzionalità avanzate della libreria
quali *Sicurezza* e *PubSub Mechanism*. 
 
##### Mbedtls
Per abilitare la sicurezza in Open62541 è necessario prima scaricare e installare la libreria **mbedtls** disponibile [qui](https://tls.mbed.org/download). 
mbedtls contiene gli strumenti necessari per i meccanismi di sicurezza, come gestione delle chiavi, dei certificati e degli algoritmi di cifratura.
E' necessario estrarre i due file .tgz nel file e creare una cartella **build** su 
```
C:\Users\UserName\Downloads\mbedtls-2.16.6-apache\mbedtls-2.16.6
```
ottenendo il full path 
```
C:\Users\UserName\Downloads\mbedtls-2.16.6-apache\mbedtls-2.16.6\build
```
Aprire Cmake-gui e selezionare come *source* il primo path e come *build* path il secondo path. Successivamente dare il comando *Configure* seguito da *Generate*. 

**N.B.** verificare l'assessa di errori della configurazione. Assicurarsi che nella finestra CmakeSetup sia selezionato il *Generator* per il progetto (nel nostro caso Visual Studio 16 2019) e la piattaforma di generazione
(win32)

Come risultato si otterra la soluzione Visual Studio al path

```
C:\Users\UserName\Downloads\mbedtls-2.16.6-apache\mbedtls-2.16.6\visualc\VS2010
```
Aprire la soluzione **mbedtls.sln** in modalità **_amministratore_** e fare la build dell'intera soluzione. Successivamente selezionare il progetto *INSTALL* e compilare in modalità *Project Only* 

Il risultato dell'intera procedura è l'installazione della libreria. 

##### Open62541

Per la compilazione di Open62541, su una directory arbitraria
```
git clone https://github.com/open62541/open62541.git
git submodule update --init --recursive
cd open62541
mkdir build
``` 
Aprire Cmake-gui e selezionare come *source* il path ` C:\...\open62541`  e come *build* il path ` C:\...\open62541\build` 
Cliccare su Configure per generare la lista di parametri da abilitare in base alle esigenze. Il risultato dell'operazione sarà il seguente
```
inserire immagine
```
La prima cosa da fare è abilitare il flag 
```
UA_ENABLE_ENCRYPTION
UA_NAMESPACE_ZERO FULL

```
**N.B.** su Windows non è possibile usufruire del meccanismo PubSub quindi è possibile non abilitare i seguenti flag. 
```
UA_ENABLE_PUBSUB
UA_ENABLE_PUBSUB_DELTAFRAMES
UA_ENABLE_PUBSUB_INFORMATIONMODEL
UA_ENABLE_PUBSUB_ETH_UADP
```
Abilitati i flag è necessario cliccare su *configure* prima di generare la soluzione. Ciò sbloccherà flag aggiuntivi nel quale inserire le dipendenze di *mbedtls*
```
MBEDTLS_INCLUDE_DIRS C:\Program Files (x86)\mbed TLS\include
MBEDTLS_LIBRARY C:\Program Files (x86)\mbed TLS\lib\mbedtls.lib
MBEDCRYPTO_LIBRARY C:\Program Files (x86)\mbed TLS\lib\mbedcrypto.lib
MBEDX509_LIBRARY C:\Program Files (x86)\mbed TLS\lib\mbedx509.lib
```
A questo punto cliccare su *Generate* per generare la soluzione **open62541.sln** sul path ` C:\...\open62541\build` 
Aprire la soluzione in modalità **_amministratore_**, fare build dell'intera soluzione e successivamente Build>Project Only sul progetto INSTALL per installare la libreria


Come risultato si otterrà l'installazione della libreria sul path `C:\Program Files (x86)\open62541` comprensiva di` include` , ` .lib`  e ` .dll` 


Copiare e incollare la libreria *open625421.dll* sui path *C:\Windows\SysWOW64* e *C:\Windows\System32*
Successivamente da prompt dei comandi 
```
cd C:\Windows\SysWOW64
regsvr32 open62541.dll
cd C:\Windows\System32
regsvr32 open62541.dll
```

### Configuration IDE
Installata la libreria, per il debug dell'applicazione è necessario configurare l'IDE Visual Studio. Nello specifico
è necessario linkare la libreria open62541.lib e aggiungere certificato e chiave privata come argomenti per abilitare la cifratura e la firma. 

```
git clone https://github.com/massimo-gollo/OPC_UA_Project.git
``` 

Aprire la solution  `OPC_UA_SERVER` e successivamente cliccare su 
```
Project > OPC_UA_SERVER Properties > C/C++ > General > Additional Include Directories > Edit >  C:\Program Files(x86)\open62541\include
Project > OPC_UA_SERVER Properties > Linker > Input > Additional Dependencies > Edit > C:\Program Files(x86)\open62541\lib\open62541.lib
``` 

#### Build and Running

Da questo momento è possibile compilare ed eseguire l'applicazione. 
Il server accetta argomenti da riga di comando che influenzano il suo comportamento.
In particolare, se vengono passati come command argument:

* Nessun argomento -> Si otterra un simple server senza alcun meccanismo di sicurezza (Basic - None). Se eseguito su Linux, il meccanismo PubSub è implementato di default con UrlAddress Multicast 224.0.0.22:4840 con TrasportProfile PubSub UDP UADP
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