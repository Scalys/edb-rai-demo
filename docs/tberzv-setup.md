# EDB demo TrustBox Edge RZ/V2L firmware setup guide

This guide describes a process for setting up Scalys TrustBox Edge RZ/V2L (TBE RZ/V2L) for running a demo for Enclave Device Blueprint for Confidential Computing.

# Device information

TrustBox Edge RZ/V2L is an edge device based on Renesas RZ/V2L MPU. It is tailored for the most secure workload on the Edge. For more information please contact [Scalys](https://scalys.com).

# Firmware information

The provided firmware is a demonstrational Armbian-based system. For any real applications firmware should be based on the latest available firmware releases by Scalys.

Default credentials:
```
login: admin
password: trust
```

Default network configuration includes internet access via ethernet port with DHCP. Alternatively, internet connection can also be provided over WiFi. It can be set up via NetworkManager:

```
$ sudo nmcli device wifi connect <SSID> password <PASSWORD>
```

# Demo composition

In an assembled case the demo consists of:
- TrustBox Edge RZ/V2L
- USB microphone Boya BY-LM40
- monitor, connected over DisplayPort

# Demo flow

The minimal flow of the default image runs the ML demo inside of enclave only. Thus, insures that the confidential workload is possible to run on the device. For the steps related to Enclave Device Blueprint a dedicated Azure infrastructure is required with unique information (such as Azure function URLs etc.), thus it is expected to be performed on the second stages of the demo.

1. connect the Monitor to TBE via DisplayPort connector
1. plug in the USB microphone into TBE RZ/V2L
1. connect to over serial port (USB type B) using terminal emulator like PuTTY, picocom or other
1. power up the device
1. set up the internet connection over WiFi
1. configure the Azure IoT Edge connection to the hub
1. when device is powered up, it is expected to boot until Linux and await confidential load to be deployed

# Firmware update

1. download the [sdcard image](http://trustbox.scalys.com/pub/openenclave/trustbox-rzv-armbian-2111-edb-2022.10.15.img.gz) binary image, unpack it and flash it to the SD card using "dd" utility in Linux:
```
$ sudo dd if=trustbox-rzv-armbian-2111-edb-2022.10.15.img.gz of=/dev/sdX
```

where /dev/sdX is the actual SD card device. Or in Windows using tool like [Etcher](https://www.balena.io/etcher).


# Demo

For an explanation of a end-to-end Enclave Device Blueprint reference implementation see the [EDB demo guide](https://github.com/Scalys/edb-key-demo/blob/main/docs/edb-demo-guide.md)
