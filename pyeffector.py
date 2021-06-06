import tkinter
from tkinter.constants import ANCHOR
import tkinter.ttk as ttk
import sounddevice as sd

def PressPlayButton(event):
    pass

def PressStopButton(event):
    pass

if __name__ == "__main__":
    root = tkinter.Tk()
    root.title(u"pyeffector gui")
    root.geometry("800x600")

    frame = ttk.Frame(root, padding=10)
    frame.grid()

    padx = 10
    pady = 2

    list_devices = []
    for device in sd.query_devices():
        list_devices.append(device["name"])

    for hostapi in sd.query_hostapis():
        print(hostapi)

    input_combo_string = tkinter.StringVar()
    output_combo_string = tkinter.StringVar()
    
    play_button = tkinter.Button(frame, text=u"Play", width=20)
    play_button.grid(row=1, column=1, padx=padx, pady=pady)
    play_button.bind("<Button-1>", PressPlayButton)

    stop_button = tkinter.Button(frame, text=u"Stop", width=20)
    stop_button.grid(row=1, column=2, padx=padx, pady=pady)
    stop_button.bind("<Button-1>", PressStopButton)

    input_device_label = tkinter.Label(frame, text=u"Input Device")
    input_device_label.grid(row=2, column=1, padx=padx, pady=pady)
    
    input_device_combo = ttk.Combobox(frame, textvariable=input_combo_string, values=list_devices, width=40)
    input_device_combo.grid(row=2, column=2, columnspan=2, padx=padx, pady=pady)

    output_device_label = tkinter.Label(frame, text=u"Output Device")
    output_device_label.grid(row=3, column=1, padx=padx, pady=pady)

    output_device_combo = ttk.Combobox(frame, textvariable=output_combo_string, values=list_devices, width=40)
    output_device_combo.grid(row=3, column=2, columnspan=2, padx=padx, pady=pady)

    sampling_rate_label = tkinter.Label(frame, text=u"Sampling Rate")
    sampling_rate_label.grid(row=4, column=1, padx=padx, pady=pady)

    sampling_rate_entry = tkinter.Entry(frame, width=20)
    sampling_rate_entry.grid(row=4, column=2, padx=padx, pady=pady)
    sampling_rate_entry.insert(tkinter.END, u"44100")
    
    use_input_channel_label = tkinter.Label(frame, text=u"Use Input Channel")
    use_input_channel_label.grid(row=5, column=1, padx=padx, pady=pady)

    use_input_entry = tkinter.Entry(frame, width=20)
    use_input_entry.grid(row=5, column=2, padx=padx, pady=pady)
    use_input_entry.insert(tkinter.END, u"2")

    bitrate_label = tkinter.Label(frame, text=u"Bit Rate")
    bitrate_label.grid(row=6, column=1, padx=padx, pady=pady)

    bitrate_entry = tkinter.Entry(frame, width=20)
    bitrate_entry.grid(row=6, column=2, padx=padx, pady=pady)
    bitrate_entry.insert(tkinter.END, u"16")

    root.mainloop()