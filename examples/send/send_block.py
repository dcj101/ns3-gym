import zmq
import numpy as np
import time
import random

context = zmq.Context()
socket = context.socket(zmq.PUSH)
socket.connect("tcp://localhost:5555")

# 生成一个大的随机矩阵
matrix = [[random.randint(0,255) for _ in range(5)] for _ in range(5)]
# 每次发送10个元素
chunk_size = 5

print(matrix)

# 将矩阵分块，并转换为字符串发送
for row in matrix:
    chunks = [row[i:i+chunk_size] for i in range(0, len(row), chunk_size)]
    for chunk in chunks:
        socket.send_string(','.join(map(str, chunk)))
        print(','.join(map(str, chunk)))
        time.sleep(0.1)

# 发送结束标志
socket.send_string("done")

# 关闭socket和context
socket.close()
context.term()




