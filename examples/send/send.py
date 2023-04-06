import zmq
import random

context = zmq.Context()
socket = context.socket(zmq.PUSH)
socket.connect("tcp://localhost:5555")

# 生成一个随机矩阵
matrix = [[random.randint(0, 255) for _ in range(3)] for _ in range(3)]

print(matrix)
# 将矩阵转换为字符串并发送
socket.send_string(str(matrix))

# 关闭socket和context
socket.close()
context.term()
