#include <iostream>
#include <zmq.hpp>
#include <vector>

int main() {
  zmq::context_t context(1);
  zmq::socket_t socket(context,ZMQ_PULL);
  socket.bind("tcp://*:5555");

  while (true) {
    // 接收带有矩阵数据的消息
    zmq::message_t message;
    socket.recv(&message);

    // 将接收到的消息转换为vector
    std::vector<char> buffer((char*)message.data(), (char*)message.data() + message.size());
    std::cout << (char *) message.data() << "\n";
    // 将vector转换为二进制矩阵数据
    int rows = 3;
    int cols = 3;
    std::vector<float> matrix_data(rows * cols);
    std::memcpy(matrix_data.data(), buffer.data(), rows * cols * sizeof(float));

    // 打印接收到的矩阵
    std::cout << "Received matrix:" << std::endl;
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < cols; j++) {
        std::cout << matrix_data[i * cols + j] << " ";
      }
      std::cout << std::endl;
    }
  }

  return 0;
}
