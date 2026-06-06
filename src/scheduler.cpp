#include "headers/scheduler.h"

// Constructor
Scheduler::Scheduler()
    : active_algorithm(Algorithm::FCFS), quantum(3), tick_duration_ms(1000), global_clock(0), current_quantum_ticks(0),
    running_process(nullptr), total_idle_ticks(0)
{

}
// Getters
Algorithm Scheduler::get_active_algorithm() const
{
    return active_algorithm;
}
int Scheduler::get_quantum() const
{
    return quantum;
}
int Scheduler::get_tick_duration_ms() const
{
    return tick_duration_ms;
}
int Scheduler::get_global_clock() const
{
    return global_clock;
}
int Scheduler::get_current_quantum_ticks() const
{
    return current_quantum_ticks;
}
const std::list<Process>& Scheduler::get_all_processes() const
{
    return all_processes;
}
const std::vector<Process*>& Scheduler::get_ready_queue() const
{
    return ready_queue;
}
const std::queue<Process*>& Scheduler::get_blocked_queue() const
{
    return blocked_queue;
}
const std::vector<Process*>& Scheduler::get_terminated_processes() const
{
    return terminated_processes;
}
// Retorna el nombre legible del algoritmo para la interfaz gráfica
std::string Scheduler::get_algorithm_name() const
{
    std::string name = "";

    switch (active_algorithm)
    {
    case Algorithm::FCFS:
        name = "Primero en llegar primero en ejecutar (FCFS)";
        break;
    case Algorithm::SJF:
        name = "Primero el trabajo más corto (SJF)";
        break;
    case Algorithm::RANDOM_SELECTION:
        name = "Selección aleatoria";
        break;
    case Algorithm::PRIORITY_SCHEDULING:
        name = "Planificación basada en prioridades";
        break;
    case Algorithm::RR:
        name = "Turno rotativo (round robin)";
        break;
    case Algorithm::SRTF:
        name = "Primero el menor tiempo restante (SRTF)";
        break;
    case Algorithm::PRIORITY_SCHEDULING_EXPULSIVE:
        name = "Planificación basada en prioridades (versión expulsiva)";
        break;
    default:
        break;
    }
    return name;
}

int Scheduler::get_total_completed() const
{
    return terminated_processes.size();
}
int Scheduler::get_total_time() const
{
    return global_clock;
}
const std::vector<int>& Scheduler::get_arrivals_per_tick() const
{
    return arrivals_per_tick;
}

Process* Scheduler::get_running_process()
{
    return running_process;
}

// Setters
void Scheduler::set_algorithm(Algorithm algorithm)
{
    active_algorithm = algorithm;
}
void Scheduler::set_quantum(int quantum_)
{
    quantum = quantum_;
}
void Scheduler::set_tick_duration_ms(int duration_ms)
{
    tick_duration_ms = duration_ms;
}


// Métodos Internos
// Verifica si hay nuevos procesos arribando en el tiempo actual
void Scheduler::check_arrivals()
{
    if(arrivals_per_tick.size() <= static_cast<size_t>(global_clock))
    {
        arrivals_per_tick.resize(global_clock + 1, 0);
    }

    for(auto& process : all_processes)
    {
        if(global_clock == process.get_arrival_time() && process.get_current_state() == State::NEW)
        {
            process.set_state(State::READY);
            ready_queue.emplace_back(&process);
            arrivals_per_tick[global_clock]++;
        }
    }
}
// Administra el ciclo del proceso en ejecución activa
void Scheduler::update_running_process()
{
    if(running_process != nullptr)
    {
        running_process->update_tick();

    }
    else if(ready_queue.empty())
    {
        total_idle_ticks++;
    }

    if(!ready_queue.empty())
    {
        for(const auto& ready : ready_queue)
        {
            if(ready != nullptr)
            {
                ready->update_tick();
            }
        }
    }

    if(!blocked_queue.empty())
    {
        std::queue<Process*> new_blocked_queue;
        while(!blocked_queue.empty())
        {
            Process* blocked = blocked_queue.front();
            blocked->update_tick();
            new_blocked_queue.push(blocked);
            blocked_queue.pop();
        }
        blocked_queue = new_blocked_queue;
    }

}
// Evalúa si el proceso en CPU terminó su ráfaga actual
void Scheduler::check_cpu_completion()
{
    if(running_process == nullptr)
    {
        return;
    }

    if(active_algorithm == Algorithm::RR)
    {
        current_quantum_ticks++;
    }

    if(running_process->get_remaining_cpu_time() == 0)
    {
        if(running_process->get_remaining_io_time() > 0)
        {
            running_process->set_state(State::BLOCKED);
            blocked_queue.push(running_process);
        }
        else
        {
            running_process->finish_process(global_clock);
            running_process->set_state(State::TERMINATED);
            terminated_processes.push_back(running_process);
        }
        running_process = nullptr;
        current_quantum_ticks = 0;
    }
    else if(active_algorithm == Algorithm::RR && current_quantum_ticks >= quantum)
    {
        running_process->set_state(State::READY);
        ready_queue.push_back(running_process);
        running_process = nullptr;
        current_quantum_ticks = 0;

        sort_ready_queue();
    }
}
// Monitorea y despierta a los procesos de la cola de bloqueados
void Scheduler::check_io_completion()
{
    if(blocked_queue.empty())
    {
        return;
    }

    std::queue<Process*> new_blocked_queue;

    while(!blocked_queue.empty())
    {
        Process* process = blocked_queue.front();

        if(process->get_remaining_io_time() == 0)
        {
            if(process->get_remaining_cpu_time() == 0)
            {
                process->finish_process(global_clock);
                process->set_state(State::TERMINATED);
                terminated_processes.push_back(process);
            }
            else
            {
                process->set_state(State::READY);
                ready_queue.push_back(process);
            }
        }
        else
        {
            new_blocked_queue.push(process);
        }
        blocked_queue.pop();
    }
    blocked_queue = new_blocked_queue;
}


