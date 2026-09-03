# pcVitals
Embedded project for displaying pc vital metrics in my pc case. 

![General Architectural Diagram](images/general_arch_diagram_v2.png)


# Steps:
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
