#!/usr/bin/env python3
"""
CO2 Monitor Web Application
Reads data from Arduino via serial port and displays real-time graphs
"""

import os
import time
import json
import sqlite3
from datetime import datetime, timedelta
import threading
import serial
from flask import Flask, render_template, jsonify, request
import plotly.graph_objs as go
import plotly.utils
from plotly.subplots import make_subplots

app = Flask(__name__)

# Configuration
SERIAL_PORT = '/dev/ttyACM0'
SERIAL_BAUDRATE = 115200
DATABASE_PATH = 'co2_data.db'
UPDATE_INTERVAL = 5  # seconds

# Global variables
latest_data = {
    'co2': 0,
    'temperature': 0.0,
    'humidity': 0.0,
    'timestamp': None
}

def init_database():
    """Initialize SQLite database"""
    conn = sqlite3.connect(DATABASE_PATH)
    cursor = conn.cursor()
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS measurements (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp DATETIME NOT NULL,
            co2 INTEGER NOT NULL,
            temperature REAL NOT NULL,
            humidity REAL NOT NULL
        )
    ''')
    conn.commit()
    conn.close()

def parse_serial_line(line):
    """Parse serial data line from Arduino"""
    try:
        # Expected format: "Time: Xs | CO2: XXX ppm | Temp: XX.XC | Humidity: XX.X%"
        if '|' in line and 'CO2:' in line:
            parts = line.split('|')
            if len(parts) >= 4:
                # Extract CO2
                co2_part = parts[1].strip()
                co2 = int(co2_part.split(':')[1].split()[0])
                
                # Extract temperature (remove the 'C' unit)
                temp_part = parts[2].strip()
                temp_str = temp_part.split(':')[1].split()[0]
                temp = float(temp_str.replace('C', ''))
                
                # Extract humidity (remove the '%' unit)
                humidity_part = parts[3].strip()
                humidity_str = humidity_part.split(':')[1].split()[0]
                humidity = float(humidity_str.replace('%', ''))
                
                return co2, temp, humidity
    except Exception as e:
        print(f"Error parsing line: {e}")
        print(f"Line: {line}")
    return None, None, None

def serial_reader():
    """Background thread to read serial data"""
    global latest_data
    
    while True:
        try:
            with serial.Serial(SERIAL_PORT, SERIAL_BAUDRATE, timeout=1) as ser:
                print(f"Connected to {SERIAL_PORT}")
                
                while True:
                    if ser.in_waiting:
                        line = ser.readline().decode('utf-8').strip()
                        if line:
                            co2, temp, humidity = parse_serial_line(line)
                            if co2 is not None:
                                timestamp = datetime.now()
                                latest_data = {
                                    'co2': co2,
                                    'temperature': temp,
                                    'humidity': humidity,
                                    'timestamp': timestamp
                                }
                                
                                # Save to database
                                conn = sqlite3.connect(DATABASE_PATH)
                                cursor = conn.cursor()
                                cursor.execute('''
                                    INSERT INTO measurements (timestamp, co2, temperature, humidity)
                                    VALUES (?, ?, ?, ?)
                                ''', (timestamp, co2, temp, humidity))
                                conn.commit()
                                conn.close()
                                
                                print(f"Saved: CO2={co2}, Temp={temp}, Humidity={humidity}")
                    
                    time.sleep(0.1)
                    
        except serial.SerialException as e:
            print(f"Serial error: {e}")
            time.sleep(5)
        except Exception as e:
            print(f"Unexpected error: {e}")
            time.sleep(5)

@app.route('/')
def index():
    """Main page"""
    return render_template('index.html')

@app.route('/api/latest')
def api_latest():
    """API endpoint for latest data"""
    return jsonify(latest_data)

@app.route('/api/history')
def api_history():
    """API endpoint for historical data"""
    hours = request.args.get('hours', 24, type=int)
    start_time = datetime.now() - timedelta(hours=hours)
    
    conn = sqlite3.connect(DATABASE_PATH)
    cursor = conn.cursor()
    cursor.execute('''
        SELECT timestamp, co2, temperature, humidity 
        FROM measurements 
        WHERE timestamp > ? 
        ORDER BY timestamp
    ''', (start_time,))
    
    data = cursor.fetchall()
    conn.close()
    
    timestamps = [row[0] for row in data]
    co2_values = [row[1] for row in data]
    temp_values = [row[2] for row in data]
    humidity_values = [row[3] for row in data]
    
    return jsonify({
        'timestamps': timestamps,
        'co2': co2_values,
        'temperature': temp_values,
        'humidity': humidity_values
    })

@app.route('/api/range')
def api_range():
    """API endpoint for data within a specific time range"""
    start_str = request.args.get('start')
    end_str = request.args.get('end')
    
    try:
        start_time = datetime.fromisoformat(start_str)
        end_time = datetime.fromisoformat(end_str)
    except:
        return jsonify({'error': 'Invalid date format'}), 400
    
    conn = sqlite3.connect(DATABASE_PATH)
    cursor = conn.cursor()
    cursor.execute('''
        SELECT timestamp, co2, temperature, humidity 
        FROM measurements 
        WHERE timestamp BETWEEN ? AND ? 
        ORDER BY timestamp
    ''', (start_time, end_time))
    
    data = cursor.fetchall()
    conn.close()
    
    timestamps = [row[0] for row in data]
    co2_values = [row[1] for row in data]
    temp_values = [row[2] for row in data]
    humidity_values = [row[3] for row in data]
    
    return jsonify({
        'timestamps': timestamps,
        'co2': co2_values,
        'temperature': temp_values,
        'humidity': humidity_values
    })

if __name__ == '__main__':
    # Initialize database
    init_database()
    
    # Start serial reader thread
    serial_thread = threading.Thread(target=serial_reader, daemon=True)
    serial_thread.start()
    
    print("Starting CO2 Monitor Web Application...")
    print(f"Serial port: {SERIAL_PORT}")
    print(f"Database: {DATABASE_PATH}")
    
    # Run Flask app
    app.run(host='0.0.0.0', port=5000, debug=False) 