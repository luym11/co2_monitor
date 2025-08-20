#!/bin/bash

echo "Setting up CO2 Monitor on Raspberry Pi..."

# Update system
echo "Updating system packages..."
sudo apt update && sudo apt upgrade -y

# Install Python and pip
echo "Installing Python and pip..."
sudo apt install -y python3 python3-pip python3-venv

# Install system dependencies for serial communication
echo "Installing serial communication dependencies..."
sudo apt install -y python3-serial

# Add user to dialout group for serial port access
echo "Adding user to dialout group..."
sudo usermod -a -G dialout $USER

# Create application directory
echo "Creating application directory..."
mkdir -p /home/pi/co2_monitor
cd /home/pi/co2_monitor

# Create virtual environment
echo "Creating Python virtual environment..."
python3 -m venv venv
source venv/bin/activate

# Install Python packages
echo "Installing Python packages..."
pip install Flask pyserial plotly

# Create systemd service for auto-start
echo "Creating systemd service..."
sudo tee /etc/systemd/system/co2-monitor.service > /dev/null <<EOF
[Unit]
Description=CO2 Monitor Web Application
After=network.target

[Service]
Type=simple
User=pi
WorkingDirectory=/home/pi/co2_monitor
Environment=PATH=/home/pi/co2_monitor/venv/bin
ExecStart=/home/pi/co2_monitor/venv/bin/python web_app.py
Restart=always
RestartSec=10

[Install]
WantedBy=multi-user.target
EOF

# Enable and start service
echo "Enabling and starting service..."
sudo systemctl daemon-reload
sudo systemctl enable co2-monitor
sudo systemctl start co2-monitor

# Check service status
echo "Checking service status..."
sudo systemctl status co2-monitor

echo ""
echo "Setup complete! The CO2 Monitor will now start automatically on boot."
echo ""
echo "To check the service status: sudo systemctl status co2-monitor"
echo "To view logs: sudo journalctl -u co2-monitor -f"
echo "To restart: sudo systemctl restart co2-monitor"
echo "To stop: sudo systemctl stop co2-monitor"
echo ""
echo "The web interface will be available at: http://YOUR_PI_IP:5000"
echo ""
echo "IMPORTANT: Log out and log back in for the dialout group changes to take effect!"
echo "Or reboot: sudo reboot" 