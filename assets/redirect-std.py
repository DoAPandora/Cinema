import sys
import nativelib

class StdBuffer(object):
    def __init__(self, type):
        self.type = type
    def write(self, data):
        nativelib.std_write(self.type, data.strip())
    def flush(self):
        pass

sys.stdout = StdBuffer(1)
sys.stderr = StdBuffer(2)

print("redirectStd Loaded!")