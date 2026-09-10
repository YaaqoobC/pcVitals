# pcVitals
Embedded project for displaying pc vital metrics in my pc case. 

![General Architectural Diagram](images/general_arch_diagram_v2.png)

# Hardware:
- LCD-1602A: 16x2 character LCD module
    - Does not contains a 'backpack' (I2C Serial Interface Adapter) so we are running in native parallel mode:
        - Set 4 bits then pulse enable pin 'E' to tell hw its ready.
    - Build my own driver to write to the screen
    - VO pin -> contrast Connected to Potentiometer
    - Using the 4-bit mode, to save on wires and GP's
- Marble Pico (RP2040)
- Potentiometer
    - Acts as an adjustable voltage divider.
    - LCD uses the voltage to adjust the contrast.

    ## Connections:
    ![Pin Connections](images/pico_pin_connections.png)

    | Component Pin | Connection / Microcontroller Pin | Wire Color |
    | :--- | :--- | :--- |
    | **Vss** | GND | Black |
    | **Vdd** | VBUS (5V) | Red |
    | **VO** | Potentiometer 2 | Orange |
    | **RS** | GP9 | White |
    | **E** | GP8 | White |
    | **D7** | GP13 | Green |
    | **D6** | GP12 | Green |
    | **D5** | GP11 | Green |
    | **D4** | GP10 | Green |
    | **A** | VBUS (5V) | Red |
    | **K** | GND | Black |
    | **POT1** | GND | Black |
    | **POT3** | VBUS (5V) | Black |



# DIY LCD Driver:
1. `lcd_init()` -- Init the display
2. `lcd_clear()` -- Clear the display
3. `lcd_set_cursor(row, col)` -- Move the Cursor
4. `lcd_print(text)` -- Print to the display

Internally a single write follows this flow:
1. lcd_print
2. lcd_data
3. lcd_send_byte
4. lcd_write_nibble x2
5. gpio_put
6. pulse_enable


# Data Pipeline:
![Windows Side Architectural Diagram](images/windows_arch_diagram.png)
1. Fetches raw hardware metrics from LibreHWMonitor's local web server.
2. Parses the JSON response into an internal C++ struct.
3. Serializes the metrics into a comma-delimited ASCII string.

# Packet
*Format:* `S,<cpuTemp>,<cpuLoad>,<cpuClk>,<gpuTemp>,<gpuLoad>,<gpuVram>,<ramUsage>,E\n`

*Example:* `S,45.2,10.0,4200.0,35.0,99.0,8000.0,16000.0,E\n`

**UART Parameters**
* **Baud Rate:** 115200
* **Data Bits:** 8
* **Parity:** None
* **Stop Bits:** 1

# Steps To Run:
1. Downloaded `LibreHardwareMonitor.zip` from https://github.com/LibreHardwareMonitor/LibreHardwareMonitor/releases -> under assets
2. Run the `LibreHardwareMonitor.exe` file as an administrator
3. Start the `Remote Web Server` by looking in the options tab
4. Compile and run:
    - Open x64 native tools in admin mode
    - `cd C:\Yaaqoob\Projects_WD\pcVitals\build `
    - `del CMakeCache.txt`
    - `cmake ..`
    - `cmake --build . --config Release`
    - `.\Release\pcVitals.exe`
    
    Or:
    ``` 
    cd C:\Yaaqoob\Projects_WD\pcVitals

    rmdir /s /q build

    mkdir build
    cd build

    cmake ..

    cmake --build . --config Debug

    .\Debug\pcVitals.exe
    ```
