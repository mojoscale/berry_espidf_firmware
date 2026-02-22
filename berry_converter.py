import struct

script = open("sample_script.be", "rb").read()
with open("sample_script.bin", "wb") as f:
    f.write(struct.pack("<I", len(script)))
    f.write(script)
