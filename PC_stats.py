import time
import psutil
import serial
import subprocess
import os
from datetime import datetime

# --- ANONYMIZED HARDWARE PROFILE --- CHANGE!!!! ---
CPU_NAME = "AMD RYZEN PROCESSOR"
GPU1_NAME = "INTEGRATED GRAPHICS"
GPU2_NAME = "DEDICATED GPU"
MOCK_IP = "192.168.1.100"
MOCK_MAC = "AA:BB:CC:DD:EE:FF"

def get_nvidia_telemetry():
    """Safely fetch GPU metrics using native system queries."""
    try:
        cmd = ["nvidia-smi", "--query-gpu=utilization.gpu,temperature.gpu", "--format=csv,noheader,nounits"]
        output = subprocess.check_output(cmd, startupinfo=subprocess.STARTUPINFO()).decode('utf-8').strip()
        usage, temp = map(int, output.split(','))
        return usage, temp
    except Exception:
        return 0, 0 

def get_network_bytes():
    """Fetch global interface network counters."""
    try:
        return psutil.net_io_counters().bytes_recv
    except Exception:
        return 0

# --- ESTABLISH CONNECTION ---
COM_PORT = 'COM3' # Users will update this to match their own device port

try:
    ser = serial.Serial(COM_PORT, 115200, timeout=1)
    time.sleep(3) # Await hardware handshake
    
    # Send static baseline profile configuration
    info_payload = f"INFO:{CPU_NAME}|{GPU1_NAME}|{GPU2_NAME}|{MOCK_IP}|{MOCK_MAC}\n"
    ser.write(info_payload.encode('utf-8'))
    print(f"Connected to display engine on {COM_PORT}.")
    time.sleep(0.5)
except Exception as e:
    print(f"Connection error: {e}")
    exit()

old_bytes_recv = get_network_bytes()

try:
    while True:
        cpu = int(psutil.cpu_percent(interval=None))
        ram = int(psutil.virtual_memory().percent)
        
        # Monitor primary active partitions dynamically
        if os.path.exists("D:"):
            disk = int(psutil.disk_usage('D:').percent)
            disk_mode = 2 
        else:
            disk = int(psutil.disk_usage('C:').percent)
            disk_mode = 1 
        
        bat_info = psutil.sensors_battery()
        battery = int(bat_info.percent) if bat_info else 100
        gpu_usage, gpu_temp = get_nvidia_telemetry()
            
        # Calculate approximate throughput network speeds
        new_bytes_recv = get_network_bytes()
        bytes_diff = new_bytes_recv - old_bytes_recv
        old_bytes_recv = new_bytes_recv
        
        net_speed_mbps = (bytes_diff * 8) / (1024 * 1024)
        net_val = int(min(net_speed_mbps, 99))
        if net_val < 0: net_val = 0
        
        current_time = datetime.now().strftime("%H:%M:%S")
        
        # Assemble standard structured data packet stream
        payload = f"{cpu},{ram},{disk},{gpu_usage},{gpu_temp},{net_val},{battery},{current_time},{disk_mode}\n"
        ser.write(payload.encode('utf-8'))
        
        time.sleep(1)
        
except KeyboardInterrupt:
    ser.close()
    print("\nStream halted safely.")
