import os
import glob
import re

examples_dir = 'examples'

# We want to replace the current messy SerialAT definitions with a clean macro.
# Actually, wait. If we just define Quectel_Serial in the library, we can patch all .ino files.

for filepath in glob.glob(f'{examples_dir}/**/*.ino', recursive=True):
    with open(filepath, 'r') as f:
        content = f.read()
    
    # Let's see how many .ino files actually have the HardwareSerial SerialAT(1) block
    if 'HardwareSerial SerialAT' in content or 'HardwareSerial& SerialAT' in content:
        print(f"Found in {filepath}")
