
/*** Definción de la estructura del proceso ***/
typedef struct rafaga {
    int tiempo;
    struct rafaga *sig;
} RAFAGA;
typedef struct datos {
    int numProceso; 
    int horaLlegada;
    int horaEntradaSalida;
    int horaCPU;
    int fin;
    int tuvoCPU;
    RAFAGA *rafActual;
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