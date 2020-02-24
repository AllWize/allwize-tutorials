# Install Arduino IDE

The Arduino IDE is a simple code editor that will allow you to create your sketches and upload them to the AllWize K2. It is open source and you can download it from the [Arduino Software page](https://www.arduino.cc/en/Main/Software) and also make a donation to help the project.

The installation procedure will depend on your operating system.

## Windows

Don't forget to install the drivers for the Arduino boards. It is an option in the Arduino IDE installation process.

![Arduino IDE Install drivers](images/arduino-ide-install-drivers.png)

## MacOS

TODO: Arduino IDE installation instructions for MacOS

## Linux

Run the `arduino-linux-setup` utility in the folder you download using your user, it will configure the user groups to work with the boards and also remove non-compatible dependencies (they are safe to remove):

```
$ ./arduino-linux-setup.sh $USER

******* Add User to dialout,tty, uucp, plugdev groups *******

[sudo] password for allwize:         
groupadd: group 'plugdev' already exists
groupadd: group 'dialout' already exists

******* Removing modem manager *******

Reading package lists... Done
Building dependency tree       
Reading state information... Done
The following packages will be REMOVED:
  modemmanager
0 upgraded, 0 newly installed, 1 to remove and 80 not upgraded.
After this operation, 3.408 kB disk space will be freed.
(Reading database ... 326946 files and directories currently installed.)
Removing modemmanager (1.10.0-1~ubuntu18.04.2) ...
Created symlink /run/systemd/system/ModemManager.service → /dev/null.
Processing triggers for dbus (1.12.2-1ubuntu1.1) ...
Processing triggers for hicolor-icon-theme (0.17-2) ...
Processing triggers for man-db (2.8.3-2ubuntu0.1) ...

Restarting udev


*********** Please Reboot your system ************

```

Restart the system once finished. You should be able to run the IDE now.


[Previous: AllWize K2 Components](01-allwizek2-components.md) |
[Tutorial menu](readme.md) |
[Next: Adding support for the AllWize K2 in the Arduino IDE](03-allwizek2-arduino-ide-support.md)