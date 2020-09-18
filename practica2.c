#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "types.h"
#include <windows.h>

//https://github.com/IsukyPohlo/uni-SOP2

#define NUMPROCESOS 20
#define MINRAFAGAS 5
#define MAXRAFAGAS 10
#define RAFAGACPUMIN 2
#define RAFAGACPUMAX 10
#define RAFAGAESMIN 10
#define RAFAGAESMAX 20

/*** Declaración de funciones ***/
PROCESO *crearProcesos(int numProcesos);
PROCESO *crearProceso(int id);
RAFAGA *crearRafagas(int num);
RAFAGA *crearRafaga(tipoDeRafaga tipo);
void imprimirLista(void *head, tipoDeNodo tipo, int extra, int hora);
void bubbleSort(PROCESO *start);
void swap(PROCESO *a, PROCESO *b);
int checarListaTermino(PROCESO *inicio);
int checarProcesoTermino(PROCESO *proceso);
PROCESO *push(PROCESO *head, PROCESO *proceso);
int enLista(PROCESO *lista, PROCESO *elemento);
PROCESO *getProcesoEnTiempoLlegada(PROCESO *inicio, int tiempo, PROCESO *cola);
PROCESO *copiarProceso(PROCESO *proceso);
void *pull(void **head, tipoDeNodo tipo);
PROCESO *getProcesoEnFinRafaga(PROCESO **inicio, int tiempo, PROCESO *cola);
void imprimirListaFile(void *head, tipoDeNodo tipo, int extra, FILE *fp);
int longitudLista(void *head, tipoDeNodo tipo);
void finalizarProceso(int num, PROCESO *head);

int main(){
    FILE *fp;
    fp = fopen( "procesos.txt", "w+");
    srand(time(0));
    int tiempo = 0, tiempoVuelta=0, tiempoEspera=0, tiempoRespuesta=0; 
    PROCESO *inicio=NULL, *cola=NULL;
    inicio = crearProcesos(NUMPROCESOS);
    bubbleSort(inicio);
    imprimirListaFile(inicio, TIPO_PROCESO, 1, fp);
    printf("Procesos ordenados:\n");
    imprimirLista(inicio, TIPO_PROCESO, 0, 1);
    char ch = 0;
    do {
        printf("Escribe I para iniciar ");
        scanf(" %c", &ch);
    } while (ch != 'I');
    fclose(fp);
    PROCESO *procesoEnCPU = NULL;
    PROCESO *entradaSalida = NULL;
    int finalRafaga = 0;
    while(!checarListaTermino(inicio)) {
        system("cls");
        printf("Tiempo: %d\n", tiempo);
        // Pone en la cola los procesos que llegan
        while(1) {
            PROCESO *temp = copiarProceso(getProcesoEnTiempoLlegada(inicio, tiempo, cola));
            if(temp == NULL) {
                // printf("Break");
                break;
            }
            // printf("Llegada %d, ID: %d", temp->horaLlegada, temp->numProceso);
            cola = push(cola, temp);
        }
        // printf("Push a cola llegada");
        // Pone en la cola los procesos que terminan su rafaga de Entrada/Salida
        if(entradaSalida != NULL) {
            while(1) {
                PROCESO *temp = getProcesoEnFinRafaga(&entradaSalida, tiempo, cola);
                // printf("Llegamos");
                if(temp == NULL) {
                    // printf("Break");
                    break;
                }
                // printf("Llegada %d, ID: %d", temp->horaLlegada, temp->numProceso);
                cola = push(cola, temp);
            }
        }
        // printf("Push a cola fin E/S");

        // printf("\n\nCola\n");
        // imprimirLista(cola, TIPO_PROCESO, 0);
        // char ch;
        // scanf(" %c", &ch);

        // Pone en entrada y salida los procesos que acaban CPU
        if(procesoEnCPU != NULL) {
            if(procesoEnCPU->horaCPU + procesoEnCPU->rafActual->tiempo == tiempo) {
                if(procesoEnCPU->rafagas != NULL) {
                    procesoEnCPU->horaEntradaSalida = tiempo;
                    procesoEnCPU->rafActual = (RAFAGA *)pull((void **)&procesoEnCPU->rafagas, TIPO_RAFAGA);
                    entradaSalida = push(entradaSalida, procesoEnCPU);
                } else {
                    tiempoVuelta += (tiempo - procesoEnCPU->horaLlegada);
                    finalizarProceso(procesoEnCPU->numProceso, inicio);
                }
                procesoEnCPU = NULL;
            }
        }
        // printf("Push a E/S");
        //Si el CPU esta libre asigna al siguiente de la cola
        if(procesoEnCPU == NULL && cola != NULL) {
            procesoEnCPU = (PROCESO *)pull((void **)&cola, TIPO_PROCESO);
            procesoEnCPU->horaCPU = tiempo;
            procesoEnCPU->rafActual = (RAFAGA *)pull((void **)&procesoEnCPU->rafagas, TIPO_RAFAGA);
            if(!procesoEnCPU->tuvoCPU) {
                procesoEnCPU->tuvoCPU = 1;
                tiempoRespuesta += tiempo - procesoEnCPU->horaLlegada;
            }
        }
        // printf("Asigna CPU");
        //Imprime cosas
        procesoEnCPU != NULL ? printf("\n\tProceso en CPU: %d Tiempo Llegada: %d Final Rafaga: %d", procesoEnCPU->numProceso, procesoEnCPU->horaLlegada, (procesoEnCPU->rafActual->tiempo+procesoEnCPU->horaCPU)) : printf("Proceso en CPU: ----");
        printf("\n\tCola de Listos\n");
        imprimirLista(cola, TIPO_PROCESO, 0, 0);
        printf("\n\tEntradas y Salidas\n");
        imprimirLista(entradaSalida, TIPO_PROCESO, 0, 0);
        tiempoEspera += longitudLista(cola, TIPO_PROCESO);
        usleep(250*1000);
        tiempo++;
    }
    float promedioVuelta = (float)tiempoVuelta/(float)NUMPROCESOS;
    float promedioEspera = (float)tiempoEspera/(float)NUMPROCESOS;
    float promedioRespuesta = (float)tiempoRespuesta/(float)NUMPROCESOS;
    float tasaSalida = (float)(tiempo-1)/(float)NUMPROCESOS;
    printf("Estadistica final:\n\tPromedio de Vuelta: %f\n\tPromedio de Espera: %f\n\tPromedio de Respuesta: %f\n\tTasa de Salida: %f\n\t", promedioVuelta, promedioEspera, promedioRespuesta,  tasaSalida);
}

