# rpi-nrf24-master
Menyambung embedded linux device (raspberry pi) ke jaringan nrf24 Mesh
# ================
konfigurasi nrf24:
# ================
channel: 60
datarate: RF24_1MBPS
# ================
konfigurasi GPIO stm32f103
# ================
pinOutput (bisa untuk relay) = {PB13, PB14, PB15, PA8, PA15, PB3, PB4, PB5}
pinInput (bisa untuk switch) = {PB11, PB10, PB1, PA3, PA2, PA1, PA0, PC13}
# ================
koneksi modul nrf24 - stm32f103
# ================
1 GND - GND
2 VCC - 3V
3 CE - PB0
4 CSN - PA4
5 CLK - PA5
6 MOSI - PA7
7 MISO - PA6
8 IRQ - Unconnected
# ================
koneksi modul nrf24 - rpi (ref: https://tmrh20.github.io/RF24/)
# ================
No | Pin | RPI         | RPi -P1 Connector
1  | GND  |	rpi-gnd	   | (25)
2  | VCC  |	rpi-3v3	   | (17)
3  | CE	  | rpi-gpio22 | (15)
4  | CSN  |	rpi-gpio8  | (24)
5  | SCK  |	rpi-sckl   | (23)
6  | MOSI |	rpi-mosi   | (19)
7  | MISO |	rpi-miso   | (21)
8  | IRQ  |	-          | -