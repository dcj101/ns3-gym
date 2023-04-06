#include <iostream>
#include <string>
#include <zmq.hpp>
#include <unistd.h> // 包含 sleep() 函数的头文件

using namespace std;
int main() {
    // 初始化 ZMQ 上下文和 PUSH socket
    zmq::context_t context(1);
    zmq::socket_t socket(context, zmq::socket_type::push);
    socket.connect("tcp://localhost:5555");

    // 生成一个 3x3 的随机矩阵
    int matrix[3][3];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            matrix[i][j] = rand() % 100;
            cout << matrix[i][j];
            if(j == 2) cout << "\n";
            else cout << " ";
        }
    }

    // 将矩阵转换为字符串并发送
    
    for (int i = 0; i < 4; i++) {
        std::string matrix_str = "";
        for (int j = 0; j < 3; j++) {
            if(i == 3) {
                matrix_str = "done";
                break;
            }
            matrix_str += std::to_string(matrix[i][j]) + ",";
        }    
        zmq::message_t message(matrix_str.length());
        memcpy(message.data(), matrix_str.c_str(), matrix_str.length());
        socket.send(message);
        sleep(0.1);
    }

    // 关闭 socket 和 context
    socket.close();
    context.close();

    return 0;
}