// Decide qué proceso pasa a RUNNING (Implementa lógica expulsiva para algoritmos expulsivos)
void Scheduler::select_next_process()
{
    if(running_process != nullptr)
    {
        if(active_algorithm == Algorithm::FCFS || active_algorithm == Algorithm::SJF || active_algorithm == Algorithm::RANDOM_SELECTION || active_algorithm == Algorithm::PRIORITY_SCHEDULING)
        {
            return;
        }
        else if(!ready_queue.empty())
        {
            sort_ready_queue();
            Process* best_candidate = ready_queue.front();
            bool should_change = false;

            switch (active_algorithm)
            {
            case Algorithm::SRTF:
                should_change = best_candidate->get_remaining_cpu_time() < running_process->get_remaining_cpu_time();
                break;
            case Algorithm::RR:
                should_change = false;
                break;
            case Algorithm::PRIORITY_SCHEDULING_EXPULSIVE:
                should_change = best_candidate->get_priority() > running_process->get_priority();
                break;
            default:
                break;
            }

            if(should_change)
            {
                running_process->set_state(State::READY);
                ready_queue.push_back(running_process);

                running_process = best_candidate;
                ready_queue.erase(ready_queue.begin());
                running_process->set_state(State::RUNNING);

                sort_ready_queue();
            }
        }
        return;
    }

    if(!ready_queue.empty())
    {
        int index = 0;
        if(active_algorithm == Algorithm::RANDOM_SELECTION)
        {
            index = get_random_ready_index();
        }
        else
        {
            sort_ready_queue();
        }
        running_process = ready_queue[index];
        ready_queue.erase(ready_queue.begin()+index);
        running_process->set_state(State::RUNNING);

        if(active_algorithm == Algorithm::RR)
        {
            current_quantum_ticks = 0;
        }
    }
}
// Ordena la cola de listos bajo criterios de SJF, FCFS, SRTF o Prioridades
void Scheduler::sort_ready_queue()
{
    switch (active_algorithm)
    {
    case Algorithm::FCFS:
        std::sort(ready_queue.begin(), ready_queue.end(), [](Process* a, Process* b)
        {
            return a->get_arrival_time() < b->get_arrival_time();
        });
        break;
    case Algorithm::SJF:
        std::sort(ready_queue.begin(), ready_queue.end(), [](Process* a, Process* b)
        {
            if (a->get_cpu_burst() == b->get_cpu_burst()) {
                return a->get_arrival_time() < b->get_arrival_time();
            }
            return a->get_cpu_burst() < b->get_cpu_burst();
        });
        break;
    case Algorithm::PRIORITY_SCHEDULING:
        std::sort(ready_queue.begin(), ready_queue.end(), [](Process* a, Process* b)
        {
            if (a->get_priority() == b->get_priority())
            {
                return a->get_arrival_time() < b->get_arrival_time();
            }
            return a->get_priority() > b->get_priority();
        });
        break;
    case Algorithm::PRIORITY_SCHEDULING_EXPULSIVE:
        std::sort(ready_queue.begin(), ready_queue.end(), [](Process* a, Process* b)
        {
            if (a->get_priority() == b->get_priority())
            {
                return a->get_arrival_time() < b->get_arrival_time();
            }
            return a->get_priority() > b->get_priority();
        });
        break;
    case Algorithm::SRTF:
        std::sort(ready_queue.begin(), ready_queue.end(), [](Process* a, Process* b)
        {
            if (a->get_remaining_cpu_time() == b->get_remaining_cpu_time())
            {
                return a->get_arrival_time() < b->get_arrival_time();
            }
            return a->get_remaining_cpu_time() < b->get_remaining_cpu_time();
        });
        break;
    default:
        break;
    }
}
// Retorna un índice al azar válido dentro de la cola de listos
int Scheduler::get_random_ready_index()
{
    if(ready_queue.empty())
    {
        return -1;
    }

    int max_index = (ready_queue.size() - 1);

    std::random_device rd;
    static std::mt19937 generator(rd());

    std::uniform_int_distribution<int> index_dist(0, max_index);
    int index = index_dist(generator);
    return index;
}

