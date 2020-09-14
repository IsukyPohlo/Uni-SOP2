#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

//https://github.com/IsukyPohlo/uni-SOP2

/*** Definción de la estructura del proceso ***/
typedef struct datos {
    int numProceso; 
    int horaLlegada;
    int rafagaCPU;
    int tiempoVuelta;
    int tiempoEspera;
    int colaListo;
    struct datos *sig;
} PROCESO;

#define numProcesos 20

/*** Declaración de funciones ***/
void crearProcesos(PROCESO **inicio, PROCESO **aux);
void algoritmoFCFS(PROCESO **inicio);
float calcularPromedioTiempoVuelta(PROCESO **inicio);
float calcularPromedioTiempoEspera(PROCESO **inicio);
float calcularTasaSalida(PROCESO **aux);

int main(){
    int i = 0;
    float promedioTiempoEspera, promedioTiempoVuelta, promedioTiempoRespuesta; 
    PROCESO *inicio, *aux;
    inicio = NULL;

    crearProcesos(&inicio, &aux);
    algoritmoFCFS(&inicio);

    //Cálculos
    promedioTiempoVuelta = calcularPromedioTiempoVuelta(&inicio);
    promedioTiempoEspera = calcularPromedioTiempoEspera(&inicio);
    promedioTiempoRespuesta = promedioTiempoEspera; //Para el caso de FCFS el tiempo de respuesya es igual al tiempo de respuesta. 
    
    printf("\nTiempo de vuelta: %f", promedioTiempoVuelta);
    printf("\nTiempo de espera: %f", promedioTiempoEspera);
    printf("\nTiempo de respuesta: %f", promedioTiempoRespuesta);
}

void crearProcesos(PROCESO **inicio, PROCESO **aux) {
    int i, horaLlegada = 0, n = 15, m = 5; 
    PROCESO *proc;
    //Asignación de memoria
    for(i = 1; i <= numProcesos; i++){
        proc = malloc(sizeof(PROCESO));
        if(proc == NULL){
            printf("No hay memoria.");
        }

        //Asignación de ráfaga y tiempo de llegada
        proc->numProceso = i; 
        if(i == 0){
            proc->horaLlegada = horaLlegada;
            horaLlegada = random() % 10;
        } else {
            horaLlegada += random() % 10;
            proc->horaLlegada = horaLlegada;
        }
        proc->rafagaCPU =  random() % (n-m+1) + m; 

        //Creación de lista
        if(*inicio == NULL){
            *inicio = proc;
        } else {
            (*aux)->sig = proc;
        }
        proc->sig = NULL;
        *aux = proc;  printf("Proceso: %d, Hora de llegada: %d , Ráfaga de CPU: %d \n",proc->numProceso, proc->horaLlegada, proc->rafagaCPU);
    }
}

void algoritmoFCFS(PROCESO **inicio) {
    int i = 1;
    PROCESO *proc;
    proc = *inicio; 

    //Sólo es para poder hacer los cálculos gg
    while(proc != NULL){
        proc->tiempoVuelta = i;
        proc->tiempoEspera = i;
        proc = proc->sig;
        i++;
    }    
}

float calcularPromedioTiempoVuelta(PROCESO **inicio) {
    int suma = 0, tiempoVuelta = 0;
    PROCESO *proc;
    proc = *inicio;

    while(proc != NULL){
        tiempoVuelta = proc->tiempoVuelta - proc->horaLlegada;
        suma += tiempoVuelta;
        proc = proc->sig;
    }

    return suma / numProcesos; 
}

float calcularPromedioTiempoEspera(PROCESO **inicio) {
    int suma = 0, tiempoEspera = 0; 
    PROCESO *proc;
    proc = *inicio;

    while(proc != NULL){
        tiempoEspera = proc->tiempoEspera - proc->horaLlegada;
        suma += tiempoEspera;
        proc = proc->sig;
    }

    return suma / numProcesos; 
}

float calcularTasaSalida(PROCESO **aux) {
    return (*aux)->tiempoVuelta / numProcesos;
}