import zmq

context = zmq.Context()
socket = context.socket(zmq.PULL)
socket.bind("tcp://*:5555")

# 接收数据并合并
received_data = b''
while True:
    data = socket.recv()
    print(data)
    if data == b"done":
        break
    received_data += data

# 将数据转换为原始矩阵
size = int(len(received_data) ** 0.5)
matrix = [received_data[i:i+size] for i in range(0, len(received_data), size)]

print(matrix)

socket.close()
context.term()
