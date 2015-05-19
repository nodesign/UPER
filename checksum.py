import struct

f = open("UPER-Release.bin", "ro")
raw = [0]*7
val = [0]*7
sum = 0

for i in range(7):
    raw[i] = f.read(4)
    val[i] = struct.unpack("I", raw[i])[0]
    sum += val[i]

f.close()
result = ((pow(2, 32)-1)-sum) + 1 

print "Checksum : %X"%result

f = open("UPER-Release.bin", "r+")
f.seek(28)
f.write(struct.pack("<I", result))
f.close
