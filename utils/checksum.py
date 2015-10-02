import struct
import sys

file_to_patch = sys.argv[1]

# Open the binary file
f = open(file_to_patch, "ro")
raw = [0]*7
val = [0]*7
sum = 0

# Read the vector table
for i in range(7):
    raw[i] = f.read(4)
    val[i] = struct.unpack("I", raw[i])[0]
    sum += val[i]

f.close()

# Compute the checksum
result = ((pow(2, 32)-1)-sum) + 1
print "Created checksum %X at offset 0x1c in file %s"%(result, file_to_patch)

f = open(file_to_patch, "r+")
f.seek(28)
f.write(struct.pack("<I", result))
f.close
