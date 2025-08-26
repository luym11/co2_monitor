# CO2 Monitor Web Application

A real-time web dashboard for monitoring CO2, temperature, and humidity from a Sensirion SCD41 sensor connected to an Arduino.

## Features

- 🌬️ **Real-time monitoring** of CO2, temperature, and humidity
- 📊 **Interactive graphs** using Plotly.js
- 💾 **Data storage** in SQLite database
- 📱 **Responsive design** that works on all devices
- 🔍 **Historical data** viewing for any time period
- 🌐 **Network accessible** - view from any device on your network

## Hardware Requirements

- Arduino board (Uno, Nano, Mega, etc.)
- Sensirion SCD41 CO2 sensor
- Green and Yellow LEDs (optional, for visual indicators)
- USB cable to connect Arduino to computer

## Wiring

### SCD41 Sensor
- **VDD** → Arduino **5V**
- **GND** → Arduino **GND**
- **SDA** → Arduino **A4**
- **SCL** → Arduino **A5**

### LEDs (Optional)
- **Green LED** → Arduino **Pin 12** (with resistor)
- **Yellow LED** → Arduino **Pin 11** (with resistor)
- **LED cathodes** → **GND**

## Software Setup

### 1. Arduino Code
Upload the `src/main.cpp` file to your Arduino using PlatformIO or Arduino IDE.

### 2. Python Dependencies
Install the required Python packages:

```bash
pip install -r requirements.txt
```

Or install manually:
```bash
pip install Flask pyserial plotly
```

### 3. Run the Web Application

```bash
python web_app.py
```

The application will:
- Connect to `/dev/ttyACM0` at 115200 baud
- Start a web server on port 5000
- Begin collecting data from the Arduino

### 4. Access the Dashboard

Open your web browser and navigate to:
- **Local access**: `http://localhost:5000`
- **Network access**: `http://YOUR_COMPUTER_IP:5000`

## Usage

### Real-time Monitoring
- Current values are displayed in large cards at the top
- Data updates every 5 seconds automatically
- Connection status is shown with a green/red indicator

### Historical Data
- **Quick ranges**: Select from predefined time periods (1 hour to 1 week)
- **Custom range**: Choose specific start and end dates/times
- **Graphs**: View trends over time for all three measurements

### Data Storage
- All measurements are automatically saved to `co2_data.db`
- Data persists between application restarts
- SQLite database for easy backup and analysis

## API Endpoints

- `GET /` - Main dashboard page
- `GET /api/latest` - Latest measurement data
- `GET /api/history?hours=N` - Historical data for last N hours
- `GET /api/range?start=ISO&end=ISO` - Data within custom time range

## Configuration

Edit `web_app.py` to modify:
- Serial port (`SERIAL_PORT`)
- Baud rate (`SERIAL_BAUDRATE`)
- Database path (`DATABASE_PATH`)
- Web server port (in `app.run()`)

## Troubleshooting

### Serial Port Issues
- Ensure Arduino is connected and recognized
- Check if `/dev/ttyACM0` exists: `ls -la /dev/ttyACM*`
- Verify no other application is using the port
- Try different port names if needed

### Permission Issues
If you get permission errors accessing the serial port:
```bash
# See current perms & owning group (usually 'dialout' or 'uucp')
ls -l /dev/ttyACM0
# See which groups you’re in
groups

# Add yourself to the device’s group (Ubuntu/Debian typically 'dialout')
sudo usermod -aG dialout "$USER"

# Apply without full logout (either option works)
newgrp dialout   # opens a subshell with updated groups
# or log out and back in, then verify:
groups

```

### Data Not Appearing
- Check Arduino serial monitor for data output
- Verify the data format matches what the parser expects
- Check application logs for error messages

## Data Format

The Arduino should output data in this format:
```
Time: 5s | CO2: 412 ppm | Temp: 23.5C | Humidity: 45.2%
```

## Network Access

To access from other devices on your network:
1. Find your computer's IP address: `ip addr show` or `ifconfig`
2. Access via `http://YOUR_IP:5000`
3. Ensure your firewall allows connections on port 5000

## Security Note

This application runs on `0.0.0.0:5000` by default, making it accessible from your local network. For production use, consider:
- Adding authentication
- Using HTTPS
- Restricting network access
- Running behind a reverse proxy

## License

This project is open source. Feel free to modify and distribute as needed. 