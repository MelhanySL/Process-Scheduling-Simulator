#ifndef PROCESS_H
#define PROCESS_H

// Representa el Ciclo de Vida del Proceso dentro del Simulador
enum class State
{
    NEW, // Nuevo
    READY, // Listo
    RUNNING, // En ejcución
    BLOCKED, // Bloqueado
    TERMINATED // Terminado
};

class Process
{
private:
    int id; // Identificador único del proceso
    int priority; // Nivel de prioridad (utilizado en algoritmos de prioridad)
    int arrival_time; // Instante de tiempo (tick) en el que el proceso entra al sistema
    int cpu_burst_time; // Duración total original de la ráfaga de CPU requerida
    int io_burst_time; // Duración total original de la ráfaga de Entrada/Salida (E/S)

    State current_state; // Estado actual del proceso en el sistema

    int remaining_cpu_time; // Tiempo restante de CPU -> Ticks restantes que el proceso necesita ejecutar en la CPU
    int remaining_io_time; // Tiempo restante de E/S -> Ticks restantes que el proceso debe permanecer bloqueado en E/S

    int waiting_time; // Tiempo de espera -> Tiempo total acumulado esperando en la cola de Listos (Ready)
    int block_time; // Tiempo de bloqueo -> Tiempo total acumulado en estado Bloqueado (I/O)
    int ending_time; // Tiempo de finalización -> Instante de tiempo en el que el proceso pasa a TERMINATED

public:
    //Constructor
    Process(int id, int priority, int arrival, int cpu_burst, int io_burst);

    //Getters
    int get_id() const;
    int get_priority() const;
    int get_arrival_time() const;
    int get_cpu_burst() const;
    int get_io_burst() const;
    State get_current_state() const;
    int get_remaining_cpu_time() const;
    int get_remaining_io_time() const;
    int get_waiting_time() const;
    int get_block_time() const;
    int get_ending_time() const;

    //Setters
    void set_state(State new_state);
    void set_priority(int new_priority);
    void set_cpu_burst(int cpu_burst);
    void set_io_burst(int io_burst);

    //Métodos
    void update_tick(); // Avanza el estado interno del proceso en 1 tick, acumulando esperas o decrementando ráfagas según su estado actual
    bool is_finished() const; // Evalúa si el proceso cumplió con toda su carga de trabajo tanto de CPU como de operaciones de E/S
    bool is_io_finished() const; // Retorna verdadero si el proceso ha completado su tiempo requerido en la cola de bloqueados
    void finish_process(int current_time); // Registra de forma definitiva el tiempo de finalización del proceso y cambia su estado a TERMINATED
    void reset_process(); // Restablece los contadores y tiempos restantes a su estado original
};

#endif // PROCESS_H
