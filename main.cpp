#include <asio.hpp>
#include <iostream>
#include <thread>
#include <chrono>

#include <SampleMessage.pb.h>

void do_read_body(asio::ip::tcp::socket &socket_, std::size_t body_length);

void do_read_header(asio::ip::tcp::socket &socket_)
{
    constexpr size_t header_size = 8;
    char *header_msg = new char[header_size];
    auto async_read_handler = [&socket_, header_msg](std::error_code ec, std::size_t length)
    {
        std::cout << "lengh of header:" << length << std::endl;
        if (!ec)
        {
            std::size_t body_length;
            memcpy(&body_length, header_msg, length);
            std::cout << "lengh of body:" << body_length << std::endl;
            delete[] header_msg;

            do_read_body(socket_, body_length);
        }
        else
        {
            std::cout << ec.message() << std::endl;
            socket_.close();
        }
    };

    asio::async_read(socket_, asio::buffer(header_msg, header_size), async_read_handler);
}

void do_read_body(asio::ip::tcp::socket &socket_, std::size_t body_length)
{
    char *body_message = new char[body_length];

    auto async_read_handler =
        [&socket_, body_message, body_length](std::error_code ec, std::size_t /*length*/)
    {
        if (!ec)
        {
            TemperatureData td;
            td.ParseFromArray(body_message, body_length);
            std::cout << td.temperature() << "   " << td.device_id() << "  " << td.timestamp() << std::endl;
            delete[] body_message;

            do_read_header(socket_);
        }
        else
        {
            socket_.close();
        }
    };

    asio::async_read(socket_, asio::buffer(body_message, body_length), async_read_handler);
}

int main(int argc, char *argv[])
{
    std::cout << "main thread Id: " << std::this_thread::get_id() << std::endl;

    if (argc > 2)
    {
        asio::io_context io_context;

        asio::ip::tcp::resolver resolver(io_context);

        auto endpoints = resolver.resolve(argv[1], argv[2]);

        asio::ip::tcp::socket socket_ = asio::ip::tcp::socket(io_context);

        auto async_connect_handler = [&socket_](std::error_code ec, asio::ip::tcp::endpoint)
        {
            std::cout << "async_connect_handler thread Id: " << std::this_thread::get_id() << std::endl;

            if (!ec)
            {
                do_read_header(socket_);
            }
            else
            {
                std::cout << "can't connect to the server:" << ec.message() << std::endl;
            }
        };

        asio::async_connect(socket_, endpoints, async_connect_handler);

        io_context.run();

        std::cout << "finished" << std::endl;
    }
    else
    {
        std::cout << "please enter server address and service port: Client <host> <port>" << std::endl;
    }

    return 0;
}