#!/usr/bin/python3

import math
import random

f = open("getIntlvQuality.bash", "w")

bankNumList = [16, 32, 48, 64]
ratioList = [0.0625, 0.125, 0.1875, 0.25, 0.3125, 0.375, 0.4375, 0.5, 0.5625, 0.625, 0.6875, 0.75, 0.8125, 0.875, 0.9375, 1.0, 1.0625, 1.125, 1.1875, 1.25, 1.3125, 1.375, 1.4375, 1.5, 1.5625, 1.625, 1.6875, 1.75, 1.8125, 1.875, 1.9375, 2.0, 2.0625, 2.125, 2.1875, 2.25, 2.3125, 2.375, 2.4375, 2.5, 2.5625, 2.625, 2.6875, 2.75, 2.8125, 2.875, 2.9375, 3.0, 3.0625, 3.125, 3.1875, 3.25, 3.3125, 3.375, 3.4375, 3.5, 3.5625, 3.625, 3.6875, 3.75, 3.8125, 3.875, 3.9375, 4.0, 4.0625, 4.125, 4.1875, 4.25, 4.3125, 4.375, 4.4375, 4.5, 4.5625, 4.625, 4.6875, 4.75, 4.8125, 4.875, 4.9375, 5.0]

for bankNum in bankNumList:
    for ratio in ratioList:
        img_hb = int(bankNum * 512 * ratio)
        img_v = img_hb
        test_case_name = "python3 measure_interleaving_quality.py\t--num_banks=" + str(bankNum) + "\t--img_hb=" + str(img_hb) + "\t--img_v=" + str(img_v) + ";\n"
        f.write(test_case_name)
