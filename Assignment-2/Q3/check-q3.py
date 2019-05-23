import numpy as np
import subprocess
import sys
import time

"""
Sample input specification - (buffer_size, followed by lines of input)
8
1 0 0 2 1
2 4 5 1 0
4 2 4 2 4
3 0 5 5 1
5 0 2 5 0
1 2 2 0 0
3 2 5 1 0
5 4 3 2 3
2 1 2 1 0
0 0 0 41 0


Sample output specification -
Buffer Items:
Overflow Items:

Buffer Items: 7 8 3 4 9 10 11 12
Overflow Items:

Buffer Items: 11 12 19 20 13 21 14 15
Overflow Items: 16 17 18 22

Buffer Items: 14 15 22 16 17 18 23 24
Overflow Items: 28 29 30 25 26 27

Buffer Items: 18 23 24 25 26 28 29 27
Overflow Items: 30 33 34 35 36 37 31 32

Buffer Items: 18 23 24 25 26 28 29 27
Overflow Items: 30 33 34 35 36 37 40 41 42 31 32 38 39

Buffer Items: 23 24 25 26 28 29 27 30
Overflow Items: 33 34 35 36 37 40 50 51 52 41 42 48 49 31 32 38 39 43 44 45 46 47

Buffer Items: 29 27 30 31 32 33 34 35
Overflow Items: 36 37 40 50 51 52 60 61 62 63 64 41 42 48 49 53 54 55 56 38 39 43 44 45 46 47 57 58 59

Buffer Items: 27 30 31 32 33 34 35 36
Overflow Items: 37 40 50 51 52 60 61 62 63 64 68 69 41 42 48 49 53 54 55 56 65 38 39 43 44 45 46 47 57 58 59 66 67

Buffer Items:
Overflow Items:

"""


def parse(output):
    output = output.split('\n')
    results = []
    for i in range(0, len(output) - 1, 3):
        buffer_items = [int(x) for x in output[i].split()[2:]]
        overflow_items = [int(x) for x in output[i + 1].split()[2:]]
        results.append([buffer_items, overflow_items])
    return results


def error(i, e_type):
    print("Error in cycle %d of type %d" % (i, e_type))
    sys.exit()


def test(i, data, buffer_size):
    cycles = len(data)
    command = "./a.out < input.txt"
    output = subprocess.check_output(command, shell=True)
    results = parse(output)
    extra = 0
    pipeline = []
    total_produce = 1
    for i in range(cycles):
        result = results[i]
        production = data[i][0] + data[i][1] + data[i][2]
        consumption = data[i][3] + data[i][4]
        extra += production - consumption
        # Check whether length of buffer and overflow is correct
        if extra <= 0:
            if len(result[0]) > 0 or len(result[1]) > 0:
                error(i, 1)
        elif extra > 0 and extra <= buffer_size:
            if len(result[0]) != extra or len(result[1]) > 0:
                error(i, 2)
        else:
            if len(result[0]) != buffer_size or len(result[1]) != (extra - buffer_size):
                error(i, 3)
        # Check whether priorities are correctly implemented
        # If any of previous `pipeline` items in current overflow buffer,
        # all new items must be in overflow buffer. This is sufficient
        # condition.
        if len([j for j in pipeline if j in result[1]]) > 0:
            for k in range(total_produce, production + total_produce):
                if k not in result[1]:
                    error(i, 4)
        pipeline = result[1]
        total_produce += production


BUFFER_RANGE = [1, 11]
CYCLES_RANGE = [5, 55]
RUNS = 5
CASES = 1000

for i in range(CASES):
    np.random.seed(i)
    buffer_size = np.random.randint(BUFFER_RANGE[0], BUFFER_RANGE[1])
    input_file = "%d\n" % buffer_size
    # input_file = ""
    cycles = np.random.randint(CYCLES_RANGE[0], CYCLES_RANGE[1])
    data = []
    production = 0
    consumption = 0
    for j in range(cycles - 1):
        prod1 = np.random.randint(0, 6)
        prod2 = np.random.randint(0, 6)
        prod3 = np.random.randint(0, 6)
        cons1 = np.random.randint(0, 6)
        cons2 = np.random.randint(0, 6)
        production += prod1 + prod2 + prod3
        consumption += cons1 + cons2
        data.append([prod1, prod2, prod3, cons1, cons2])
        input_file += '%d %d %d %d %d\n' % (prod1, prod2, prod3, cons1, cons2)
    if production > consumption:
        data.append([0, 0, 0, production - consumption, 0])
        input_file += "0 0 0 %d 0\n" % (production - consumption)
    elif production < consumption:
        data.append([consumption - production, 0, 0, 0, 0])
        input_file += "%d 0 0 0 0\n" % (consumption - production)

    # with open('input.txt', 'w') as f:
    #     f.write(input_file)
    # with open('input_file.txt', 'w') as f:
    #     f.write(str(buffer_size))
    with open('input.txt', 'w') as f:
        f.write(input_file)
    print("Input File %d generated" % i)

    for j in range(RUNS):
        test(i, data, buffer_size)

print("All tests passed!")
