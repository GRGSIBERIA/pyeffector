import tkinter
from tkinter.constants import ANCHOR
import tkinter.ttk as ttk
import sounddevice as sd

class MainWindow:
    def PressPlayButton(self, event):
        print("press play")

    def PressStopButton(self, event):
        pass

    def PressQueryButton(self, event):
        outname = self.rid["output device"].get()
        inname = self.rid["input device"].get()
        
        if inname != '':
            info = sd.query_devices(inname)
            self.var["max input channel"].set(info["max_input_channels"])
            self.var["input low input latency"].set(info["default_low_input_latency"])
            self.var["input high input latency"].set(info["default_high_input_latency"])
            self.var["input sampling rate"].set(info["default_samplerate"])

        if outname != '':
            info = sd.query_devices(outname)
            self.var["max output channel"].set(info["max_output_channels"])
            self.var["output low output latency"].set(info["default_low_output_latency"])
            self.var["output high output latency"].set(info["default_high_output_latency"])
            self.var["output sampling rate"].set(info["default_samplerate"])

    def ArrangeDeviceFrame(self):
        frame = ttk.Frame(self.root, padding=10)
        frame.grid()

        padx = 10
        pady = 2

        self.device_info = {}
        list_devices = []
        for device in sd.query_devices():
            list_devices.append(device["name"])
            self.device_info[device["name"]] = device

        input_combo_string = tkinter.StringVar()
        output_combo_string = tkinter.StringVar()
        kwargs = {"padx": padx, "pady": pady, "row": 1}
        
        #play_button = tkinter.Button(frame, text=u"Play", width=20)
        #play_button.grid(column=1, **kwargs)
        #play_button.bind("<Button-1>", self.PressPlayButton)

        #stop_button = tkinter.Button(frame, text=u"Stop", width=20)
        #stop_button.grid(column=2, **kwargs)
        #stop_button.bind("<Button-1>", self.PressStopButton)
        #kwargs["row"] += 1

        tkinter.Label(frame, text=u"Input Device").grid(column=1, **kwargs)
        kwargs["row"] += 1

        input_device_combo = ttk.Combobox(frame, textvariable=input_combo_string, values=list_devices, width=40)
        input_device_combo.grid(column=1, columnspan=2, **kwargs)
        self.rid["input device"] = input_device_combo
        kwargs["row"] += 1

        tkinter.Label(frame, text=u"Output Device").grid(column=1, **kwargs)
        kwargs["row"] += 1

        output_device_combo = ttk.Combobox(frame, textvariable=output_combo_string, values=list_devices, width=40)
        output_device_combo.grid(column=1, columnspan=2, **kwargs)
        self.rid["output device"] = output_device_combo
        kwargs["row"] += 1

        query_button = tkinter.Button(frame, text=u"Query", width=20)
        query_button.grid(column=1, **kwargs)
        query_button.bind("<Button-1>", self.PressQueryButton)
    
    def DefineInputOutputLabel(self, frame, text: str, inname: str, outname: str, kwargs: hash):
        tkinter.Label(frame, text=text).grid(column=1, **kwargs)
        name = inname
        self.var[name] = tkinter.StringVar()
        self.rid[name] = tkinter.Label(frame, textvariable=self.var[name])
        self.rid[name].grid(column=2, **kwargs)
        name = outname
        self.var[name] = tkinter.StringVar()
        self.rid[name] = tkinter.Label(frame, textvariable=self.var[name])
        self.rid[name].grid(column=3, **kwargs)
        kwargs["row"] += 1

    def ArrangeDeviceInfoFrame(self):
        frame = ttk.Frame(self.root, padding=10)
        frame.grid()

        kwargs = {"padx": 10, "pady": 2, "row": 1}

        tkinter.Label(frame, text=u"Input").grid(column=2, **kwargs)
        tkinter.Label(frame, text=u"Output").grid(column=3, **kwargs)
        kwargs["row"] += 1

        self.DefineInputOutputLabel(
            frame, u"Max Channels", "max input channel", "max output channel", kwargs)
        self.DefineInputOutputLabel(
            frame, u"Low Input Latency", "input low input latency", "output low input latency", kwargs)
        self.DefineInputOutputLabel(
            frame, u"Low Output Latency", "input low output latency", "output low output latency", kwargs)
        self.DefineInputOutputLabel(
            frame, u"High Input Latency", "input high input latency", "output high input latency", kwargs)
        self.DefineInputOutputLabel(
            frame, u"High Output Latency", "input high output latency", "output high output latency", kwargs)
        self.DefineInputOutputLabel(
            frame, u"Sampling Rate", "input sampling rate", "output sampling rate", kwargs)


    def __init__(self, title: str, geometry: str):
        self.root = tkinter.Tk()
        self.root.title(title)
        self.root.geometry(geometry)
        self.rid = {}
        self.var = {}

        self.ArrangeDeviceFrame()
        self.ArrangeDeviceInfoFrame()

        """
        sampling_rate_label = tkinter.Label(frame, text=u"Sampling Rate")
        sampling_rate_label.grid(row=6, column=1, padx=padx, pady=pady)

        sampling_rate_entry = tkinter.Entry(frame, width=20)
        sampling_rate_entry.grid(row=6, column=2, padx=padx, pady=pady)
        sampling_rate_entry.insert(tkinter.END, u"44100")
        
        use_input_channel_label = tkinter.Label(frame, text=u"Use Input Channel")
        use_input_channel_label.grid(row=7, column=1, padx=padx, pady=pady)

        use_input_entry = tkinter.Entry(frame, width=20)
        use_input_entry.grid(row=7, column=2, padx=padx, pady=pady)
        use_input_entry.insert(tkinter.END, u"2")

        bitrate_label = tkinter.Label(frame, text=u"Bit Rate")
        bitrate_label.grid(row=8, column=1, padx=padx, pady=pady)

        bitrate_entry = tkinter.Entry(frame, width=20)
        bitrate_entry.grid(row=8, column=2, padx=padx, pady=pady)
        bitrate_entry.insert(tkinter.END, u"16")
        """

        self.root.mainloop()

if __name__ == "__main__":
    mainwindow = MainWindow("pyeffector gui", "800x600")

    