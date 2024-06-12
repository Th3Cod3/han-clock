# MCU CODE

## File structure

| Directory | Description |
| --- | --- |
| RTE | Runtime environment, this directory is part of the keil IDE |
| RTE/Device | Device specific files |
| RTE/_`<context>` | Context specific files |
| firmware | Firmware files. Contains higher-level application code that uses drivers to implement specific functionalities. |
| firmware/`<module>` | Module specific firmware files |
| drivers | Drivers files. Contains low-level drivers that interact with the hardware. |
| drivers/`<module>` | Module specific driver files |
| utils | Utility files. Contains files that are used across the project. |
| src | Source files. Contains the main code that ties everything together. |
