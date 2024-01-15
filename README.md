# Progetto Finale Linguaggi e Compilatori

##### A.A. 2023/2024
---------

La seguente cartella contiene tutti i file necessari per la produzione di un compilatore di linguaggio **Kaleidoscope**, che include tutte le modifiche previste dalla consegna d'esame (fino a _grammatica di quarto livello_).
Oltre alle opportune (e già note) modifiche alla grammatica effettuate nei file [parser.yy](./parser.yy) e [scanner.ll](./scanner.ll), di seguito sono sommariamente elencate le principali modifiche effettuate alle classi utili alla generazione del codice **IR** per implementare tali nuove funzioni.

## Produzione del codice

#### Grammatica di primo livello

Si introducono nella grammatica i meccanismi di assegnamento
```
<nome_variabile> = <espressione>;
``` 
e di definizione di variabili globali (inizializzate al valore 0.0 per scelta implementativa)
```
global var <nome_variabile>;
```
----------

- definizione delle classi ```GlobalVariableAST``` e ```AssignmentExprAST```
- modifica del metodo _codegen_ della classe ```VariableExprAST``` per gestire lo scope delle variabili globali
- modifica della classe ```BlockExpASt``` in ```BlockAST```, aggiunta la possibilità di avere \<statement> multipli all'interno del blocco

#### Grammatica di secondo livello

Si introducono i costrutti ```if```
```
if (<espressione condizionale>) <statement> [else <statement>]
```
e ```for```
```
for (<initexp>;<espressione condizionale>;<assegnamento>) <statement>
```
--------
- creazione delle classi ```IfStmtAST``` e ```ForStmtAST```
- introduzione della classe virtuale ```InitAST``` da cui ereditano ```VarBindingAST```, ```ArrayBindingAST``` (_quarto livello_), ```AssignmentExprAST```, utile per rappresentare il concetto di dichiarazione e modifica di variabili, che presentano necessità di ridefinire funzioni comuni, in particolare ```GetInitType()``` che restituisce un enum così definito:
    ```c++
    enum initType {
    ASSIGNMENT,
    BINDING,
    INIT
    };
    ```
    utile per gestire l'oscuramento dello scope nel blocco initexp del costrutto _for_ nel caso avvenga un binding di variabile.
- il blocco _if_, per scelta implementativa, crea sempre un branch condizionato a due blocchi _trueBB_ (condizione vera) e _falseBB_ (condizione falsa), anche quando il blocco [else \<statement>] non è presente e quindi la creazione di un blocco falseBB non sarebbe richiesto (in tal caso, conterrà solo il branch al blocco di merge): in questo modo il PHINode deve gestire un fumero fisso di blocchi entranti (due, appunto)
- a livello di parsing (con opportune modifiche alla precedenza degli operatori):
    - introdotti simboli di pre/post incremento nella grammatica
    - introdotta la possibilità di rappresentare numeri negativi

#### Grammatica di terzo livello

Si introducono gli operatori relazionali ```and```,```or``` 
```
<espressione> (and|or) <espressione>
```
e ```not```
```
not <espressione>
```
attraverso una modifica della (già presente) funzione ```codegen()``` della classe ```BinaryExprAST```.

#### Grammatica di quarto livello

Aggiunta la possibilità di operare su strutture dati di tipo vettoriale (**Arrays**).


- dichiarazione:
    ```
    var <nome array> [<n>] \\\dove il campo <n> è necessariamente un intero numerico
    ```
    
- dichiarazione con inizializzazione:
    ```
    var <nome array> [<n>] = {<n_parametri separati da virgola>}
    ```
- accesso ad un elemento:
    ```
    <nome array> [<espressione>]
    ```
------
- creazione della classe ```ArrayBindingAST``` per gestire la dichiarazione di array e l'eventuale inizializzazione
- modifica della classe ```AssignmentExprAST``` per gestire l'assegnamento delle singole celle di array
- modifica della classe ```VariableExprAST``` per l'accesso a singole celle di array



##Esecuzione e compilazione

- Per la creazione del compilatore eseguibile ```kcomp``` e di tutti i file necessari:
    ```shell
    make
    ```
    nella [root directory](./) del progetto.
    >- il Makefile deve essere personalizzato sulla base della configurazione attuale macchina e della sua architettura (quello [già fornito](./Makefile) è configurato per operare su macchina con architettura x86_64, sistema operativo Linux Ubuntu 20.04 LTS e llvm installato tramite repository ufficiale e attualmente in versione 17).
    >- assicurarsi che il file ```kcomp``` venga generato nella root directory

- Per generare gli eseguibili relativi ai vari programmi Kaleidoscope:
    ```shell
    cd test_progetto
    make <nome_programma>
    ```
- Per provare gli eseguibili:
    ```shell
    ./<nome_programma>
    ```
