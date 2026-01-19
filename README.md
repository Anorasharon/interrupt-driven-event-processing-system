# Interrupt Driven Event Processing System (ESP32 + FreeRTOS)

## 📌 Overview
This project implements an **interrupt-driven event processing system** using ESP32 and FreeRTOS.
A PIR motion sensor triggers a GPIO interrupt, which is handled by an ISR and safely communicated to a task using a FreeRTOS queue.
The task processes events using a state machine and activates a buzzer with cooldown protection.

This project demonstrates **real-time embedded system design best practices**.

---

## 🧠 Core Concepts Demonstrated
- GPIO Interrupt Handling (ISR)
- ISR-to-Task Communication
- FreeRTOS Queues
- Task Scheduling & Priorities
- State Machine Design
- Cooldown / Debounce Logic
- ESP-IDF Logging

---

## 🛠 Hardware Used
- ESP32
- PIR Motion Sensor
- Active Buzzer

---

## ⚙️ GPIO Configuration

| Component | GPIO |
|---------|------|
| PIR Sensor | GPIO 22 |
| Buzzer | GPIO 23 |

---

## 🔄 System Flow
1. PIR sensor detects motion
2. GPIO interrupt triggers ISR
3. ISR sends `EVENT_MOTION` to FreeRTOS queue
4. Event task receives and validates event
5. State machine transitions to ALERT state
6. Buzzer activates for 2 seconds
7. System returns to IDLE state

---

## 🧪 Build & Flash Instructions

```bash
idf.py build
idf.py flash monitor
