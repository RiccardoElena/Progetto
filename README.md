# Progetto Laboratorio sistemi operativi

## Indice

- [Membri del gruppo](#membri-del-gruppo)
- [Compilazione e Utilizzo](#compilazione-e-utilizzo)
  - [Tecnologie necessarie per installazione e utilizzo](#tecnologie-necessarie-per-installazione-e-utilizzo)
  - [Compilazione](#compilazione)
    - [Compilazione Client](#compilazione-client)
    - [Compilazione Server](#compilazione-server)
  - [Avvio](#avvio)
    - [Avvio Client](#avvio-client)
      - [Lanciare su SDK](#lanciare-su-sdk)
      - [Lanciare su un robot Furhat](#lanciare-su-un-robot)
    - [Avvio Server](#avvio-server)
- [Personalizzazione](#personalizzazione)
  - [Personalizzazione Client](#personalizzazione-client)
  - [Personalizzazione Server](#personalizzazione-server)
- [Load Testing](#load-testing)

## Membri del gruppo

- [Riccardo Elena](https://github.com/RiccardoElena)
- [Pasquale Miranda](https://github.com/43616F73)

## Compilazione e Utilizzo

### Tecnologie necessarie per installazione e utilizzo

Per **l'installazione e l'avvio** del progetto è necessario aver installato e attivi:

- [Docker](https://www.docker.com) e [Docker-compose](https://docs.docker.com/compose/)
- [JVM](https://www.java.com/it/)
- [Furhat SDK](https://docs.furhat.io/getting_started/) oppure avere un robot ***Furhat*** utilizzabile.

### Compilazione

#### Compilazione Client

Non è necessaria alcuna compilazione. È sufficiente assicurarsi che sia presente il file `Client/build/libs/Client-all.skill`.

In caso non sia presente è possibile utilizzare i comandi dalla `root` del progetto:

```bash
cd Client &&
./gradlew shadowJar
```

#### Compilazione Server

Non è necessaria alcuna compilazione. Tutta la compilazione avviene internamente al docker.

### Avvio

#### Avvio Client

##### Lanciare su SDK

***Una volta lanciato il [Furhat SDK](https://docs.furhat.io/getting_started/)*** sarà sufficiente utilizzare i seguenti comandi dalla cartella `Client` del progetto:

```bash
java -jar Client-all.skill || java -cp java -cp Client-all.skill furhatos.skills.Skill
```

> ***Nota:*** Se nello step di compilazione è stato necessario creare il jar poichè il file non era precedentemente presente, sarà necessario recarsi nella cartella `Client/build/libs` prima di invocare il comando sopracitato

##### Lanciare su un Robot

Per l'avvio della client da un **robot Furhat** si rimanda alla [documentazione ufficilale](https://arc.net/l/quote/vyzeqljh).

#### Avvio Server

Per avviare il server è necessario fornire una `API Key` per Gemini. Per farlo è possibile eseguire il comando:

```bash
GEMINI_API_KEY=YOUR_API_KEY > Server/.env
```

dalla `root` del progetto. Successivamente, sempre dalla `root` del progetto, è sufficiente utilizzare il seguente comando:

```bash
cd Server && docker-compose up --build
```

## Personalizzazione

Sia client che server sono fortemente personalizzabili semplicemente modificando file di configurazione.

### Personalizzazione Client

È possibile modificare settaggi riguardanti la connessione al server e il comportamento di Furhat modificando i valori dei file presenti in `Client/src/main/kotlin/furhatos/app/newskill/setting`.

### Personalizzazione Server

È possibile modificare agilmente le impostazioni del server modificando i falori del file `Server/config.h`.

## Load testing

Essendo particolarmente complesso testare la portata del server esclusivamente mediante interazioni con Furhat sono presenti degli script python nella cartella `Server/load_tests`.
