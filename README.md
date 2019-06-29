# rpi-nrf24-master

Menyambung embedded linux device (raspberry pi) ke jaringan nrf24 Mesh

# direktori

certs -> berisi sertifkat untuk koneksi mqtt dengan mosquitto

src -> berisi kode dari program

conf -> contoh konfigurasi file yang bisa digunakan untuk mosquitto dan program.

# Librari:

1. https://github.com/nRF24/RF24

2. https://github.com/nRF24/RF24Network

3. https://github.com/nRF24/RF24Mesh

4. libmosquitto-dev

5. libcrypto-dev

6. libssl-dev

7. libboost-system-dev

# konfigurasi nrf24:

nrf24client id: 1

channel: 60

datarate: RF24_1MBPS

# konfigurasi GPIO stm32f103

pinOutput (bisa untuk relay) = {PB13, PB14, PB15, PA8, PA15, PB3, PB4, PB5}

pinInput (bisa untuk switch) = {PB11, PB10, PB1, PA3, PA2, PA1, PA0, PC13}

# koneksi modul nrf24 - stm32f103

1 GND - GND

2 VCC - 3V

3 CE - PB0

4 CSN - PA4

5 CLK - PA5

6 MOSI - PA7

7 MISO - PA6

8 IRQ - Unconnected

# koneksi modul nrf24 - rpi 

(ref: https://tmrh20.github.io/RF24/)

No | Pin | RPI         | RPi -P1 Connector

1  | GND  |	rpi-gnd	   | (25)

2  | VCC  |	rpi-3v3	   | (17)

3  | CE	  | rpi-gpio22 | (15)

4  | CSN  |	rpi-gpio8  | (24)

5  | SCK  |	rpi-sckl   | (23)

6  | MOSI |	rpi-mosi   | (19)

7  | MISO |	rpi-miso   | (21)

8  | IRQ  |	-          | -

# Serial Command (stm32f103):

g -> melihat id

i,<id> -> mengatur nrf24 klien id, 

misal: i,4 -> mengatur klien id menjadi 4

s,<id>,<pin>,<value> -> mengatur output pin dari nrf24 mesh dengan klien id = <id>, pin = <pin> dan output = <value>

misal: s,1,28,0 -> mengatur output pin dari klien nrf24 dengan id = 1, pin = 28, dan output = 0

# mqtt command:

Untuk mengirimkan command dari mqtt ke nrf24 klien bisa melalui topik: homebridge/from/set/<wrf_id>

dengan format perintah: <wrf_id>,<nrf24_id>,<pin>,<value>

misal: rf0,1,28,0 -> akan mengirimkan perintah untuk mengubah output pin dari mqtt klien = rf0, nrf24 mesh klien id = 1, pin = 28, dan output = 0
