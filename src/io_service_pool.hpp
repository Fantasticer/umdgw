#ifndef __UMDGW_IO_SERVICE_POOL_HPP_INCLUDED__
#define __UMDGW_IO_SERVICE_POOL_HPP_INCLUDED__

#include<boost/asio.hpp>
#include<boost/shared_ptr.hpp>
#include<boost/noncopyable.hpp>
#include<vector>
#include<boost/enable_shared_from_this.hpp>
#include<boost/thread.hpp>

typedef boost::shared_ptr<boost::asio::io_service> io_service_ptr;
typedef boost::shared_ptr<boost::asio::io_service::work> worker_ptr;

namespace umdgw {

  class io_service_pool_t 
    :public boost::enable_shared_from_this<io_service_pool_t>,
    private boost::noncopyable
  {
  public:
    io_service_pool_t(std::size_t poolSize);
    ~io_service_pool_t();
    void run();
    void stop();
    boost::shared_ptr<boost::asio::io_service> getNextService();

  private:
    std::vector<io_service_ptr> io_services_;
    std::vector<worker_ptr> workers_;
    std::size_t next_service_;
    std::vector<boost::shared_ptr<boost::thread>> threads_;
  };


}// namespace umdgw
#endif // !__UMDGW_IO_SERVICE_POLL_HPP_INCLUDED__
