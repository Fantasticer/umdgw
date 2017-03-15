#include"ctx.hpp"
namespace umdgw 
{
  ctx_t::ctx_t(const config_ctx_t& config):
    input_service_pool_ptr_(new io_service_pool_t(config.input_io_threads_count)),
    output_service_pool_ptr_(new io_service_pool_t(config.output_io_threads_count))
  {
    
  }
  ctx_t::~ctx_t() {


  }


  boost::shared_ptr<socket_t> ctx_t::createSocket(boost::shared_ptr<service_t> service) {

    boost::shared_ptr<socket_t> sock(new socket_t(shared_from_this(),service));

    bool rt = sock->init(service->getDataType());
    if (!rt)
      sock.reset();

    return sock;
  }
  boost::shared_ptr<io_service_pool_t> ctx_t::createOutPutServicePool(int ioThreads)
  {
    if (ioThreads <= 0) {
      return nullptr;
    }
    boost::shared_ptr<io_service_pool_t> ptr(new io_service_pool_t(ioThreads));
    return ptr;
  }

  void ctx_t::pubmessage_t(std::string serviceName, uint8_t* content, int size) {


  }

}
