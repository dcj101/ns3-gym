import zmq

context = zmq.Context()
socket = context.socket(zmq.PULL)
socket.bind("tcp://*:5555")

# 接收数据并合并
received_data = b''
while True:
    data = socket.recv()
    if data == b"done":
        break
    received_data += data

# 将数据转换为原始矩阵
data_str = received_data.decode('utf-8')
matrix_str = data_str.split(',')
matrix_str.remove('')
matrix = [matrix_str[i:i+3] for i in range(0, len(matrix_str), 3)]
matrix = [[int(j) for j in i] for i in matrix]

print(matrix)

socket.close()
context.term()
