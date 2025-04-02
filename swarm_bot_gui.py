import tkinter as tk
from tkinter import messagebox

def send_command(command):
    # Placeholder function to send commands to bots
    messagebox.showinfo("Command Sent", f"Command: {command}")

def send_circle_command():
    radius = radius_entry.get()
    if radius.isdigit():
        send_command(f"CIRCLE:{radius}")
    else:
        messagebox.showerror("Input Error", "Please enter a valid radius")

# GUI Setup
root = tk.Tk()
root.title("Swarm Bot Control Interface")
root.geometry("400x300")

tk.Label(root, text="Swarm Bot Formation Control", font=("Arial", 14)).pack(pady=10)

btn_line = tk.Button(root, text="Straight Line", command=lambda: send_command("LINE"))
btn_line.pack(pady=5)

btn_triangle = tk.Button(root, text="Triangle", command=lambda: send_command("TRIANGLE"))
btn_triangle.pack(pady=5)

tk.Label(root, text="Circle Radius:").pack(pady=5)
radius_entry = tk.Entry(root)
radius_entry.pack(pady=5)

btn_circle = tk.Button(root, text="Circle", command=send_circle_command)
btn_circle.pack(pady=5)

root.mainloop()
