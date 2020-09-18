==========================
# Practica 2 - Sistemas Operativos Otoño 2020
* Libni Chamán Gaytán
* Simon Green Feldman
* Irving Leopoldo López Sánchez
==========================
## PRACTICA 2
El programa realizado en la practica 2 simula una cola de planificación de tipo FCFS (First Come First Serve).
El programa crea 20 procesos con todos sus datos aleatorios, los encola, les da CPU según, ejecuta las ráfagas tanto de CPU como de Entradas y Salidas hasta que todos los procesos terminan de realizar todas sus ráfagas.
Al final calcula y despliega el promedio de tiempo de vuelta, el promedio de tiempo de espera, el promedio del tiempo de respuesta y la taza de salida.
### Para compilar:
Para compilar el programa es neceario correr el comando:
    ```
    gcc -o practica2 -practica2.c
    ```  
### Para correr el programa:
Para correr el programa se utiliza el comando:
    `./practica2`
Posteriormente se desplegarán los procesos y sus horas de llegada, se creará el archivo `procesos.txt` en le que se pueden ver los procesos, su hora de llegada y sus ráfagas.
Para continuar es necesario escribir la letra `I` y presionar `Enter`.