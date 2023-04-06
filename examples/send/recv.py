import zmq
import numpy as np

context = zmq.Context()
socket = context.socket(zmq.PULL)
socket.bind("tcp://*:5555")

# 接收二进制数据并转换回矩阵
data = socket.recv()
# matrix = np.fromstring(data, dtype=np.float64).reshape((3, 3))

print(data)
# 处理矩阵
# ...

# 关闭socket和context
socket.close()
context.term()
