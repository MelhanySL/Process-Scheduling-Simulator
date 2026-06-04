#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include <queue>
#include <list>
#include <string>
#include <random>
#include <algorithm>
#include "process.h"

// Define los Algoritmos de Planificación soportados por el motor de simulación
enum class Algorithm
{
    // Non-preemptive algorithms
    FCFS, // First-Come, First-Served
    SJF, // Shortest Job First
    RANDOM_SELECTION, // Random Selection
    PRIORITY_SCHEDULING, // Priority Scheduling

    // Preemptive algorithms
    RR, // Round Robin (RR)
    SRTF, // Shortest Remaining Time First (SRTF)
    PRIORITY_SCHEDULING_EXPULSIVE // Priority Scheduling (versión expulsiva)
};

class Scheduler
{
private:
    Algorithm active_algorithm; // Algoritmo activo seleccionado para la simulación
    int quantum; // Quantum de tiempo máximo asignado a la CPU (usado en RR)
    int tick_duration_ms; // Duración base en milisegundos de cada paso (tick)

    int global_clock; // Reloj lógico global del sistema (mide el tiempo total en ticks)
    int current_quantum_ticks; // Contador de ticks consumidos por el proceso actual en la CPU

    std::list<Process> all_processes; // Todos los procesos
    std::vector<Process*> ready_queue; // Cola de Listos
    std::queue<Process*> blocked_queue; // Cola de Bloqueados
    std::vector<Process*> terminated_processes; // Procesos completados

    Process* running_process; // Puntero al proceso que se encuentra actualmente en la CPU

    int total_idle_ticks; // Contador -> acumula cuántos ticks de reloj la CPU estuvo completamente ociosa
    std::vector<int> arrivals_per_tick; // Historial paso a paso

    // Métodos internos
    void check_arrivals(); // Verifica si hay nuevos procesos arribando en el tiempo actual
    void update_running_process(); // Administra el ciclo del proceso en ejecución activa
    void check_cpu_completion(); // Evalúa si el proceso en CPU terminó su ráfaga actual
    void check_io_completion(); // Monitorea y despierta a los procesos de la cola de bloqueados
    void select_next_process(); // Despacha el siguiente proceso a CPU aplicando el algoritmo activo
    void sort_ready_queue(); // Ordena la cola de listos bajo criterios de SJF, FCFS, SRTF o Prioridades
    int get_random_ready_index(); // Retorna un índice al azar válido dentro de la cola de listos
public:
    // Constructor
    Scheduler();

    // Destructor
    ~Scheduler() = default;

    // Getters
    Algorithm get_active_algorithm() const;
    int get_quantum() const;
    int get_tick_duration_ms() const;
    int get_global_clock() const;
    int get_current_quantum_ticks() const;
    const std::list<Process>& get_all_processes() const;
    const std::vector<Process*>& get_ready_queue() const;
    const std::queue<Process*>& get_blocked_queue() const;
    const std::vector<Process*>& get_terminated_processes() const;
    Process* get_running_process();

    int get_total_completed() const; // Cantidad de procesos que llegaron al estado TERMINATED
    const std::vector<int>& get_arrivals_per_tick() const; // Historial de ráfagas de llegada
    int get_total_time() const; // Retorna el tiempo total invertido en la simulación en ticks

    // Para la interfaz
    std::string get_algorithm_name() const;

    // Setters
    void set_algorithm(Algorithm algorithm);
    void set_quantum(int quantum_);
    void set_tick_duration_ms(int duration_ms);

    // Estadísticas
    double get_cpu_usage() const; // Porcentaje de uso del procesador
    double get_avg_waiting_time() const; // Tiempo promedio que pasaron los procesos esperando
    double get_avg_block_time() const; // Tiempo promedio que pasaron los procesos en E/S
    double get_avg_execution_time() const; // Tiempo promedio de retorno / respuesta total


    // Métodos
    void add_process(const Process& process); // Registra un nuevo proceso
    void generate_random_processes(int count, int max_cpu, int max_io, int max_priority, int max_arrival); //Genera un lote parametrizado de procesos con valores pseudoaleatorios válidos
    // Para la silumación
    void step(); // Ejecuta un único paso elemental de reloj (Avanza 1 tick y corre todas las rutinas)
    void reset(); // Resetea los procesos y contadores manteniendo la lista base cargada para simular otro algoritmo
    void clear_all(); // Limpia por completo todas las colas e historiales
    void run_until_complete(); // Ejecuta de forma secuencial la simulación hasta vaciar todas las colas
};

#endif // SCHEDULER_H