// Métodos
// Registra un nuevo proceso
void Scheduler::add_process(const Process& process)
{
    all_processes.emplace_back(process);
}
//Genera un lote parametrizado de procesos con valores pseudoaleatorios válidos
void Scheduler::generate_random_processes(int count, int max_cpu, int max_io, int max_priority, int max_arrival)
{
    if (count <= 0)
    {
        return;
    }
    if (max_cpu <= 0)
    {
        max_cpu = 1;
    }
    if (max_priority <= 0)
    {
        max_priority = 1;
    }
    if (max_arrival < 0)
    {
        max_arrival = 0;
    }

    // Configuración del motor de números aleatorios con distribución uniforme
    std::random_device rd;
    static std::mt19937 generator(rd());

    std::uniform_int_distribution<int> cpu_dist(1, max_cpu);
    std::uniform_int_distribution<int> io_dist(0, max_io);
    std::uniform_int_distribution<int> priority_dist(1, max_priority);
    std::uniform_int_distribution<int> arrival_dist(0, max_arrival);

    for (int i = 0; i < count; ++i)
    {
        int id = i + 1;
        int cpu_burst = cpu_dist(generator);
        int io_burst = io_dist(generator);
        int priority = priority_dist(generator);
        int arrival = arrival_dist(generator);

        Process new_process(id, priority, arrival, cpu_burst, io_burst);
        add_process(new_process);
    }
}

// Ejecuta un único paso elemental de reloj (Avanza 1 tick y corre todas las rutinas necesarias)
void Scheduler::step()
{
    update_running_process();
    check_arrivals();
    check_io_completion();
    check_cpu_completion();
    select_next_process();
    global_clock++;
}
// Resetea los procesos y contadores manteniendo la lista base cargada para simular otro algoritmo
void Scheduler::reset()
{
    for(auto& process : all_processes)
    {
        process.reset_process();
    }

    ready_queue.clear();
    terminated_processes.clear();
    while(!blocked_queue.empty())
    {
        blocked_queue.pop();
    }

    running_process = nullptr;
    global_clock = 0;
    current_quantum_ticks = 0;
    total_idle_ticks = 0;
    arrivals_per_tick.clear();
}
// Limpia por completo todas las colas e historiales
void Scheduler::clear_all()
{
    all_processes.clear();
    ready_queue.clear();
    terminated_processes.clear();
    while(!blocked_queue.empty())
    {
        blocked_queue.pop();
    }
    running_process = nullptr;
    global_clock = 0;
    current_quantum_ticks = 0;
    total_idle_ticks = 0;
    arrivals_per_tick.clear();
}
// Ejecuta de forma secuencial la simulación hasta vaciar todas las colas
void Scheduler::run_until_complete()
{
    while(terminated_processes.size() < all_processes.size())
    {
        step();
    }
}

// Estadísticas
// Porcentaje de uso del procesador
double Scheduler::get_cpu_usage() const
{
    if(global_clock == 0)
    {
        return 0.0;
    }
    double result = (100.00 * (global_clock - total_idle_ticks)) / global_clock;
    return result;
}
// Tiempo promedio que pasaron los procesos esperando
double Scheduler::get_avg_waiting_time() const
{
    if(all_processes.empty())
    {
        return 0.0;
    }

    double sum = 0.0;

    for(auto& process : all_processes)
    {
        sum += process.get_waiting_time();
    }

    return sum / all_processes.size();
}
// Tiempo promedio que pasaron los procesos en E/S
double Scheduler::get_avg_block_time() const
{
    if(all_processes.empty())
    {
        return 0.0;
    }

    double sum = 0.0;

    for(auto& process : all_processes)
    {
        sum += process.get_block_time();
    }

    return sum / all_processes.size();
}
// Tiempo promedio de retorno / respuesta total
double Scheduler::get_avg_execution_time() const
{
    if(terminated_processes.empty())
    {
        return 0.0;
    }

    double sum = 0.0;

    for(const auto& process : terminated_processes)
    {
        double execution_time = process->get_ending_time() - process->get_arrival_time();
        sum += execution_time;
    }

    return sum / terminated_processes.size();
}
