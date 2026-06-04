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
* `resources/` -> Archivos fuente (`.cpp`) con la implementación de la lógica y eventos.
* `screens/` -> Archivos de diseño de interfaz de usuario (`.ui`) de Qt.
* `Process_Scheduling_Simulator.pro` -> Archivos de configuración del proyecto para la compilación (qmake).

---

## 🛠️ Requisitos e Instalación

Para compilar y ejecutar este proyecto localmente, necesitas tener instalado:

1.  **Qt Creator** (Versión compatible con C++17 o superior).
2.  Un compilador compatible (MinGW en Windows, GCC en Linux o Clang en macOS).

### Pasos para ejecutar:

1.  Clona o descarga este repositorio.
2.  Abre Qt Creator.
3.  Selecciona **Open Project** y busca el archivo `Process_Scheduling_Simulator.pro`.
4.  Configura el kit de compilación de tu preferencia.
5.  Haz clic derecho en la raíz del proyecto y selecciona **Run qmake** (o compila directamente con el icono del martillo 🔨).
6.  Presiona el botón **Run** (Icono de Play verde ▶️).

---
