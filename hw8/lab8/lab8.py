import sys
import random
import matplotlib.pyplot as plt


def find_page(cache, refer_bit, page, frame_num):
    """
    check whether the cache has this page number
    if has the page, set the bit, return True
    if not, return False, which means we should replace one of page in the cache
    :param cache:
    :param refer_bit:
    :param page:
    :param frame_num:
    :return:
    """
    for frame in range(frame_num):
        if cache[frame] == page:
            refer_bit[frame] = 1
            return True
    return False


def replace(cache, pointer, refer_bit, page, frame_num):
    """
    replace policy:
    loop the cache, if the bit of entry is 0, just replace and return next pointer
    if bit is 1, it has second chance, set to 0
    :param cache:
    :param pointer:
    :param refer_bit:
    :param page:
    :param frame_num:
    :return:
    """
    while True:
        if refer_bit[pointer] == 0:
            cache[pointer] = page
            pointer = (pointer + 1) % frame_num
            return pointer
        else:
            refer_bit[pointer] = 0
        pointer = (pointer + 1) % frame_num


def lru_second_chance_algorithm(page_trace, frame_num):
    """
    subroutine : LRU Second Chance Algorithm
    :param page_trace:
    :param frame_num:
    :return: number of page_fault
    """
    pointer = 0
    page_fault = 0
    cache = [-1 for i in range(frame_num)]
    refer_bit = [0 for i in range(frame_num)]

    for page in page_trace:
        if not find_page(cache, refer_bit, page, frame_num):
            pointer = replace(cache, pointer, refer_bit, page, frame_num)
            page_fault += 1
    return page_fault


if __name__ == '__main__':
    # 1. fetch n and k parameters from command line
    n = int(sys.argv[1])
    k = int(sys.argv[2])

    # 2. generate one random page_trace
    page_trace = []
    for i in range(n):
        page_trace.append(random.randint(0, pow(2, k) - 1))

    x_frames = []
    y_faults = []
    # 3. use lru second chance algorithm
    for f in range(4, pow(2, k) + 1):
        page_fault = lru_second_chance_algorithm(page_trace, f)
        print("num of frames: " + str(f), "num of page faults: " + str(page_fault))
        x_frames.append(f)
        y_faults.append(page_fault)

    # 4. plot page_faults vs number of frames
    plt.plot(x_frames, y_faults)
    plt.xlabel('num of frame')
    plt.ylabel('num of page fault')
    plt.show()


