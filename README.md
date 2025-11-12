# Minisumo-Autonomo-Senior-activacionBT

Firmware de control para un robot minisumo autonomo basado en ESP32-DevKit. El
algoritmo PID original se porto desde Arduino y ahora vuelve a ejecutarse sobre
el core de Arduino para ESP32 dentro de PlatformIO, manteniendo los modulos
especificos para sensores, motores, LED de estado y logica de estrategia.

## Caracteristicas principales

- Lectura analogica de tres sensores tipo Sharp (driver ADC1) y calculo de posicion.
- Control de motores con MCPWM, rampas de potencia y frenado activo.
- Seleccion de estrategia mediante boton: pulsaciones cortas (1 a 3) y pulsacion larga para confirmar.
- LED RGB para mostrar la estrategia activa y los estados de confirmacion.
- Arranque remoto via Bluetooth serial: empareja con `MinisumoBT` y envia `1` para habilitar el GO virtual o `0` para detenerte sin usar un pin dedicado.

## Archivos clave

- `platformio.ini`: define la plataforma `espressif32`, el board `esp32dev`, el framework Arduino y la velocidad del monitor (115200 bps).
- `src/main.cpp`: implementa `setup()` y `loop()` llamando al resto de modulos.
- `src/control_logic.*`, `src/motor_control.*`, `src/sensor_array.*`, `src/status_led.*`.
- `src/hardware_config.h`: mapea pines, constantes PID y umbrales de sensores.

## Requisitos previos

- PlatformIO Core 6.1 (CLI) o la extension de VS Code (usa el mismo backend).
- Python 3.x (lo instala PlatformIO) y suficiente espacio en disco para toolchains de ESP-IDF.
- Drivers USB del ESP32 (CP210x o CH340, segun la tarjeta) y un cable de datos.
- ESP32 Dev Module cableado segun `src/hardware_config.h`.

## Configuracion rapida

1. Clona o descarga este repositorio.
2. Abre una terminal dentro de la carpeta del proyecto.
3. Verifica la instalacion con `pio --version`. Si no existe, instala PlatformIO Core.
4. El primer `pio run` descargara el core de Arduino para ESP32 y las dependencias necesarias.

## Tutorial: clonar y abrir en Visual Studio Code

1. **Instala VS Code** desde https://code.visualstudio.com/ y abre la aplicacion.
2. **Instala la extension PlatformIO IDE** (icono de cuadros > busca "PlatformIO" > Install). Esto agrega la CLI y los paneles graficos.
3. **Clona el repositorio** directamente desde VS Code:
   - Presiona `Ctrl+Shift+P`, escribe `Git: Clone` y selecciona la opcion.
   - Pega la URL del repositorio (por ejemplo `https://github.com/usuario/Minisumo-Autonomo-Senior-activacionBT.git`).
   - Elige una carpeta vacia donde guardar el proyecto y acepta la pregunta "Would you like to open the cloned repository?".
4. **Espera las descargas automaticas**: al abrir la carpeta, PlatformIO instalara toolchains de ESP-IDF en segundo plano (primer arranque tarda algunos minutos).
5. **Selecciona el entorno**: en la barra inferior veras `PIO Home`. Abrelo, ve a *Projects* y confirma que `esp32dev` aparezca como Default Environment.
6. **Compila y sube desde la GUI**:
   - Usa los botones de PlatformIO (check para `Build`, flecha para `Upload`).
   - Si prefieres la terminal integrada, ejecuta `pio run -e esp32dev` y `pio run -t upload -e esp32dev --upload-port COMx`.
7. **Monitor serie (opcional)**: pulsa el icono del enchufe en la barra lateral de PlatformIO o corre `pio device monitor --port COMx --baud 115200` para logs. El control GO se hace via Bluetooth, no necesita cable.
8. **Gestiona configuraciones**: ajusta pines y constantes en `src/hardware_config.h` o variables globales. Si necesitas cambiar particiones o flags avanzados, edita `platformio.ini` usando `board_build.*` o archivos en `src/`.

Tip: si usas Visual Studio (no Code), instala la CLI de PlatformIO y Git por separado, clona el repositorio con `git clone ...` y ejecuta los comandos desde la terminal de PowerShell o CMD.

## Flujo de trabajo con PlatformIO

Todos los comandos se ejecutan desde la raiz del proyecto. Ajusta `COMx` por el puerto real
(puedes descubrirlo con `pio device list`). El entorno por defecto es `esp32dev`.

### Compilar

```bash
pio run -e esp32dev
```

### Cargar firmware

```bash
pio run -t upload -e esp32dev --upload-port COMx
```

### Limpiar y recompilar

```bash
pio run -t clean
pio run -e esp32dev
```

### Monitor serie

```bash
pio device monitor --port COMx --baud 115200
```

Sal del monitor con `Ctrl+]`. Para automatizar carga + monitor:

```bash
pio run -t upload -e esp32dev --upload-port COMx && ^
pio device monitor --port COMx --baud 115200
```

### Control via Bluetooth serie

1. Empareja tu PC o smartphone con el dispositivo `MinisumoBT`.
2. Abre una aplicacion de terminal Bluetooth (por ejemplo, Serial Bluetooth Terminal en Android) y conecta al puerto RFCOMM creado.
3. Envia `1` para activar el GO virtual y comenzar la estrategia seleccionada. Envia `0` para detener el robot y reiniciar el lazo.
4. Puedes seguir usando el monitor serie por USB solo para depuracion; no es necesario para arrancar el robot.

### Configuracion avanzada

- Ajusta opciones de compilacion en `platformio.ini` (flags, particiones, tamano de flash, etc.).
- Cambia pines, PID y umbrales dentro de `src/hardware_config.h`.
- Si necesitas personalizar componentes de ESP-IDF expuestos por Arduino, puedes incluir los headers de `driver/*` directamente y modificar el codigo en `src/*.c`.

## Flujo de operacion del robot

1. Al encender, entra en modo de seleccion de estrategia:
   - Pulsaciones cortas incrementan la estrategia (maximo 3) y el LED muestra el color asignado.
   - Una pulsacion larga (>1 s) confirma y pasa a la secuencia de arranque.
2. Tras confirmar la estrategia, envia `1` por la sesion Bluetooth (`MinisumoBT`) para habilitar el GO virtual; envia `0` para detener el robot en cualquier momento.
3. Si el sensor central supera `SENSOR_SPEED_BOOST_THRESHOLD` durante
   `SENSOR_SPEED_BOOST_DURATION_MS`, la potencia base aumenta automaticamente.

Ajusta parametros de hardware, PID, umbrales y colores dentro de `src/hardware_config.h`.

## Notas adicionales

- Aunque corremos sobre Arduino, siguen disponibles las APIs de ESP-IDF (drivers, FreeRTOS), por lo que los modulos C originales se reutilizan sin cambios mayores.
- PlatformIO administra toda la toolchain; no necesitas ejecutar `idf.py` ni activar entornos manualmente.
- Mantener `platformio.ini` como unica fuente de configuracion permite usar la CLI, VS Code o CI sin pasos extra.
