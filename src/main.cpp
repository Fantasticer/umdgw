
#include"ctx.hpp"
#include<iostream>
#include"service.hpp"
#include"szBinary_input_service.hpp"
#include<boost/shared_ptr.hpp>
#include"config.hpp"
#include<string>

using namespace umdgw;

int main() {
  config_ctx_t ctx_config;
  ctx_config.input_io_threads_count = 1;
  ctx_config.output_io_threads_count = 2;
  boost::shared_ptr<ctx_t> ctx_ptr(new ctx_t(ctx_config));
  boost::shared_ptr<sz_binary_input_service_t> inputService_ptr(new sz_binary_input_service_t());
  std::string sName = "SZBINARY_INPUT";
  std::string addr = "127.0.0.1";
  std::string port = "20001";

  inputService_ptr->init(sName, addr, port, SERVICE_DATA_TYPE::STREAM, SERVICE_IO_TYPE::SIN, 1, ctx_ptr);

  inputService_ptr->run();
  char command;
  while (std::cin >> command) {
    if (command == 'q') {
      break;
    }
    else {
      std::cout << "invalid command" << std::endl;
    }
  }

  return 0;
}
