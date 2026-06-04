#include "headers/process.h"

// Constructor
Process::Process(int id, int priority, int arrival, int cpu_burst, int io_burst)
    : id(id), priority(priority), arrival_time(arrival), cpu_burst_time(cpu_burst), io_burst_time(io_burst)
{
    this->remaining_cpu_time = cpu_burst;
    this->remaining_io_time = io_burst;

    this->current_state = State::NEW;

    this->block_time = 0;
    this->waiting_time = 0;
    this->ending_time = 0;
}
// Getters
int Process::get_id() const
{
    return id;
}
int Process::get_priority() const
{
    return priority;
}
int Process::get_arrival_time() const
{
    return arrival_time;
}
int Process::get_cpu_burst() const
{
    return cpu_burst_time;
}
int Process::get_io_burst() const
{
    return io_burst_time;
}
State Process::get_current_state() const
{
    return current_state;
}
int Process::get_remaining_cpu_time() const
{
    return remaining_cpu_time;
}
int Process::get_remaining_io_time() const
{
    return remaining_io_time;
}
int Process::get_waiting_time() const
{
    return waiting_time;
}
int Process::get_block_time() const
{
    return block_time;
}
int Process::get_ending_time() const
{
    return ending_time;
}

//Setters
void Process::set_state(State new_state)
{
    current_state = new_state;
}
void Process::set_priority(int new_priority)
{
    priority = new_priority;
}
void Process::set_cpu_burst(int cpu_burst)
{
    cpu_burst_time = cpu_burst;
}
void Process::set_io_burst(int io_burst)
{
    io_burst_time = io_burst;
}

// Métodos

// Avanza el estado interno del proceso en 1 tick, acumulando esperas o decrementando ráfagas según su estado actual
void Process::update_tick()
{
    switch (current_state)
    {
        case State::READY:
            waiting_time++;
            break;

        case State::RUNNING:
            if(remaining_cpu_time > 0)
            {
                remaining_cpu_time--;
            }
            break;

        case State::BLOCKED:
            if(remaining_io_time > 0)
            {
                remaining_io_time--;
                block_time++;
            }
            break;

        case State::TERMINATED:
            break;

        case State::NEW:
            break;

        default:
            break;
    }
}
// Evalúa si el proceso cumplió con toda su carga de trabajo tanto de CPU como de operaciones de E/S
bool Process::is_finished() const
{
    return remaining_cpu_time == 0 && remaining_io_time == 0;
}
// Retorna verdadero si el proceso ha completado su tiempo requerido en la cola de Bloqueados
bool Process::is_io_finished() const
{
    return remaining_io_time == 0;
}
// Transiciona formalmente el estado a TERMINATED y fija el instante del reloj global de finalización
void Process::finish_process(int current_time)
{
    this->ending_time = current_time;
    this->current_state = State::TERMINATED;
}
// Restablece los contadores y tiempos restantes a su estado original para permitir simulaciones limpias
void Process::reset_process()
{
    current_state = State::NEW;
    remaining_cpu_time = cpu_burst_time;
    remaining_io_time = io_burst_time;
    waiting_time = 0;
    block_time = 0;
}