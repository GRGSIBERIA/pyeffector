import numpy as np
from numba import vectorize, void, float64

@vectorize([void(float64[:], float64[:], float64, float64)])
def hard_clipping(indata: np.ndarray, outdata: np.ndarray, amplitude=5.0, level=0.8):
    outdata[:] = np.clip(indata * amplitude, -1.0, 1.0) * level

@vectorize([void(float64[:], float64[:], float64, float64, float64)])
def soft_clipping(indata: np.ndarray, outdata: np.ndarray, amplitude=5.0, level=0.8, quarity=0.3):
    d = np.arctan(indata) / (np.pi / 2.0)
    d[:] = np.where(d > 0, amplitude * d, amplitude * d * quarity)
    outdata[:] = np.clip(d, -1.0, 1.0)