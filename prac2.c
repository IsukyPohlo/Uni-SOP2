#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
//#include "types.h"

/*** Definción de la estructura del proceso ***/
typedef struct rafaga {
    int tiempo;
    struct rafaga *sig;
} RAFAGA;
typedef struct datos {
    int numProceso; 
    int horaLlegada;
    RAFAGA *rafagas;
    struct datos *sig;
} PROCESO;
typedef enum {
  CPU,
  ES
} tipoDeRafaga;
typedef enum {
  TIPO_PROCESO,
  TIPO_RAFAGA
} tipoDeNodo;

#define NUMPROCESOS 5

/*** Declaración de funciones ***/
PROCESO *crearProcesos(int numProcesos);
PROCESO *crearProceso(int id);
RAFAGA *crearRafagas(int num);
RAFAGA *crearRafaga(tipoDeRafaga tipo);
PROCESO *push(PROCESO *head, PROCESO *proceso);
PROCESO *getProcesoEnTiempoLlegada(PROCESO *inicio, int tiempo, PROCESO *cola);
PROCESO *copiarProceso(PROCESO *proceso);
int checarListaTermino(PROCESO *inicio);
int checarProcesoTermino(PROCESO *proceso);
int enLista(PROCESO *lista, PROCESO *elemento);
void *pull(void **head, tipoDeNodo tipo);
void imprimirLista(void *head, tipoDeNodo tipo, int extra);
void bubbleSort(PROCESO *start);
void swap(PROCESO *a, PROCESO *b);

int main(){
    srand(time(0));
    int tiempoCPU = 0,  tiempoVueltaCPU=0, tiempoEsperaCPU = 0, finalRafagaCPU=0; 
    int tiempoES = 0, tiempoVueltaES = 0, tiempoEsperaES = 0, finalRafagaES = 0;
    PROCESO *inicio=NULL, *colaListoCPU=NULL, *colaListoES = NULL, *procesoEnCPU = NULL, *procesoEnES = NULL, *auxProceso = NULL;

    inicio = crearProcesos(NUMPROCESOS); //Se crean los 20 procesos con su tiempo de llegada y ráfagas de CPU y E/S
    bubbleSort(inicio); //Ordena los procesos por tiempo de llegada
    imprimirLista(inicio, TIPO_PROCESO, 1);

    while(!checarListaTermino(inicio)) { //Verficar si todos los procesos han sido atendidos y han terminado
        while(1) {
            printf("Tiempo: %d\n", tiempoCPU);
            PROCESO *temp = copiarProceso(getProcesoEnTiempoLlegada(inicio, tiempoCPU, colaListoCPU)); //Se realiza una copia del proceso que ha llegado  
            if(temp == NULL) { //Si no hay proceso en el tiempo actual 
                printf("Break");
                break;
            }  
            printf("Llegada %d, ID: %d", temp->horaLlegada, temp->numProceso);
            colaListoCPU = push(colaListoCPU, temp); //Se añade a la cola de listo el proceso que ha llegado
        }
        printf("\n\nCola \n"); imprimirLista(colaListoCPU, TIPO_PROCESO, 0);
        char ch;
        scanf(" %c", &ch);
        if(procesoEnCPU == NULL && colaListoCPU != NULL) { //Si el CPU está libre, le asigna el CPU
            tiempoEsperaCPU += tiempoCPU;
            procesoEnCPU = (PROCESO *)pull((void **)&colaListoCPU, TIPO_PROCESO); //Lo quita de la cola de listos
            finalRafagaCPU = tiempoCPU + procesoEnCPU->rafagas->tiempo; //Obtener el tiempo en que termina la ráfaga del proceso que se está ejecutando
        }
        if(finalRafagaCPU == tiempoCPU && procesoEnCPU != NULL) { //En caso de que haya terminado su ráfaga de CPU     
            tiempoVueltaCPU += finalRafagaCPU;
            printf("Moviendo %d a ES", procesoEnCPU->numProceso); //Mueve el procesos a la cola de listos de E/S
            colaListoES = push(colaListoES, procesoEnCPU); //Mueve el proceso a la cola de listos de E/S
            procesoEnCPU = NULL; //Libera el CPU
            pull((void**)&colaListoES->rafagas, TIPO_RAFAGA); //Elimina la ráfaga de CPU
            /***** Hacer pull de la ráfaga de CPU en lista de procesos ******/
            printf("\n\nRAFAGAS ES\n"); imprimirLista(colaListoES, TIPO_RAFAGA, 0);
            if(procesoEnES == NULL && colaListoES != NULL) { //Si está libre la E/S, le asigna
                tiempoEsperaES += tiempoES;
                procesoEnES = (PROCESO *)pull((void **)&colaListoES, TIPO_PROCESO); //Lo quita de la cola de E/S
                finalRafagaES = tiempoES + procesoEnES->rafagas->tiempo; //Obtener el tiempo en que termina la ráfaga del proceso que se está ejecutando       
            }
            if(finalRafagaES == tiempoES && procesoEnES != NULL){
                tiempoVueltaES += finalRafagaES;
                printf("Ha terminado en E/S");  
                pull((void**)&colaListoES->rafagas, TIPO_RAFAGA); //Elimina la ráfaga de E/S
                if(colaListoES->rafagas->sig != NULL){
                    printf("Se ha encolado a la lista de CPU");
                    colaListoCPU = push(colaListoCPU, procesoEnES);//push a la cola de listos
                }
            }
        }
        printf("\n\nCola \n"); imprimirLista(colaListoCPU, TIPO_PROCESO, 0);
        if(procesoEnCPU != NULL) printf("\n\nProceso en CPU: %d Tiempo Llegada: %d Final Rafaga: %d\n", procesoEnCPU->numProceso, procesoEnCPU->horaLlegada, finalRafagaCPU);
        tiempoCPU++;
        tiempoES++;
    }

    if(checarListaTermino(inicio)) {
        //Cálculos
        printf("----------- C P U --------------");
        printf("La Tasa de Salida de CPU es de: %f", finalRafagaCPU / NUMPROCESOS);  
        printf("El Tiempo promedio de Espera de CPU es de: %f", tiempoEsperaCPU/ NUMPROCESOS);
        printf("El Tiempo promedio de Vuelta de CPU es de: %f", tiempoVueltaCPU / NUMPROCESOS);
        printf("El Tiempo promedio de Respuesta de CPU es de: %f", tiempoEsperaCPU / NUMPROCESOS);

        printf("----------- E / S --------------");
        printf("La Tasa de Salida de E/S es de: %f", finalRafagaES / NUMPROCESOS);  
        printf("El Tiempo promedio de Espera de E/S es de: %f", tiempoEsperaES / NUMPROCESOS);
        printf("El Tiempo promedio de Vuelta de E/S es de: %f", tiempoVueltaES / NUMPROCESOS);
        printf("El Tiempo promedio de Respuesta de E/S es de: %f", tiempoEsperaES / NUMPROCESOS);
    }

}

