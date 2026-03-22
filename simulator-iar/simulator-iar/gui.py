import tkinter as tk
from tkinter import messagebox, ttk
import serial  # Ensure you have pyserial installed: pip install pyserial
import winsound
import logging
import threading
from datetime import datetime

class HomeControlApp:
    UART_COMMANDS = {
        "Lamp": "L",
        "Plug": "P",
        "Door": "D",
        "Temperature": "T",
    }

    def __init__(self, root):
        self.root = root
        self.root.title("Home Appliances Control")

        logging.basicConfig(level=logging.DEBUG)

        # Initialize UART communication
        self.ser = None
        self.init_uart()

        # Initialize GUI components
        self.setup_gui()

        # Alarm threshold for temperature
        self.alarm_threshold = 38
        self.alarm_triggered = False

        # Variables to log door status with timestamps
        self.door_log = []
        self.previous_door_status = None  # Track the previous door status

        # Start checking the status periodically
        self.check_status()

    def init_uart(self):
        """Initialize UART communication."""
        try:
            # Change 'COM8' to the correct COM port on your system
            self.ser = serial.Serial('COM8', baudrate=9600, timeout=1)
            self.ser.flushInput()  # Flush input to clear any residual data
            logging.info("UART initialized successfully")
        except serial.SerialException as e:
            messagebox.showerror("UART Error", f"Failed to open UART: {e}")
            logging.error(f"UART Error: {e}")
            self.ser = None

    def setup_gui(self):
        """Set up the GUI components."""
        self.option_var = tk.StringVar()
        self.option_var.set("Lamp")

        self.option_menu = ttk.Combobox(self.root, textvariable=self.option_var, values=list(self.UART_COMMANDS.keys()))
        self.option_menu.pack(pady=10)

        self.control_panel = tk.Frame(self.root)
        self.control_panel.pack(pady=10)

        self.lamp_status = tk.StringVar(value="Off")
        self.plug_status = tk.StringVar(value="Off")
        self.door_status = tk.StringVar(value="Closed")
        self.temperature = tk.DoubleVar(value=25.0)

        self.lamp_button = tk.Button(self.control_panel, text="Toggle Lamp", command=self.toggle_lamp)
        self.plug_button = tk.Button(self.control_panel, text="Toggle Plug", command=self.toggle_plug)
        self.door_label = tk.Label(self.control_panel, text="Door Status:")
        self.temp_label = tk.Label(self.control_panel, text="Room Temperature:")
        self.log_label = tk.Label(self.control_panel, text="Door Log:")
        self.log_text = tk.Text(self.control_panel, height=10, width=50)

        self.lamp_label = tk.Label(self.control_panel, textvariable=self.lamp_status)
        self.plug_label = tk.Label(self.control_panel, textvariable=self.plug_status)
        self.door_status_label = tk.Label(self.control_panel, textvariable=self.door_status)
        self.temp_value_label = tk.Label(self.control_panel, textvariable=self.temperature)

        self.update_gui()
        self.option_menu.bind("<<ComboboxSelected>>", self.update_gui)

    def update_gui(self, event=None):
        """Update the GUI based on the selected option."""
        selected_option = self.option_var.get()

        for widget in self.control_panel.winfo_children():
            widget.pack_forget()

        if selected_option == "Lamp":
            self.lamp_button.pack(pady=5)
            self.lamp_label.pack(pady=5)
        elif selected_option == "Plug":
            self.plug_button.pack(pady=5)
            self.plug_label.pack(pady=5)
        elif selected_option == "Door":
            self.door_label.pack(pady=5)
            self.door_status_label.pack(pady=5)
            self.log_label.pack(pady=5)
            self.log_text.pack(pady=5)
        elif selected_option == "Temperature":
            self.temp_label.pack(pady=5)
            self.temp_value_label.pack(pady=5)

        self.send_uart_command(self.UART_COMMANDS[selected_option])

    def send_uart_command(self, command):
        """Send UART command to the microcontroller."""
        if self.ser:
            try:
                logging.debug(f"Sending command: {command}")
                self.ser.write(command.encode())
            except serial.SerialException as e:
                logging.error(f"Error sending UART command: {e}")
        else:
            logging.error("UART not initialized")

    def toggle_lamp(self):
        """Toggle the lamp status between On and Off."""
        if self.lamp_status.get() == "Off":
            self.lamp_status.set("On")
            self.send_uart_command("C")
        else:
            self.lamp_status.set("Off")
            self.send_uart_command("E")

    def toggle_plug(self):
        """Toggle the plug status between On and Off."""
        if self.plug_status.get() == "Off":
            self.plug_status.set("On")
            self.send_uart_command("A")
        else:
            self.plug_status.set("Off")
            self.send_uart_command("B")

    def check_status(self):
        """Check the room temperature and door status."""
        try:
            while self.ser and self.ser.in_waiting > 0:
                line = self.ser.readline().decode('utf-8').strip()
                logging.debug(f"Data received: {line}")

                if line.startswith("TEMP:"):
                    temperature = float(line[5:])
                    self.temperature.set(temperature)
                    if temperature > self.alarm_threshold:
                        if not self.alarm_triggered:
                            self.alarm_triggered = True
                            threading.Thread(target=self.sound_alarm).start()
                    else:
                        self.alarm_triggered = False
                elif line.startswith("DOOR:"):
                    door_status = line[5:]
                    self.door_status.set(door_status)
                    self.log_door_status(door_status)
        except serial.SerialException as e:
            logging.error(f"Error reading UART: {e}")

        self.root.after(1000, self.check_status)

    def sound_alarm(self):
        """Play an alarm sound."""
        winsound.Beep(1000, 1000)
        self.alarm_triggered = False  # Reset the alarm trigger flag

    def log_door_status(self, status):
        """Log the door status with a timestamp if it changes."""
        if status != self.previous_door_status:
            current_time = datetime.now().strftime("%H:%M")
            log_entry = f"At {current_time}: Door {status.lower()}.\n"
            self.door_log.append(log_entry)
            self.update_log_text()
            self.previous_door_status = status  # Update the previous status

    def update_log_text(self):
        """Update the log text widget with the latest entries."""
        self.log_text.config(state=tk.NORMAL)
        self.log_text.delete('1.0', tk.END)
        self.log_text.insert(tk.END, ''.join(self.door_log))
        self.log_text.config(state=tk.DISABLED)

if __name__ == "__main__":
    root = tk.Tk()
    app = HomeControlApp(root)
    root.mainloop()