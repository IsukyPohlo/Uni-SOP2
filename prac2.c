#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

/*** Definción de la estructura del proceso ***/
typedef struct datos {
    int horaLlegada;
    int rafagaCPU;
    struct datos *sig;
} PROCESO;

#define numProcesos 20

/*** Declaración de funciones ***/
void crearProcesos(PROCESO **inicio, PROCESO **aux);
void algoritmoFCFS();
float calcularPromedioTiempoVuelta();
float calcularPromedioTiempoEspera();
float calcularPromedioTiempoRespuesta();
float calcularTasaSalida();

int main(){
    int i = 0;
    PROCESO *inicio, *aux;
    inicio = NULL;

    crearProcesos(&inicio, &aux);
    
}

void crearProcesos(PROCESO **inicio, PROCESO **aux) {
    int i;
    PROCESO *proc;
    //Asignación de memoria
    for(i = 0; i <= numProcesos; i++){
        proc = malloc(sizeof(PROCESO));
        if(proc == NULL){
            printf("No hay memoria.");
        }
        proc->horaLlegada = i;
        proc->rafagaCPU = i; 

        //Creación de lista
        if(*inicio == NULL){
            *inicio = proc;
        } else {
            (*aux)->sig = proc;
        }
        proc->sig = NULL;
        *aux = proc;  printf("Hora de llegada: %d \n",proc->horaLlegada);
    }
}

void algoritmoFCFS() {

}

float calcularPromedioTiempoVuelta() {

}

float calcularPromedioTiempoEspera() {

}

float calcularPromedioTiempoRespuesta() {

}

float calcularTasaSalida() {

}