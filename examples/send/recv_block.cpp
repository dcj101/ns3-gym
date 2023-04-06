#include <zmq.hpp>
#include <iostream>
#include <vector>
#include <sstream>

int main()
{
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_PULL);
    socket.bind("tcp://*:5555");

    std::vector<std::vector<int>> matrix(5);
    std::string received_data;
    while (true)
    {
        zmq::message_t message;
        socket.recv(&message);
        std::string data(static_cast<char*>(message.data()), message.size());
        std::cout << data << std::endl;
        if (data == "done")
        {
            break;
        }
        received_data += data + ',';
    }
    std::cout << received_data << "\n";
    std::stringstream ss(received_data);
    std::string item;
    int i = -1, j = 0;
    while (std::getline(ss, item, ','))
    {
        i++;
        matrix[j].push_back(std::stoi(item));
        if(i == 4) j++, i = -1;
    }

    for (auto row : matrix)
    {
        for (auto element : row)
        {
            std::cout << element << " ";
        }
        std::cout << std::endl;
    }

    socket.close();
    context.close();

    return 0;
}