void *pull(void **head, tipoDeNodo tipo) {
    if(tipo == TIPO_PROCESO) {
        PROCESO *tmp = (PROCESO *)*head;
        *head = tmp->sig;
        return tmp;
    } else {
        RAFAGA *tmp = (RAFAGA *)*head;
        *head = tmp->sig;
        //return tmp;
    }
}

PROCESO *push(PROCESO *head, PROCESO *proceso) {
    PROCESO *cur=head;
    if(head == NULL) {
        head = proceso;
    } else {
        while(cur->sig != NULL) {
            cur=cur->sig;
        }
        cur->sig = proceso;
    }
    return head;
}

PROCESO *crearProcesos(int numProcesos) {
    PROCESO *head=NULL, *proceso, *prev;
    for(int i = 0; i < numProcesos; i++) {
        proceso = crearProceso(i+1);
        head = push(head, proceso);
    }
    return head;
}


PROCESO *crearProceso(int id) {
    PROCESO *proceso = malloc(sizeof(PROCESO));
    if(proceso == NULL ) {
        printf("No hay espacio");
        exit(1);
    }
    proceso->sig = NULL;
    proceso->numProceso = id;
    proceso->horaLlegada = (rand() % (20 - 0 + 1)) + 0;
    int numRafagasES = (rand() % (10 - 5 + 1)) + 5;
    int numRafagasCPU = numRafagasES+1;
    int numRafagasTotales = numRafagasES+numRafagasCPU;
    proceso->rafagas = crearRafagas(numRafagasTotales);
    return proceso;
}

