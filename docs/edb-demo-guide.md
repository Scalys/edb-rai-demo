# Enclave Device Blueprint with Scalys TrustBox RZ/V2L

## Device information

TrustBox Edge RZ/V2L is an edge device based on Renesas RZ/V2L MPU. It is tailored for secure ML worload at the edge. For more information please contact [Scalys](https://scalys.com).

## Demo description

The subject of the demo is a generalized protected workload (in this case - confidential ML inference algorith) running inside of an enclave on the edge device, which is developed and deployed with the use of Enclave Device Blueprint.

ML algorithm performes a recognition and classification of the sound recorded from a microphone. Recognized classes of sounds: silence, typical human voice, keyboard typing sound. This kind of workload is an example of practical application of ML at the Edge, as it is device/site based and contains prorietary IP of a customer. This makes it a good application of Confidential Computing for protecting the workload and its deployment.

## Firmware information

The provided firmware is a demonstrational Armbian-based system. For any real applications firmware should be based on the latest available firmware releases by Scalys.

Default credentials:
```
login: admin
password: trust
```

Default network configuration sets up both ethernet ports to expect internet connection via DHCP. It is also possible to connect to Internet via WiFi.

## Blueprint demonstration

A demo consists of two "demo flows":
- visual simple demonstration, that confidentially performs an ML inference
- full Enclave Device Blueprint end-to-end demonstration with all steps required to implement the Blueprint

### Visual Demo composition

In an assembled case the demo consists of:

- TrustBox Edge RZ/V2L
- web camera Logitech C270
- a set of manufactured physical keys
- a PC for connecting to the running demo over network
- (optional) serial USB-B connection to PC for additional control

Notes: the machine learning demo was prepared in expectation that camera is pointing downwards from approximately 20-30cm hight onto the white surface. Other configurations are not guaranteed to provide a reliable inference quality.

### Visual demo flow

The minimal flow of the default image runs the ML demo inside of enclave only. Thus, insures that the confidential workload is possible to run on the device. For the steps related to Enclave Device Blueprint a dedicated Azure infrastructure is required with unique information (such as Azure function URLs etc.), thus it is expected to be performed on the second stages of the demo.

1. connect the Monitor to TBE via DisplayPort connector
1. plug in the USB microphone into TBE RZ/V2L
1. connect to over serial port (USB type B) using terminal emulator like PuTTY, picocom or other
1. power up the device
1. set up the internet connection over WiFi
1. configure the Azure IoT Edge connection to the hub
1. when device is powered up, it is expected to boot until Linux and await confidential load to be deployed

## Enclave Device Blueprint operation

A full end-to-end Enclave Device Blueprint experience consists of several different stages performed at different locations: at cloud, at developer machine (or CI/CD pipeline), at edge device.

For an overview please refer to the [Enclave Device Blueprint overview](https://github.com/Azure-Samples/Project_Confidential_Apps_for_IoT_with_Enclaves).

### Deploy the Azure infrastructure

As a cornerstone for the key management and edge workload deployment this demo uses the Azure cloud infrastructure. As all the other stages/components in the Blueprint, Azure cloud can be replaced with another cloud provider platform or a local Key Management Service.

The provided reference implementation is based on Azure cloud and it provides a means of one-click deployment of all the required infrastructure. It can be found [here](https://github.com/Azure-Samples/Project_Confidential_Apps_for_IoT_with_Enclaves/blob/main/docs/azure-environment-setup.md).

### Provision Key

Proceed to allocate a new class key for the future applicatoin:
```
$ edb-provision-key.sh KEY FUNCTION CODE
Provision new keys in the clowd for Enclave Device Blueprint

Arguments:
    KEY        key name. In most cases should match an uuid name of the trusted
               application.
               Example: c7198089-6313-4ba6-8e3c-47846520df18
    FUNCTION   name of the Azure function app. 
               Example: funczz2n5qra5ixe8
    CODE       function code acquired from Azure portal.
               Example: ecnSSC7bumjV8i717d1uZuQritHxKv83vEwszt4iTS4MAzFuS76FkA==
```

### Developing Trusted Application

First stage is to start developing the Trusted Application using OpenEnclave framework. For documentation please refer to its [community documentation](https://github.com/openenclave/openenclave/tree/master/docs/Community).

For cross-compiling the image to the device use the prepared [toolchain](http://trustbox.scalys.com/pub/openenclave/poky-glibc-x86_64-scalys-openenclave-image-aarch64-trustbox-rzv2l-toolchain-ext-3.1.19.sh). Or directly use the docker build environment that comes with the project:

```
$ ./docker/run-20.04.sh
```

#### Reality AI ML model

Follow to the Reality AI [portal](https://portal.reality.ai) (account required).

<img src="./docs/images/rai-portal.jpg" alt="RealityAI portal" width="640">

To generate the model perform the following steps:
1. record audio samples
1. create project
1. upload the audio sources: Data -> Source
1. curate data into segmented lists: Data -> Curate
1. search for optimal ML models: AI Explore™ -> Classes
1. create base tools for the best discovered models: AI Explore™ -> Classes
1. train the created base tools on the input data: Build -> Train
1. generate the deployment for Cortex-A55 architecture and download it
1. produced and downloaded zip-archive will contain a set of files. Put libRealityAI.a into the demo sources project: edb-rai-demo/libs/realityai/libRealityAI.a. Put header file into edb-rai-demo/enc.
1. proceed building the EDB demo application

#### Application build

To build the EDB RealityAI demo trusted application:

1. source the build environment
    ```
    source ~/poky_sdk/environment-setup-aarch64-poky-linux
    ```
1. build the application
    ```
    $ mkdir build && cd build
    $ cmake ../edb-rai-demo -DPKG_CONFIG_SYSROOT_DIR="${PKG_CONFIG_SYSROOT_DIR}" -DOE_TEE=OP-TEE -DOE_PACKAGE_OPTEE_PLATFORM=trustsom -DOE_OPTEE_PLATFORM=trustsom -DCMAKE_TOOLCHAIN_FILE=../edb-key-demo/cmake/arm-cross.cmake -DCMAKE_BUILD_TYPE=Release -DOE_PACKAGE_PREFIX=${PKG_CONFIG_SYSROOT_DIR}/opt/oe
    $ make
    ```
1. build results (`edb_rai_demo` and `c7198089-6313-4ba6-8e3c-47846520df18.ta`) are located in `out/bin` directory.

### Confidential Package assembly

To enclose the resulting Trusted Application into a Confidential Package (.cpk) use the cpk-tool. Instructions on where to obtain it can be found [here](github.com/Scalys/ConfidentialPackageTools/).
This tool allows to build a Confidential Package in several ways. But for the most classical approach the http Wrap Key approach should be used. The command call in this case would look like this:

```
$ cpk-tool build -i WORKLOAD -a NAME -w local -e http -k WRAP_URL -o OUTPUT_CPK
```

where:
1. `WORKLOAD` - a file to be enclosed in the confidential package (Trusted Application)
1. `NAME` - name of the key to be used (should match the one used in Key Provisioning stage)
1. `WRAP_URL` - an URL of a Wrap Key function. Should be taken from the deployed infrastructure in Azure Function Apps (see the function WrapKey).
1. `OUTPUT_CPK` - output .cpk file

Example:

```
$ cpk-tool build -i c7198089-6313-4ba6-8e3c-47846520df18 .ta -a c7198089-6313-4ba6-8e3c-47846520df18 -w local -e http -k "https://funczz2n5qra5ixf6.azurewebsites.net/api/WrapKey?code=j7v-P_2sTWeogbdoIO55W8-qaj6pXjQw4dBEC8XlBXtlAzFuWJbfQg==" -o ./edb_rai_demo.cpk
```

### Deploying the confidential package

Delivering the confidential package to the Edge can be done in a number of different ways, ranging from a cloud deployment (like the Azure IoT Edge) to a physical transfer by any means possible. Using the Enclave Device Blueprint means that the workload will only be decrypted on the end device.

For enclosing the produced Confidential Package in the Azure IoT Edge refer to the [guide](https://docs.microsoft.com/en-us/azure/iot-edge/module-development?view=iotedge-2020-11) on developing the IoT Edge modules.

### Sync the keys on the Edge

To be able to install (unpack and re-key the confidential workload) a sync process should be performed on the Edge device. It can be done using the Device Twins and the helper script ```edb-sync-key.sh```. It should run in parallel and when needed - extract the class key.

### Install the workload

To be able to execute the Confidential Package it first should be installed. This process includes using the class key to decrypt the protected workload within enclave, and store it re-keyed on the local storage by means of Secure Storage.

It can be done with the following call:
```
$ cpk-tool install -p edb_rai_demo.cpk
```

### Execute the application

Once the confidential package is installed on the system, it can be executed like a typical Trusted Application:

```
$ ./edb_rai_demo c7198089-6313-4ba6-8e3c-47846520df18.ta
```
