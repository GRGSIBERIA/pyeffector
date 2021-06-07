import time
import tkinter
from tkinter import messagebox
from tkinter.constants import ANCHOR
import tkinter.ttk as ttk
import sounddevice as sd
import numpy as np

def StreamCallback(indata: np.ndarray, outdata: np.ndarray, frames: int, time, status: sd.CallbackFlags):
    if status:
        print(status)
    outdata[:] = indata

class MainWindow:
    def PressPlayButton(self, event):
        print("press play")

        if self.rid["output device"].get() == "" or self.rid["input device"].get() == "":
            messagebox.showerror(u"unable input/output device", u"Please, select input/output device.")
            return

        fs = int(self.rid["sampling rate"].get())
        outdevice = self.rid["output device"].get()
        indevice = self.rid["input device"].get()

        outinfo = sd.query_devices(outdevice)
        ininfo = sd.query_devices(indevice)
        sd.default.device = (ininfo["name"], outinfo["name"])

        inmax_ch = ininfo["max_input_channels"]
        outmax_ch = outinfo["max_output_channels"]
        inoutmax_ch = outmax_ch if inmax_ch > outmax_ch else inmax_ch
        

        self.stream = sd.Stream(samplerate=fs, channels=inoutmax_ch, dtype="float32", callback=StreamCallback)
        self.stream.start()
        print("start streaming")

    def PressStopButton(self, event):
        print("press stop")

        if self.stream != None:
            if self.stream.active:
                self.stream.stop()
                print("stop streaming")
    
    def PressAbortButton(self, event):
        if self.stream != None:
            if self.stream.active:
                self.stream.abort()
                print("abort streaming")

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
        pady = 4

        self.device_info = {}
        list_devices = []
        for device in sd.query_devices():
            list_devices.append(device["name"])
            self.device_info[device["name"]] = device

        self.input_combo_string = tkinter.StringVar()
        self.output_combo_string = tkinter.StringVar()
        kwargs = {"padx": padx, "pady": pady, "row": 1}
        
        tkinter.Label(frame, text=u"Input Device").grid(column=1, **kwargs)
        kwargs["row"] += 1
        
        self.rid["input device"] = ttk.Combobox(frame, textvariable=self.input_combo_string, values=list_devices, width=40)
        self.rid["input device"].insert(tkinter.END, str(list_devices[sd.default.device[0]]))
        self.rid["input device"].grid(column=1, columnspan=2, **kwargs)
        kwargs["row"] += 1

        tkinter.Label(frame, text=u"Output Device").grid(column=1, **kwargs)
        kwargs["row"] += 1

        self.rid["output device"] = ttk.Combobox(frame, textvariable=self.output_combo_string, values=list_devices, width=40)
        self.rid["output device"].insert(tkinter.END, str(list_devices[sd.default.device[1]]))
        self.rid["output device"].grid(column=1, columnspan=2, **kwargs)
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
        
        tkinter.Label(frame, text=u"Using Sampling Rate").grid(column=1, **kwargs)
        self.var["sampling rate"] = tkinter.StringVar()
        self.rid["sampling rate"] = ttk.Combobox(frame, textvariable=self.var["sampling rate"], values=["22050", "44100", "48000", "96000", "192000"])
        self.rid["sampling rate"].insert(tkinter.END, "96000")
        self.rid["sampling rate"].grid(column=2, **kwargs)
        kwargs["row"] += 1

        play_btn = tkinter.Button(frame, text=u"Play", width=10)
        play_btn.grid(column=1, **kwargs)
        play_btn.bind("<Button-1>", self.PressPlayButton)

        stop_btn = tkinter.Button(frame, text=u"Stop", width=10)
        stop_btn.grid(column=2, **kwargs)
        stop_btn.bind("<Button-1>", self.PressStopButton)

        abort_btn = tkinter.Button(frame, text="Abort", width=10)
        abort_btn.grid(column=3, **kwargs)
        abort_btn.bind("<Button-1>", self.PressAbortButton)


    def __init__(self, title: str, geometry: str):
        self.root = tkinter.Tk()
        self.root.title(title)
        self.root.geometry(geometry)
        self.rid = {}
        self.var = {}

        self.ArrangeDeviceFrame()
        self.ArrangeDeviceInfoFrame()

        self.root.mainloop()

if __name__ == "__main__":
    mainwindow = MainWindow("pyeffector gui", "800x600")

    