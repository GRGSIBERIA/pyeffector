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

    combo_string = tkinter.StringVar()
    
    play_button = tkinter.Button(text=u"Play", width=20)
    play_button.grid(row=1, column=1, padx=padx, pady=pady)
    play_button.bind("<Button-1>", PressPlayButton)

    stop_button = tkinter.Button(text=u"Stop", width=20)
    stop_button.grid(row=1, column=2, padx=padx, pady=pady)
    stop_button.bind("<Button-1>", PressStopButton)

    input_device_combo = ttk.Combobox(root, textvariable=combo_string, values=list_devices, width=40)
    input_device_combo.grid(row=2, column=1, columnspan=2, padx=padx, pady=pady)

    sampling_rate_label = tkinter.Label(text=u"Sampling Rate")
    sampling_rate_label.grid(row=3, column=1, padx=padx, pady=pady)

    sampling_rate_entry = tkinter.Entry(width=20)
    sampling_rate_entry.grid(row=3, column=2, padx=padx, pady=pady)
    sampling_rate_entry.insert(tkinter.END, u"44100")
    
    use_input_channel_label = tkinter.Label(text=u"Use input channel")
    use_input_channel_label.grid(row=4, column=1, padx=padx, pady=pady)

    use_input_entry = tkinter.Entry(width=20)
    use_input_entry.grid(row=4, column=2, padx=padx, pady=pady)
    use_input_entry.insert(tkinter.END, u"2")

    root.mainloop()