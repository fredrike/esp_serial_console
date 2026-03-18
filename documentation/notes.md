# Changing USB Serial Device ID

## Important: Multiple USB Serial Devices

**If you have multiple USB serial devices connected to the same system** (e.g., SkyConnect ZigBee adapter, other ESP32 devices), you may need to change the USB device identifiers to prevent conflicts and ensure devices get consistent port assignments.

### The Problem

The major issue occurs when multiple CP210x devices (like SkyConnect) connect to the same system. Without unique identifiers, the Linux kernel may assign different `/dev/ttyUSBx` ports on each boot, causing devices to swap positions:

```sh
[210963.198095] usb 1-1: cp210x converter now attached to ttyUSB0
```

### The Solution: Program Unique USB IDs

Use the `cp210x-program` tool to set unique vendor ID, product ID, and serial numbers for each device. This allows udev rules to create persistent device names.

**Example Configuration:**

**For ESP32 Serial Terminal:**

```sh
./cp210x-program -m 001/007 -w \
  --set-product-string="Frippes Serial Terminal" \
  --set-vendor-id="0x0001" \
  --set-product-id="0x0abc" \
  --set-serial-number="0xabcabc"
```

**For SkyConnect (ZigBee adapter):**

```sh
./cp210x-program -r -m 001/004 -w \
  --set-vendor-id="0x10c4" \
  --set-product-id="0xea60" \
  --set-serial-number="022ED9BO"
```

### Verifying the Changes

After programming, check the USB device info:

```sh
$ dmesg | tail
[    1.640366] usb 1-1: New USB device found, idVendor=0001, idProduct=0abc, bcdDevice= 1.00
[    1.640382] usb 1-1: New USB device strings: Mfr=1, Product=2, SerialNumber=3
[    1.640389] usb 1-1: Product: Frippes Serial Terminal
[    1.640394] usb 1-1: Manufacturer: Silicon Labs
[    1.640399] usb 1-1: SerialNumber: 0xabcabc
```

### Setting Up udev Rules for Persistent Device Names

Create udev rules to map devices to consistent names:

```sh
# /etc/udev/rules.d/99-usb-serial.rules
SUBSYSTEM=="tty", ATTRS{idVendor}=="0001", ATTRS{idProduct}=="0abc", SYMLINK+="esp32-terminal"
SUBSYSTEM=="tty", ATTRS{serial}=="c42779718a91ed11bdf9ccd13b20a988", SYMLINK+="skyconnect"
```

Reload udev rules:

```sh
sudo udevadm control --reload-rules
sudo udevadm trigger
```

Verify the persistent names:

```sh
$ ls -l /dev/skyconnect /dev/esp32-terminal
lrwxrwxrwx 1 root root 7 Dec  1 19:29 /dev/skyconnect -> ttyUSB1
lrwxrwxrwx 1 root root 7 Dec  1 19:29 /dev/esp32-terminal -> ttyUSB0
```

### Getting Device Info

To find the bus/device numbers for programming:

```sh
$ lsusb
Bus 001 Device 007: ID 0001:0abc Silicon Labs Frippes Serial Terminal
Bus 001 Device 004: ID 10c4:ea60 Nabu Casa SkyConnect v1.0

$ udevadm info -a -n /dev/ttyUSB0
```

---

## Using Docker to Program USB Devices

If you need to program USB devices from a Docker container (useful for systems like TrueNAS):

```sh
# Run privileged container with USB access
sudo docker run --rm -it --privileged \
  -v /dev/bus/usb:/dev/bus/usb \
  --entrypoint=/bin/sh python:3.12

# Inside container:
apt update && apt install python3-usb
pip install kw-cp210x-program
git clone https://github.com/VCTLabs/cp210x-program.git
cd cp210x-program

# Program the devices (replace bus/device numbers as needed)
./cp210x-program -r -m 001/002 -w \
  --set-vendor-id="0x0001" \
  --set-product-id="0xea60" \
  --set-serial-number="0xabcabc"

./cp210x-program -r -m 001/004 -w \
  --set-vendor-id="0x10c4" \
  --set-product-id="0xea60" \
  --set-serial-number="022ED9BO"
```

```sh
./cp210x-program -r -m 001/002
USB find_device returned:
DEVICE ID 0001:0abc on Bus 001 Address 002 =================
 bLength                :   0x12 (18 bytes)
 bDescriptorType        :    0x1 Device
 bcdUSB                 :  0x200 USB 2.0
 bDeviceClass           :    0x0 Specified at interface
 bDeviceSubClass        :    0x0
 bDeviceProtocol        :    0x0
 bMaxPacketSize0        :   0x40 (64 bytes)
 idVendor               : 0x0001
 idProduct              : 0x0abc
 bcdDevice              :  0x100 Device 1.0
 iManufacturer          :    0x1 Silicon Labs
 iProduct               :    0x2 Frippes Serial Terminal
 iSerialNumber          :    0x3 0xabcabc
 bNumConfigurations     :    0x1
  CONFIGURATION 1: 100 mA ==================================
   bLength              :    0x9 (9 bytes)
   bDescriptorType      :    0x2 Configuration
   wTotalLength         :   0x20 (32 bytes)
   bNumInterfaces       :    0x1
   bConfigurationValue  :    0x1
   iConfiguration       :    0x0
   bmAttributes         :   0x80 Bus Powered
   bMaxPower            :   0x32 (100 mA)
    INTERFACE 0: Vendor Specific ===========================
     bLength            :    0x9 (9 bytes)
     bDescriptorType    :    0x4 Interface
     bInterfaceNumber   :    0x0
     bAlternateSetting  :    0x0
     bNumEndpoints      :    0x2
     bInterfaceClass    :   0xff Vendor Specific
     bInterfaceSubClass :    0x0
     bInterfaceProtocol :    0x0
     iInterface         :    0x2 Frippes Serial Terminal
      ENDPOINT 0x1: Bulk OUT ===============================
       bLength          :    0x7 (7 bytes)
       bDescriptorType  :    0x5 Endpoint
       bEndpointAddress :    0x1 OUT
       bmAttributes     :    0x2 Bulk
       wMaxPacketSize   :   0x40 (64 bytes)
       bInterval        :    0x0
      ENDPOINT 0x82: Bulk IN ===============================
       bLength          :    0x7 (7 bytes)
       bDescriptorType  :    0x5 Endpoint
       bEndpointAddress :   0x82 IN
       bmAttributes     :    0x2 Bulk
       wMaxPacketSize   :   0x40 (64 bytes)
       bInterval        :    0x0
```

## Setup serial-getty

```sh
cat  /etc/udev/rules.d/99-usb-serial.rules
  SUBSYSTEM=="tty", ATTRS{serial}=="022ED9BO", SYMLINK+="ttyESP32"

sudo udevadm control --reload-rules; sudo udevadm trigger

fer@truenas:~$ ls -l /dev/ttyESP32
lrwxrwxrwx 1 root root 7 Dec  1 21:27 /dev/ttyESP32 -> ttyUSB1

sudo systemctl enable serial-getty@ttyESP32.service
sudo systemctl start serial-getty@ttyESP32.service
```

---

## References

- [cp210x-program tool](https://github.com/VCTLabs/cp210x-program) - USB ID programming utility
- [udev rules documentation](https://www.freedesktop.org/software/systemd/man/udev.html) - Linux device management