void finalizarProceso(int num, PROCESO *head) {
    PROCESO *cur = head;
    while(cur != NULL) {
        if(cur->numProceso == num) cur->fin = 1;
        cur = cur->sig;
    }
}

int longitudLista(void *head, tipoDeNodo tipo) {
    int counter = 0;
    if(tipo == TIPO_PROCESO) {
        PROCESO *cur = (PROCESO *) head;
        while(cur != NULL) {
            counter ++;
            cur = cur->sig;
        }
    } else {
        RAFAGA *cur = (RAFAGA *) head;
        while(cur != NULL) {
            counter ++;
            cur = cur->sig;
        }
    }
    return counter;
}

void *pull(void **head, tipoDeNodo tipo) {
    if(tipo == TIPO_PROCESO) {
        PROCESO *tmp = (PROCESO *)*head;
        *head = tmp->sig;
        tmp->sig = NULL;
        return tmp;
    } else {
        RAFAGA *tmp = (RAFAGA *)*head;
        *head = tmp->sig;
        tmp->sig = NULL;
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
    proceso->fin = 0;
    proceso->tuvoCPU = 0;
    proceso->numProceso = id;
    proceso->horaLlegada = (rand() % (NUMPROCESOS - 0 + 1)) + 0;
    int numRafagasES = (rand() % (MAXRAFAGAS - MINRAFAGAS + 1)) + MINRAFAGAS;
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
    temp->horaCPU = 0;
    temp->horaEntradaSalida = 0;
    temp->sig = NULL;
    temp->fin = proceso->fin;
    temp->tuvoCPU = proceso->tuvoCPU;
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
    int min = tipo == CPU ? RAFAGACPUMIN : RAFAGAESMIN;
    int max = tipo == CPU ? RAFAGACPUMAX : RAFAGAESMAX;
    RAFAGA *rafaga = malloc(sizeof(RAFAGA));
    if(rafaga == NULL ) {
        printf("No hay espacio");
        exit(1);
    }
    rafaga->sig = NULL;
    rafaga->tiempo = (rand() % (max - min + 1)) + min;
    return rafaga;
}

void imprimirLista(void *head, tipoDeNodo tipo, int extra, int hora) {
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
            if(hora) {
                printf("Proceso numero: %d\tHora de llegada: %d\n", nodo->numProceso, nodo->horaLlegada);
            } else {
                printf("Proceso numero: %d\n", nodo->numProceso);
            }
            if(extra) imprimirLista(nodo->rafagas, TIPO_RAFAGA, 0, 0);
            nodo = nodo->sig;
        }
    }
}

void imprimirListaFile(void *head, tipoDeNodo tipo, int extra, FILE *fp) {
    void *nodo = NULL;
    if(tipo == TIPO_RAFAGA) {
        RAFAGA *nodo = (RAFAGA *)head;
        while(nodo != NULL) {
            fprintf(fp, "Tiempo: %d\n", nodo->tiempo);
            nodo = nodo->sig;
        }
    } else {
        PROCESO *nodo = (PROCESO *)head;
        while(nodo != NULL) {
            fprintf(fp, "Numero: %d\tHora de llegada: %d\n", nodo->numProceso, nodo->horaLlegada);
            if(extra) imprimirListaFile(nodo->rafagas, TIPO_RAFAGA, 0, fp);
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
    // printf("Proceso: %d, Num Rafagas: %d\n", proceso->numProceso, longitudLista(proceso->rafagas, TIPO_RAFAGA));
    if(proceso->fin == 1) {
        return 1;
    }
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

PROCESO *getProcesoEnFinRafaga(PROCESO **inicio, int tiempo, PROCESO *cola) {
    PROCESO *temp = *inicio, *prev;
    prev = temp;
    if(temp != NULL && (temp->horaEntradaSalida+temp->rafActual->tiempo) == tiempo && !enLista(cola, temp)) {
        *inicio = temp->sig;
        temp->sig = NULL;
        return temp;
    }
    while (temp != NULL) {
        if((temp->horaEntradaSalida+temp->rafActual->tiempo) == tiempo && !enLista(cola, temp)) {
            prev->sig = temp->sig;
            temp->sig = NULL;
            return temp;
        }
        prev = temp;
        temp = temp->sig;
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