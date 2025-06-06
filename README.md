# MQTT-AT-Commands-for-ESP8266-
The article describes the use of ESP 8266 with ESPRESSIF AT firmware to communicate with MQTT Broker.

I. To communicate with MQTT via AT firmware, you need to prepare the following hardware and software:
  1. Hardware
     - Chip Esp8266EX, flash > 2MB (16Mbit)
     - USB to TTL (can use usb cp2102 or CH340)
  2. Software
     - Firmware AT command of Espressif version >= 2.1xxx
     - Flash dowmload tool Esp version >= 3.8xxx
     - Terminal program (Hercules, putty,...)
    
  
II. Implementation steps
  1. Download firmware AT for esp 8266 at https://docs.espressif.com/projects/esp-at/en/release-v2.2.0.0_esp8266/AT_Binary_Lists/ESP8266_AT_binaries.html
     ![image](https://github.com/user-attachments/assets/879355ac-7d44-4ea0-b87d-83d0e6ec5ec6)

  2. Extral file
     ![image](https://github.com/user-attachments/assets/ce2b6bab-dc1c-43dd-bbb0-2427691db349)

  3. Open flash dowmload tool
     
     ![image](https://github.com/user-attachments/assets/23a502ec-039d-4568-a5cf-c94b3b954289)
     
     ![image](https://github.com/user-attachments/assets/e7dc1ec2-e546-4e14-abba-54c8a7d5f39d)
     
     ![image](https://github.com/user-attachments/assets/d9098abf-d34f-485e-95f5-d9c2815aed2c)
     
     ![image](https://github.com/user-attachments/assets/9872db8b-87e4-40f6-96a0-d1901eb9c350)
     
     ![image](https://github.com/user-attachments/assets/26971ec8-33f7-475b-b122-885e8eb72ed1)

  4. Connect Esp with USB TTL
     File selection process according to file configuration
     => Flash firmware
     

     

     
