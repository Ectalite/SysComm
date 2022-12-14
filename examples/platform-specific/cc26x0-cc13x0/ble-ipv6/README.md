# platform-specific/cc26x0-cc13x0/ble-ipv6

## BLEach: a fully open-source IPv6-over-BLE stack for Constrained Embedded IoT Devices

### Overview
In 2015, the IETF released the [RFC 7668][rfc7668] that specifies how IPv6 packets
can be exchanged using BLE connections (IPv6 over BLE).
This Contiki extenstion implements [BLEach][bleachWeb], a fully open-source IPv6-over-BLE stack for Contiki.
BLEach in Contiki-NG can be used for node (BLE slave) devices.

It was developed by
* [Michael Spoerk](http://www.michaelspoerk.com), Graz University of Technology, michael.spoerk@tugraz.at, github user: [spoerk](https://github.com/spoerk)

This IPv6-over-BLE stack is presented and evaluated in the paper:
[BLEach: Exploiting the Full Potential of IPv6 over BLE in Constrained Embedded IoT Devices](http://sensys.acm.org/2017/), ACM SenSys'17.

### Features
This implementation includes:
  * IPv6-over-BLE node implementation compliant to [RFC 7668][rfc7668]
    * connect to a single IPv6-over-BLE border router
    * maximum IPv6 packet length of 1280 bytes
    * BLE L2CAP channels in LE credit-based flow control mode
  * BLE link layer support for version [4.1][bleSpec]:
    * BLE advertisement
    * BLE connection slave

It has been tested on the TI CC2650 SensorTag and the TI CC2650 LaunchPad hardware.

### Modules
The IPv6-over-BLE stack comes with the following modules:

#### BLE radio
The implementation of the BLE radio for the TI CC26xx platform is implemented in `arch/cpu/cc26x0-cc13x0/rf-core/ble-cc2650.c`
and `arch/cpu/cc26x0-cc13x0/rf-core/ble-hal/*.[ch]`.
These files contain all the hardware specific code for supporting BLE as a link layer.

#### BLE L2CAP layer
The L2CAP LE credit-based flow control support is implemented in `arch/cpu/cc26x0-cc13x0/rf-core/ble-l2cap.c`.
Besides implementing rudimentary L2CAP support, this module handles fragmentation of large IPv6 packets.

### Using BLEach
Currently, BLEach is only available for the Texas Instruments CC2650 hardware platform.

The following sections describe how to configure BLEach for IPv6-over-BLE nodes and border routers.

#### IPv6-over-BLE node (BLE slave)
To enable IPv6 over BLE, the project conf needs to contain:
```
#define PACKETBUF_CONF_SIZE                   1280
#define QUEUEBUF_CONF_NUM                       1
#define UIP_CONF_BUFFER_SIZE                  1280

#define NETSTACK_CONF_RADIO                 ble_cc2650_driver
#define NETSTACK_CONF_MAC                   ble_l2cap_driver

#define RTIMER_CONF_MULTIPLE_ACCESS       1

/* 6LoWPAN settings */
#define SICSLOWPAN_CONF_COMPRESSION           SICSLOWPAN_COMPRESSION_HC06
#define SICSLOWPAN_CONF_COMPRESSION_THRESHOLD   0  /* always use compression */
#define SICSLOWPAN_CONF_FRAG                    0
#define SICSLOWPAN_FRAMER_HDRLEN                0

/* network stack settings */
#define UIP_CONF_ROUTER                         0
#define UIP_CONF_ND6_SEND_NA          1

```

The following optional parameter can be used to configure that BLE advertisement behaviour:
```
#define BLE_CONF_DEVICE_NAME          "TI CC26xx device"
#define BLE_CONF_ADV_INTERVAL         25
```
`BLE_CONF_DEVICE_NAME` holds the device name that is used for advertisement, `BLE_CONF_ADV_INTERVAL`
specifies the used advertisement interval in milliseconds.


[rfc7668]: https://tools.ietf.org/html/rfc7668
[bleSpec]: https://www.bluetooth.com/specifications/bluetooth-core-specification/legacy-specifications
[bleachWeb]: http://www.iti.tugraz.at/BLEach
