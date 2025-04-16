# 🔲 Bluetooth-Controlled LED Matrix Display

A microcontroller-based system that displays animations and scrolling text on an 8x32 LED matrix via Bluetooth using an ESP32 microcontroller. The project includes custom animations like Pac-Man, heartbeats, and waveform visualizations, which can be triggered via a Unity-based mobile application.

---

## 🧠 Features

- 15+ unique animations: Pac-Man, heart pulse, sinus wave, arrows, spectrum, etc.  
- Bluetooth communication between Unity app and ESP32  
- Real-time scrolling text display  
- Serial command parsing from mobile input  
- State-based animation controller  
- Matrix clearing, brightness control, and directional effects  
- Fully modular animation system

---

## 🛠️ Tech Stack

- **ESP32** (Bluetooth-enabled microcontroller)  
- **Arduino IDE** for firmware development  
- **MD_MAX72XX** library for LED matrix control  
- **Unity (planned/app side)** for mobile animation selection interface  
- **8x32 LED Matrix** (4 daisy-chained 8x8 modules)

  
## 🔧 Hardware Requirements

- ESP32 Dev Board  
- 4 x 8x8 LED Matrix (MAX7219-based)  
- Breadboard, jumper wires  
- Optional: Bluetooth-enabled phone (for mobile interface)

---
