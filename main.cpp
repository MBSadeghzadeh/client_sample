#include <asio.hpp>
#include <iostream>
#include <SampleMessage.pb.h>

void do_read_body(asio::ip::tcp::socket &socket_, std::size_t body_length);

void do_read_header(asio::ip::tcp::socket &socket_)
{
    char header_msg[4];
    
    asio::async_read(socket_,
                     asio::buffer(header_msg, 4),
                     [&socket_,header_msg](std::error_code ec, std::size_t /*length*/)
                     {
                         if (!ec)
                         {
                            std::size_t body_length = std::atoi(header_msg);
                             do_read_body(socket_, body_length);
                         }
                         else
                         {
                             socket_.close();
                         }
                     });
}

void do_read_body(asio::ip::tcp::socket &socket_, std::size_t body_length)
{
    char *body_message = new char[body_length];
    asio::async_read(socket_,
                     asio::buffer(body_message, body_length),
                     [&socket_, body_message, &body_length](std::error_code ec, std::size_t /*length*/)
                     {
                         if (!ec)
                         {
                             std::cout.write(body_message, body_length);
                             std::cout << "\n";
                             do_read_header(socket_);
                         }
                         else
                         {
                             socket_.close();
                         }

                         delete []body_message;
                     });
}

int main()
{
    asio::io_context io_context;

    asio::ip::tcp::resolver resolver(io_context);

    auto endpoints = resolver.resolve("127.0.0.1", "245671");

    asio::ip::tcp::socket socket_ = asio::ip::tcp::socket(io_context);

    asio::async_connect(socket_, endpoints,
                        [&socket_](std::error_code ec, asio::ip::tcp::endpoint)
                        {
                            if (!ec)
                            {
                                do_read_header(socket_);
                            }
                            else {
                                std::cout<< "error to connect" <<ec.message() << std::endl;
                            }
                        });
    TemperatureData td;
    td.set_version("1.0.0");
    
    io_context.run(); 

    std::cout<< "finished" << std::endl;

    return 0;
}