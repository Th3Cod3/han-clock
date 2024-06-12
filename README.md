# HAN Clock

## Node-Red

### Requirements

- Docker
- Docker Compose

### Setup Node-Red

To start the container, run the following command:

```bash
docker-compose up -d
```

### WSL

On Windows we have to use the WSL to run the docker container running Node-Red. To make possible that the serial port is accesible from the container, we need to install the usbipd package. usbipd is a daemon that allows to share USB devices over IP. To install the package, run the following command on powershell as administrator:

```cmd
win-get install usbipd
```

after that, we need to bind the device to the usbipd daemon and attach it to the container.

Example of binding and attaching the device:

**Note:** The device id can be found by running the command `usbip list`

```cmd
usbipd bind -b 13-3 -f; usbipd attach -b 13-3 -w;
```

```cmd
usbipd detach -b 13-3; usbipd unbind -b 13-3;
```
