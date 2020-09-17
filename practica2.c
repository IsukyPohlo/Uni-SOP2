#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "types.h"

//https://github.com/IsukyPohlo/uni-SOP2

#define NUMPROCESOS 20

/*** Declaración de funciones ***/
PROCESO *crearProcesos(int numProcesos);
PROCESO *crearProceso(int id);
RAFAGA *crearRafagas(int num);
RAFAGA *crearRafaga(tipoDeRafaga tipo);
void imprimirLista(void *head, tipoDeNodo tipo, int extra);
void bubbleSort(PROCESO *start);
void swap(PROCESO *a, PROCESO *b);
int checarListaTermino(PROCESO *inicio);
int checarProcesoTermino(PROCESO *proceso);
PROCESO *push(PROCESO *head, PROCESO *proceso);
int enLista(PROCESO *lista, PROCESO *elemento);
PROCESO *getProcesoEnTiempoLlegada(PROCESO *inicio, int tiempo, PROCESO *cola);
PROCESO *copiarProceso(PROCESO *proceso);
void *pull(void **head, tipoDeNodo tipo);

int main(){
    srand(time(0));
    int tiempo = 0, tiempoVuelta=0, tiempoEspera=0; 
    PROCESO *inicio=NULL, *cola=NULL;
    inicio = crearProcesos(NUMPROCESOS);
    bubbleSort(inicio);
    imprimirLista(inicio, TIPO_PROCESO, 1);
    PROCESO *procesoEnCPU = NULL;
    int finalRafaga = 0;
    while(!checarListaTermino(inicio)) {
        while(1) {
            printf("Tiempo: %d\n", tiempo);
            PROCESO *temp = copiarProceso(getProcesoEnTiempoLlegada(inicio, tiempo, cola));
            if(temp == NULL) {
                printf("Break");
                break;
            }
            printf("Llegada %d, ID: %d", temp->horaLlegada, temp->numProceso);
            cola = push(cola, temp);
        }
        printf("\n\nCola\n");
        imprimirLista(cola, TIPO_PROCESO, 0);
        char ch;
        scanf(" %c", &ch);
        if(procesoEnCPU == NULL && cola != NULL) {
            procesoEnCPU = (PROCESO *)pull((void **)&cola, TIPO_PROCESO);
            RAFAGA *rafActual = (RAFAGA *)pull((void **)&procesoEnCPU->rafagas, TIPO_RAFAGA);
            finalRafaga = tiempo + rafActual->tiempo;
        }
        if(finalRafaga == tiempo && procesoEnCPU != NULL) {
            printf("Moviendo %d a ES", procesoEnCPU->numProceso);
            procesoEnCPU = NULL;
        }
        if(procesoEnCPU == NULL && cola != NULL) {
            procesoEnCPU = (PROCESO *)pull((void **)&cola, TIPO_PROCESO);
            RAFAGA *rafActual = (RAFAGA *)pull((void **)&procesoEnCPU->rafagas, TIPO_RAFAGA);
            finalRafaga = tiempo + rafActual->tiempo;
        }
        printf("\n\nCola\n");
        imprimirLista(cola, TIPO_PROCESO, 0);
        if(procesoEnCPU != NULL) printf("\n\nProceso en CPU: %d Tiempo Llegada: %d Final Rafaga: %d\n", procesoEnCPU->numProceso, procesoEnCPU->horaLlegada, finalRafaga);
        tiempo++;
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
        return tmp;
    }
}

PROCESO *crearProcesos(int numProcesos) {
    PROCESO *head=NULL, *proceso, *prev;
    for(int i = 0; i < numProcesos; i++) {
        proceso = crearProceso(i+1);
        head = push(head, proceso);
    }
    return head;
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
  
    /* Checking for empty list */
    if (start == NULL) 
        return; 
  
    do
    { 
        swapped = 0; 
        ptr1 = start; 
  
        while (ptr1->sig != lptr) 
        { 
            if (ptr1->horaLlegada > ptr1->sig->horaLlegada) 
            {  
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
    int colaListo;
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