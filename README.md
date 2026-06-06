# Simulador de Planificación de Procesos

Este es un simulador interactivo de planificación de procesos desarrollado en **C++** utilizando el framework gráfico **Qt (Qt Widgets)**. El sistema modela el comportamiento de un sistema operativo al gestionar una cola de procesos a través de diferentes algoritmos de asignación de CPU y ráfagas de entrada/salida (I/O).

---

## 🚀 Características Principales

* **Algoritmos de Planificación Soportados:**
* No expulsivos:
    * Primero en llegar primero en ejecutar (FCFS)
    * Primero el trabajo más corto (SJF)
    * Selección aleatoria
    * Planificación basada en prioridades

* Expulsivos:
    * Turno rotativo (round robin)
    * Primero el menor tiempo restante (SRTF)
    * Planificación basada en prioridades


* **Modos de Generación de Procesos:**
    * **Manual:** Permite al usuario definir ráfagas de CPU, tiempos de llegada y prioridades específicas.
    * **Aleatorio:** Generación automática de cargas de trabajo para pruebas rápidas de rendimiento.
    
* **Control del Tiempo de Ejecución:**
    * Bucle de simulación controlado por eventos mediante `QTimer`.
    * Modo automático con velocidad ajustable (mínimo seguro de 20ms para estabilidad de la interfaz).
    * Modo manual paso a paso (*Step-by-step*) para análisis detallado.

* **Reporte Final de Métricas:** Estadísticas automatizadas de uso de CPU, tiempos de espera promedio, tiempos de bloqueo promedio y tiempos de retorno.

---

## 📁 Estructura del Proyecto

El código está organizado siguiendo principios de separación de responsabilidades para garantizar la portabilidad y limpieza del repositorio:

* `headers/` -> Archivos de cabecera (`.h`) con la lógica del planificador (`scheduler.h`), procesos (`process.h`) y controladores de ventanas.
* `src/` -> Archivos fuente (`.cpp`) con la implementación de la lógica y eventos.
* `screens/` -> Archivos de diseño de interfaz de usuario (`.ui`) de Qt.
* `Process_Scheduling_Simulator.pro` -> Archivos de configuración del proyecto para la compilación (qmake).

---

## 🛠️ Requisitos e Instalación
Para compilar y ejecutar este proyecto localmente, necesitas tener instalado un compilador compatible (MinGW en Windows, GCC en Linux) y las librerías del framework Qt (versión compatible con C++17 o superior).

### Opción 1: Ejecución desde Qt Creator (Recomendado)
1.  Clona o descarga este repositorio.
2. Abre Qt Creator.
3. Selecciona **Open Project** y busca el archivo `Process_Scheduling_Simulator.pro`.
4. Configura el kit de compilación de tu preferencia (ej. MinGW).
5. Haz clic derecho en la raíz del proyecto y selecciona **Run qmake**.
6. Presiona el botón **Run** (Icono de Play verde ▶️).


### Opción 2: Compilación desde la Terminal (Linux)

#### 1. Clonar el repositorio
```bash
git clone https://github.com/MelhanySL/Process-Scheduling-Simulator.git
cd Process-Scheduling-Simulator
```
#### 2. Instalar herramientas de desarrollo y Qt6
```bash
sudo apt update && sudo apt install build-essential qt6-base-dev
```
#### 3. Compilar el proyecto
```bash
qmake Process_Scheduling_Simulator.pro
make clean
make -j4
```
#### 4. Ejecutar el simulador gráfico
```bash
./Process_Scheduling_Simulator
```
