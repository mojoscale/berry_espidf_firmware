import struct
import subprocess
import sys

CHIP = "esp32s3"
PORT = "COM6"
BAUD = "921600"
OFFSET = "0x7e0000"
SCRIPT_FILE = "sample_script.be"
OUTPUT_BIN = "sample_script.bin"

# 1️⃣ Convert script
script = open(SCRIPT_FILE, "rb").read()

with open(OUTPUT_BIN, "wb") as f:
    f.write(struct.pack("<I", len(script)))
    f.write(script)

print("✔ Binary generated:", OUTPUT_BIN)

# 2️⃣ Flash via module
cmd = [
    sys.executable,
    "-m",
    "esptool",
    "--chip",
    CHIP,
    "--port",
    PORT,
    "--baud",
    BAUD,
    "write_flash",
    OFFSET,
    OUTPUT_BIN,
]

print("Flashing to partition at", OFFSET)

result = subprocess.run(cmd)

if result.returncode != 0:
    print("❌ Flash failed")
    sys.exit(1)

print("✔ Flash successful")