PROCESO *copiarProceso(PROCESO *proceso) {
    if(proceso == NULL) return NULL;
    PROCESO *temp = malloc(sizeof(PROCESO));
    if(temp == NULL ) {
        printf("No hay espacio");
        exit(1);
    }
    temp->numProceso = proceso->numProceso;
    temp->horaLlegada = proceso->horaLlegada;
    temp->rafagas = proceso->rafagas;
    temp->sig = NULL;
    return temp;
}

RAFAGA *crearRafagas(int num) {
    RAFAGA *head=NULL, *rafaga, *prev;
    tipoDeRafaga tipo;
    for(int i = 0; i < num; i++) {
        if(i % 2 == 0) {
            tipo = CPU;
        } else {
            tipo = ES;
        }
        rafaga = crearRafaga(tipo);
        if(i == 0) {
            head = rafaga;
        } else {
            prev->sig = rafaga;
        }
        prev = rafaga;
    }
    return head;
}

RAFAGA *crearRafaga(tipoDeRafaga tipo) {
    int min = tipo == CPU ? 2 : 10;
    int max = tipo == CPU ? 10 : 20;
    RAFAGA *rafaga = malloc(sizeof(RAFAGA));
    if(rafaga == NULL ) {
        printf("No hay espacio");
        exit(1);
    }
    rafaga->sig = NULL;
    rafaga->tiempo = (rand() % (max - min + 1)) + min;
    return rafaga;
}

void imprimirLista(void *head, tipoDeNodo tipo, int extra) {
    void *nodo = NULL;
    if(tipo == TIPO_RAFAGA) {
        RAFAGA *nodo = (RAFAGA *)head;
        while(nodo != NULL) {
            printf("Tiempo: %d\n", nodo->tiempo);
            nodo = nodo->sig;
        }
    } else {
        PROCESO *nodo = (PROCESO *)head;
        while(nodo != NULL) {
            printf("Numero: %d\tHora de llegada: %d\n", nodo->numProceso, nodo->horaLlegada);
            if(extra) imprimirLista(nodo->rafagas, TIPO_RAFAGA, 0);
            nodo = nodo->sig;
        }
    }
}

void bubbleSort(PROCESO *start) { 
    int swapped, i; 
    PROCESO *ptr1; 
    PROCESO *lptr = NULL; 
  
    //Checar que la lista esté vacía
    if (start == NULL)  return; 

    do{ 
        swapped = 0; 
        ptr1 = start; 
        while (ptr1->sig != lptr) { 
            if (ptr1->horaLlegada > ptr1->sig->horaLlegada) {  
                swap(ptr1, ptr1->sig); 
                swapped = 1; 
            } 
            ptr1 = ptr1->sig; 
        } 
        lptr = ptr1; 
    } 
    while (swapped); 
} 

void swap(PROCESO *a, PROCESO *b) { 
    int numProcesoTemp = a->numProceso; 
    int horaLlegadaTemp = a->horaLlegada;
    RAFAGA *rafagasTemp = a->rafagas;
    a->numProceso = b->numProceso; 
    a->horaLlegada = b->horaLlegada;
    a->rafagas = b->rafagas;
    b->numProceso = numProcesoTemp;
    b->horaLlegada = horaLlegadaTemp;
    b->rafagas = rafagasTemp;
} 

int checarProcesoTermino(PROCESO *proceso) {
    if(proceso->rafagas == NULL) return 1;
    return 0;
}
int checarListaTermino(PROCESO *inicio) {
    PROCESO *nodo = inicio;
    while(nodo != NULL) {
        if(!checarProcesoTermino(nodo)) return 0;
        nodo=nodo->sig;
    }
    return 1;
}
PROCESO *getProcesoEnTiempoLlegada(PROCESO *inicio, int tiempo, PROCESO *cola) {
    PROCESO *cur = inicio;
    while(cur != NULL) {
        if(tiempo == cur->horaLlegada && !enLista(cola, cur)) return cur;
        cur = cur->sig;
    }
    return NULL;
}

int enLista(PROCESO *lista, PROCESO *elemento) {
    PROCESO *cur = lista;
    while(cur != NULL) {
        if(cur->numProceso == elemento->numProceso) return 1;
        cur = cur->sig;
    }
    return 0;
}

