# pcVitals
Embedded project for displaying pc vital metrics in my pc case. 

# Steps:
1. Downloaded `LibreHardwareMonitor.zip` from https://github.com/LibreHardwareMonitor/LibreHardwareMonitor/releases -> under assets
2. Run the `LibreHardwareMonitor.exe` file as an administrator
3. Start the `Remote Web Server` by looking in the options tab
4. Compile and run:
    - `cd C:\Yaaqoob\Projects_WD\pcVitals\build `
    - `del CMakeCache.txt`
    - `cmake ..`
    - `cmake --build . --config Release`
    - `.\Release\pcVitals.exe`
