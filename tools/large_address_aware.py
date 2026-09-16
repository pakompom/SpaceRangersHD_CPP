from pathlib import Path
import sys

path = Path(sys.argv[1])
data = bytearray(path.read_bytes())
pe = int.from_bytes(data[0x3c:0x40], "little")
assert data[pe:pe + 4] == b"PE\0\0"
offset = pe + 22
flags = int.from_bytes(data[offset:offset + 2], "little")
if not flags & 0x20:
    data[offset:offset + 2] = (flags | 0x20).to_bytes(2, "little")
    path.write_bytes(data)
