#include  "io_service_pool.hpp"

namespace umdgw{
    io_service_pool_t::io_service_pool_t(std::size_t poolSize)
    :next_service_(0)
  {
    io_services_.reserve(poolSize);
    workers_.reserve(poolSize);
    for (std::size_t i = 0; i < poolSize; i++) {
      io_service_ptr service(new boost::asio::io_service());
      worker_ptr work(new boost::asio::io_service::work(*service));
      io_services_.push_back(service);
      workers_.push_back(work);
    }
  }

  io_service_pool_t::~io_service_pool_t()
  {
    //do nothing
  }


  void io_service_pool_t::run() {
    
    for (std::size_t i = 0; i < io_services_.size(); i++) {
      boost::shared_ptr<boost::thread> t(new boost::thread(
        bind(&boost::asio::io_service::run, io_services_[i])));
      threads_.push_back(t);
    }


  }

  void io_service_pool_t::stop() {
    for (std::size_t i = 0; i < io_services_.size(); i++) {
      io_services_[i]->stop();
    }
    for (std::size_t i = 0; i < threads_.size(); i++) {
      threads_[i]->join();
    }
  }

  boost::shared_ptr<boost::asio::io_service> io_service_pool_t::getNextService() {
    io_service_ptr tmp = io_services_[next_service_];
    next_service_ = (++next_service_) % (io_services_.size());
    return tmp;
  }

}
