# Minisumo-Autonomo-Senior-activacionBT

Firmware de control para un robot minisumo autonomo basado en ESP32-DevKit. El
algoritmo PID original se porto desde Arduino y ahora corre sobre ESP-IDF dentro
de PlatformIO, modularizado para sensores, motores, LED de estado y logica de
estrategia.

## Caracteristicas principales

- Lectura analogica de tres sensores tipo Sharp (driver ADC1) y calculo de posicion.
- Control de motores con MCPWM, rampas de potencia y frenado activo.
- Seleccion de estrategia mediante boton: pulsaciones cortas (1 a 3) y pulsacion larga para confirmar.
- LED RGB para mostrar la estrategia activa y los estados de confirmacion.
- Entrada GO que sincroniza la salida del robot y logica de speed boost cuando el sensor central
  detecta alta intensidad durante un intervalo definido.

## Archivos clave

- `platformio.ini`: define la plataforma `espressif32`, el board `esp32dev` y la velocidad del monitor (115200 bps).
- `main/app_main.c`: orquesta la seleccion de estrategia y llama a los modulos.
- `main/control_logic.*`, `main/motor_control.*`, `main/sensor_array.*`, `main/status_led.*`.
- `main/hardware_config.h`: mapea pines, constantes PID y umbrales de sensores.

## Requisitos previos

- PlatformIO Core 6.1 (CLI) o la extension de VS Code (usa el mismo backend).
- Python 3.x (lo instala PlatformIO) y suficiente espacio en disco para toolchains de ESP-IDF.
- Drivers USB del ESP32 (CP210x o CH340, segun la tarjeta) y un cable de datos.
- ESP32 Dev Module cableado segun `main/hardware_config.h`.

## Configuracion rapida

1. Clona o descarga este repositorio.
2. Abre una terminal dentro de la carpeta del proyecto.
3. Verifica la instalacion con `pio --version`. Si no existe, instala PlatformIO Core.
4. El primer `pio run` descargara ESP-IDF, Xtensa GCC y dependencias automaticas.

## Tutorial: clonar y abrir en Visual Studio Code

1. **Instala VS Code** desde https://code.visualstudio.com/ y abre la aplicacion.
2. **Instala la extension PlatformIO IDE** (icono de cuadros > busca "PlatformIO" > Install). Esto agrega la CLI y los paneles graficos.
3. **Clona el repositorio** directamente desde VS Code:
   - Presiona `Ctrl+Shift+P`, escribe `Git: Clone` y selecciona la opcion.
   - Pega la URL del repositorio (por ejemplo `https://github.com/usuario/Minisumo-Autonomo-Senior-activacionBT.git`).
   - Elige una carpeta vacia donde guardar el proyecto y acepta la pregunta "Would you like to open the cloned repository?".
4. **Espera las descargas automáticas**: al abrir la carpeta, PlatformIO instalara toolchains de ESP-IDF en segundo plano (primer arranque tarda algunos minutos).
5. **Selecciona el entorno**: en la barra inferior veras `PIO Home`. Abrelo, ve a *Projects* y confirma que `esp32dev` aparezca como Default Environment.
6. **Compila y sube desde la GUI**:
   - Usa los botones de PlatformIO (check para `Build`, flecha para `Upload`).
   - Si prefieres la terminal integrada, ejecuta `pio run -e esp32dev` y `pio run -t upload -e esp32dev --upload-port COMx`.
7. **Monitor serie**: pulsa el icono del enchufe en la barra lateral de PlatformIO o corre `pio device monitor --port COMx --baud 115200` dentro del terminal integrado.
8. **Gestiona configuraciones**: cualquier cambio en `platformio.ini` o `sdkconfig.defaults` se refleja de inmediato; puedes abrir `menuconfig` con `pio run -t menuconfig -e esp32dev` desde VS Code.

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

### Menuconfig de ESP-IDF (opcional)

```bash
pio run -t menuconfig -e esp32dev
```

Esto genera `sdkconfig` dentro de `.pio/build/esp32dev` y PlatformIO lo reutiliza en compilaciones posteriores.

## Flujo de operacion del robot

1. Al encender, entra en modo de seleccion de estrategia:
   - Pulsaciones cortas incrementan la estrategia (maximo 3) y el LED muestra el color asignado.
   - Una pulsacion larga (>1 s) confirma y pasa a la secuencia de arranque.
2. Espera a que la entrada `GO` quede en nivel alto para ejecutar la maniobra inicial y habilitar el lazo PID.
3. Si el sensor central supera `SENSOR_SPEED_BOOST_THRESHOLD` durante
   `SENSOR_SPEED_BOOST_DURATION_MS`, la potencia base aumenta automaticamente.

Ajusta parametros de hardware, PID, umbrales y colores dentro de `main/hardware_config.h`.

## Notas adicionales

- PlatformIO ya exporta ESP-IDF y administra el toolchain, por lo que no es necesario ejecutar `idf.py set-target`
  ni activar entornos manualmente.
- Mantener `platformio.ini` como unica fuente de configuracion permite usar la CLI, VS Code o CI sin pasos extra.
