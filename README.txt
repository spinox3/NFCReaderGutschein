# NFC Reader Voucher System

## Overview
This project implements an ESP32-based NFC free schoolday voucher system that simulates a real NFC chip containing voucher data.

## Components

### Hardware
- **ESP32 Microcontroller** - Main processor
- **NFC Reader Module** - Reads/writes NFC tags
- **NFC Contact** - Passive NFC chip for voucher storage

### Features

#### ESP32 NFC Module
- Reads NFC chip data
- Writes voucher information to NFC tags
- Stores voucher data in JSON format
- Hosts a React web server for visual representation of necessary information (e.g how many vouchers left)
