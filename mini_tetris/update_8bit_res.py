import os
import glob

INPUT_PATH = "res"
OUTPUT_PATH = "headers"

for path in glob.glob("./%s/*.c" % INPUT_PATH):
    # read
    f = open(path, "r")
    str = f.readlines()
    f.close()

    str[8] = str[8].replace('uint32_t', 'uint8_t', 1).replace('[70] ', '[10] ')

    for i in range(10, len(str)):
        if (str[i].find('0xff') != -1):
            binaries = str[i].rstrip().replace('0xff000000', '0').replace('0xffffffff', '1').split(", ")
            binaries[-1] = binaries[-1].replace(',', '')

            bit8 = 0
            for j in range(6, -1, -1):
                bit8 |= int(binaries[j]) * (1 << (6 - j));

            if (str[i][-2] == " "):
                str[i] = ("0x%0.2x" % bit8) + ",\n"
            else:
                str[i] = ("0x%0.2x" % bit8) + "\n"

    del str[3:5] #define FULL_FRAME_WIDTH X
                 #define FULL_FRAME_HEIGHT X

    # write
    filename_without_extension = os.path.splitext(os.path.basename(path))[0]
    f = open("./%s/%s.h" % (OUTPUT_PATH, filename_without_extension), "w")
    for line in str:
        f.write(line)
    f.close()
