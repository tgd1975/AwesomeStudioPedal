#!/usr/bin/env python3
"""
Simple serial monitor for ESP32/Arduino devices
Usage: python3 serial_monitor.py /dev/ttyUSB0 115200
"""

import sys
import serial
import serial.tools.list_ports

def list_ports():
    """List available serial ports"""
    print("Available serial ports:")
    ports = serial.tools.list_ports.comports()
    for port in ports:
        print(f"  {port.device}: {port.description}")
    return [port.device for port in ports]

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 serial_monitor.py /dev/ttyUSB0 [baudrate]")
        print("\nAvailable ports:")
        list_ports()
        sys.exit(1)
    
    port = sys.argv[1]
    baudrate = int(sys.argv[2]) if len(sys.argv) > 2 else 115200
    
    try:
        with serial.Serial(port, baudrate, timeout=0.1) as ser:
            print(f"Connected to {port} at {baudrate} baud")
            print("Press Ctrl+C to exit")
            
            while True:
                if ser.in_waiting:
                    line = ser.readline()
                    try:
                        print(line.decode('utf-8', errors='ignore').strip())
                    except:
                        print(f"Binary data: {line.hex()}")
    
    except serial.SerialException as e:
        print(f"Serial error: {e}")
        sys.exit(1)
    except KeyboardInterrupt:
        print("\nExiting...")
        sys.exit(0)

if __name__ == "__main__":
    main()