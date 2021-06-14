import numpy as np
from numba import jit

@jit("void(f8[:], f8[:], f8, f8)")
def hard_clipping(indata: np.ndarray, outdata: np.ndarray, amplitude=5.0, level=0.8):
    length = np.abs(indata * amplitude)
    outdata[:] = np.where(length > 1.0, indata * np.sign(indata), length * level)

@jit("void(f8[:], f8[:], f8, f8, f8)")
def soft_clipping(indata: np.ndarray, outdata: np.ndarray, amplitude=5.0, level=0.8, quarity=0.3):
    d = np.arctan(indata) / (np.pi / 2.0)
    d[:] = np.where(d > 0, amplitude * d, amplitude * d * quarity)
    outdata[:] = np.where(np.abs(d) > 1.0, np.sign(d) * 1.0, d * level)

@jit("void(f8[:], f8[:], f8, f8)")
def compressor(indata: np.ndarray, outdata: np.ndarray, threshold=0.1, ratio=2.0):
    diff = (np.abs(indata) - threshold) / ratio
    outdata[:] = np.where(np.abs(indata) < threshold, 
        indata, 
        np.where(indata < 0.0, 
            -(threshold + diff), 
            threshold + diff))