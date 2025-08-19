# CO2 Monitor on Raspberry Pi

Complete setup guide for running the CO2 Monitor web application on a Raspberry Pi.

## Hardware Requirements

- **Raspberry Pi** (3B+, 4B, or newer recommended)
- **Arduino board** (Uno, Nano, Mega, etc.)
- **Sensirion SCD41 CO2 sensor**
- **Green and Yellow LEDs** (optional)
- **MicroSD card** (16GB+ recommended)
- **Power supply** for Raspberry Pi

## Quick Setup

### 1. Transfer Files to Pi

Copy these files to your Raspberry Pi:
- `web_app.py`
- `templates/index.html`
- `setup_pi.sh`

### 2. Run Setup Script

```bash
chmod +x setup_pi.sh
./setup_pi.sh
```

### 3. Reboot

```bash
sudo reboot
```

## Manual Setup (Alternative)

If you prefer manual setup:

### 1. Install Dependencies

```bash
sudo apt update && sudo apt upgrade -y
sudo apt install -y python3 python3-pip python3-venv python3-serial
```

### 2. Add User to Dialout Group

```bash
sudo usermod -a -G dialout $USER
# Log out and back in, or reboot
```

### 3. Create Application Directory

```bash
mkdir -p /home/pi/co2_monitor
cd /home/pi/co2_monitor
```

### 4. Create Virtual Environment

```bash
python3 -m venv venv
source venv/bin/activate
pip install Flask pyserial plotly
```

### 5. Copy Application Files

```bash
# Copy web_app.py and templates/ folder here
```

## Serial Port Configuration

### Check Available Ports

```bash
ls -la /dev/tty*
```

Common ports on Pi:
- **`/dev/ttyUSB0`** - USB-to-serial adapters
- **`/dev/ttyACM0`** - Arduino via USB
- **`/dev/ttyAMA0`** - Hardware serial (GPIO 14/15)

### Update Port in web_app.py

Edit `web_app.py` and change:
```python
SERIAL_PORT = '/dev/ttyUSB0'  # or your actual port
```

## Auto-Start Service

The setup script creates a systemd service that:
- Starts automatically on boot
- Restarts if it crashes
- Logs to system journal

### Service Commands

```bash
# Check status
sudo systemctl status co2-monitor

# View logs
sudo journalctl -u co2-monitor -f

# Restart service
sudo systemctl restart co2-monitor

# Stop service
sudo systemctl stop co2-monitor

# Disable auto-start
sudo systemctl disable co2-monitor
```

## Accessing the Dashboard

### Find Your Pi's IP Address

```bash
hostname -I
# or
ip addr show
```

### Access from Browser

- **Local**: `http://localhost:5000`
- **Network**: `http://YOUR_PI_IP:5000`
- **Example**: `http://192.168.1.100:5000`

## Troubleshooting

### Serial Port Issues

1. **Check if Arduino is recognized:**
   ```bash
   lsusb
   ```

2. **Check port permissions:**
   ```bash
   ls -la /dev/ttyUSB0
   ```

3. **Test serial communication:**
   ```bash
   sudo apt install minicom
   minicom -D /dev/ttyUSB0 -b 115200
   ```

### Service Issues

1. **Check service logs:**
   ```bash
   sudo journalctl -u co2-monitor -f
   ```

2. **Check if port 5000 is available:**
   ```bash
   sudo netstat -tlnp | grep :5000
   ```

3. **Test manually:**
   ```bash
   cd /home/pi/co2_monitor
   source venv/bin/activate
   python web_app.py
   ```

### Database Issues

1. **Check database file:**
   ```bash
   ls -la /home/pi/co2_monitor/co2_data.db
   ```

2. **Check permissions:**
   ```bash
   sudo chown pi:pi /home/pi/co2_monitor/co2_data.db
   ```

## Performance Tips

### For Pi 3B+ or older:
- Consider reducing update frequency to 10 seconds
- Use lighter web interface (remove some animations)
- Monitor CPU usage: `htop`

### For Pi 4B or newer:
- Should handle the application easily
- Can increase update frequency if needed

## Network Configuration

### Static IP (Recommended)

Edit `/etc/dhcpcd.conf`:
```bash
sudo nano /etc/dhcpcd.conf
```

Add:
```
interface eth0
static ip_address=192.168.1.100/24
static routers=192.168.1.1
static domain_name_servers=8.8.8.8
```

### Firewall

```bash
sudo ufw allow 5000
sudo ufw enable
```

## Backup and Maintenance

### Backup Database

```bash
cp /home/pi/co2_monitor/co2_data.db /home/pi/backup_$(date +%Y%m%d).db
```

### Update Application

```bash
cd /home/pi/co2_monitor
sudo systemctl stop co2-monitor
# Copy new files
sudo systemctl start co2-monitor
```

## Monitoring and Logs

### View Real-time Logs

```bash
sudo journalctl -u co2-monitor -f
```

### Check System Resources

```bash
htop
df -h
free -h
```

## Security Considerations

- The web interface runs on port 5000 and is accessible from your local network
- Consider using a reverse proxy (nginx) for production use
- Add authentication if needed
- Use HTTPS for external access

## Support

If you encounter issues:
1. Check the service logs
2. Verify serial port configuration
3. Ensure all dependencies are installed
4. Check file permissions and ownership 