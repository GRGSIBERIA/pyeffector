import numpy as np
from numba import jit

def hard_clipping(indata: np.ndarray, outdata: np.ndarray, amplitude=5.0, level=0.8):
    outdata[:] = np.clip(indata * amplitude, -1.0, 1.0) * level
