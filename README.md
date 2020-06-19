# OPC_UA_Server

## Overview

Il progetto consiste nella realizzazione di un custom Server, secondo le specifiche imposte dallo standard IEC 62541, tramite l'utilizzo della libreria ANSI C Open62541 v1.1.
Il Server espone nodi personalizzati nell'AddressSpace. 
Implementa i meccanismi standard di sicurezza per la cifratura e la firma digitale.
Implementa inoltre un Publisher secondo il meccanismo PubSub *brokerless* utilizzando il profilo di trasporto UDP UADP.

Viene, inoltre, rilasciato un ulteriore Server OPC UA che agisce da Subscriber al fine di verificare il corretto funzionamento del maccanismo PubSub.
Questo Server processa i pacchetti ricevuti dal Publisher e ne espone i dati sull'AddressSpace.

Maggiori dettagli sullo stack C utilizzato sono disponibili ai seguenti link 
* [Open62541](https://open62541.org/) 
* [Open62541 - Docs](https://open62541.org/doc/current/)
* [Open62541 - Github](https://github.com/open62541/open62541)

Maggiori dettagli sul profilo di trasporto implementato sono disponibili al seguente link
* [UDP UADP](http://opcfoundation-onlineapplications.org/ProfileReporting/index.htm?ModifyProfile.aspx?ProfileID=2faacf36-fea4-4004-be6e-89456642e831)

Sono state utilizzate librerie standard C e open62541, questa è multipiattaforma, a meno di alcune funzionalità sperimentali che non sono state incluse nel progetto.

Si rilascia il codice sorgente corredato di _Solution_ Visual Studio, si consiglia quindi questo IDE per la compilazione del progetto su sistema operativo Windows. 

Di seguito è riportata la procedura per il build delle dipendenze del progetto, istruzioni necessarie poichè la documentazione è carente di informazioni per l'installazione della libreria in ambiente Windows.
Per informazioni su compilazione e installazione delle librerie in ambiente Linux si rimanda alla [documentazione ufficiale](https://open62541.org/doc/current/building.html) ritenuta esaustiva.

Specifiche tecniche sulle funzionalita del Server OPC UA e sul meccanismo PubSub sono disponibili nella [relazione finale](https://github.com/massimo-gollo/OPC_UA_Project/blob/develope/res/Relazione%20progetto%20Industrial%20Informatics%20.pdf)

## Getting Started

Tutti i progetti, librerie comprese, sono stati compilati in modalità win32 (x86).
Configurazioni diverse non assicurano il successo della compilazione.

Al fine di potere compilare con successo i due Server che implementano il Publisher e il Subscriber sono necessarie due versioni distinte di open62541, questo è dovuto al fatto che il team di sviluppo dello stack non ha ancora rilasciato le interfacce necessarie relative al Subscriber.
Per ovviare a questa limitazione è necessario compilare open62541 come libreria statica e importare interfacce private della libreria, normalmente non visibili se si dovesse utilizzare open62541 come libreria di terze parti, condivisa e installata sul sistema operativo. 
Le interfacce del Publisher nella versione 1.1 della libreria sono già state rilasciate.

### Prerequisites
La configurazione delle librerie e l'abilitazione dei relativi flag necessari per le diverse funzionalità 
è effettuata tramite *CMake* disponibile al seguente [link](https://cmake.org/download/).

Per abilitare i meccanismi di sicurezza della libreria è necessario scaricare ed installare la libreria **mbedtls** disponibile [qui](https://tls.mbed.org/download).
La procedura per l'installazione è disponibile al paragrafo seguente. 
 
##### Mbedtls

Mbedtls fornisce gli strumenti necessari per i meccanismi di sicurezza quali gestione delle chiavi, gestione dei certificati e degli algoritmi di  firma e cifratura.
Una volta scaricato il file dal link al paragrafo precedente, procedere con l'estrazione su directory arbitraria.
Creare una cartella **build** all'interno del path
```
C:\path\to\mbedtls-X.Y.Z-apache\mbedtls-2.16.6
```
ottenendo il full path 
```
C:\path\to\mbedtls-X.Y.Z-apache\mbedtls-2.16.6\build
```
Aprire CMake-gui e inserire come campo *source* il primo path e come *build* path il secondo path.
Successivamente dare il comando *Configure* seguito da *Generate*. 

**N.B.** Assicurarsi che nella finestra CMakeSetup sia selezionato come **Generator** Visual Studio 16 2019 (l'ambiente utilizzato è comunque personalizzabile)e la piattaforma di generazione **win32**
Verificata l'assenza di errori della configurazione, si ottiene come risultato la Solution Visual Studio al path

```
C:\path\to\tombedtls-2.16.6-apache\mbedtls-2.16.6\build
```
Aprire la soluzione **mbed TLS.sln**, con Visual Studio in modalità **_amministratore_** e fare build dell'intera soluzione.
Successivamente selezionare il progetto *INSTALL* e compilare in modalità *Project Only* 

Il risultato dell'intera procedura è l'installazione della libreria sul sistema. 

##### Open62541

Per la compilazione di Open62541, su una directory arbitraria
```
git clone https://github.com/open62541/open62541.git
cd open62541
git submodule update --init --recursive
mkdir build
``` 
Aprire CMake-gui e selezionare come *source* il path
``` 
C:\path\to\open62541
```
e come *build* il path 
```
C:\path\to\open62541\build
```
Cliccare su Configure per generare la lista di parametri da abilitare necessari alle funzionalità del server.
Il risultato dell'operazione sarà il seguente

<details>
  <summary>Cmake img (da aggiornare)</summary>
  <img src="https://raw.githubusercontent.com/massimo-gollo/OPC_UA_Project/develope/res/imgs/cmake-open62541-linux.png?token=ACSJQODP5I44RBOMR4FZCZ265CTJU" alt="ca">
  
</details>

La prima cosa da fare è abilitare i flag di compilazione
```
UA_ENABLE_ENCRYPTION
UA_NAMESPACE_ZERO FULL
UA_ENABLE_PUBSUB
UA_ENABLE_PUBSUB_INFORMATIONMODEL
BUILD_SHARED_LIBS
```

Abilitati i flag è necessario cliccare su *Configure* prima di generare la soluzione. Ciò renderà disponibili flag aggiuntivi nel quale inserire le dipendenze di *mbedtls*
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
All'interno saranno disponibili i sorgenti sulla cartella `include`  e le librerie ` .lib` e ` .dll`

Copiare e incollare la libreria *open625421.dll* sui path *C:\Windows\SysWOW64* e *C:\Windows\System32*
Successivamente da prompt dei comandi 
```
cd C:\Windows\SysWOW64
regsvr32 open62541.dll
cd C:\Windows\System32
regsvr32 open62541.dll
```

### Configuration IDE
Installata la libreria, per la compilazione dell'applicazione è necessario configurare l'IDE. 
Nello specifico è necessario linkare la libreria open62541.lib e aggiungere certificato e chiave privata come argomenti per abilitare la cifratura e la firma. 

```
git clone https://github.com/massimo-gollo/OPC_UA_Project.git
``` 
Aprire la solutione `OPC_UA_SERVER` e successivamente cliccare su 
```
Project > OPC_UA_SERVER Properties > C/C++ > General > Additional Include Directories > Edit >  C:\Program Files(x86)\open62541\include
Project > OPC_UA_SERVER Properties > Linker > Input > Additional Dependencies > Edit >          C:\Program Files(x86)\open62541\lib\open62541.lib
``` 

#### Build and Running

Da questo momento è possibile compilare ed eseguire l'applicazione. 
Il server accetta argomenti da riga di comando che influenzano il suo comportamento.
In particolare, se vengono passati come argomenti:

* Nessun argomento -> Si otterrà un simple server senza alcun meccanismo di sicurezza (Basic - None). Il meccanismo PubSub è abilitato di default con UrlAddress Multicast `224.0.0.22:4840` con TrasportProfile PubSub UDP UADP
* Certificato e chiave privata -> Il server implementa tutti i meccanismi di sicurezza per firma e cifratura. Esporrà diversi endpoints, uno per ogni meccanismo richiesto
* CustomUrl -> personalizzare l'url multicast per PubSub sotto UDP UADP 

Da IDE VisualStudio, l'inserimento degli argomenti si ottiene dalle opzioni 
```
Project > OPC_UA_SERVER Properties > General > Command Arguments > Edit > --cert [Path to cert] --key [Path to key]
``` 
Risultato analogo si ottiene con la compilazione della soluzione e l'esecuzione da terminale 

```
cd path/to/Debug 
OPC_UA_Server.exe --cert path/to/Cert.der --key path/to/key .der
``` 

Per la generazione del certificato e della chiave (file .der) si rimanda allo script fonit nella sezione [Cert Generation](https://github.com/massimo-gollo/OPC_UA_Project/tree/develope/res/certs)

## Authors

* **Massimo Gollo** 
* **Alessandro Spallina** 


