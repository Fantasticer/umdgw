#ifndef __UMDGW_SOCKET_HPP_INCLUDED__
#define __UMDGW_SOCKET_HPP_INCLUDED__

#include<boost/asio.hpp>
#include<boost/system/error_code.hpp>
#include<boost/shared_ptr.hpp>
#include<boost/enable_shared_from_this.hpp>
#include<boost/bind.hpp>
#include<boost/date_time/posix_time/posix_time.hpp>
#include<string>
#include"service.hpp"
#include"ctx.hpp"
#include<stdint.h>
#include<map>
#include"config.hpp"

namespace umdgw {
  class ctx_t;
  class service_t;

  class socket_t
    :public boost::enable_shared_from_this<socket_t>
  {
  public:
    socket_t(boost::shared_ptr<ctx_t> ctx,
      boost::shared_ptr<service_t> service);

    ~socket_t();

    bool init(SERVICE_DATA_TYPE type);
    void connect(std::string addr,std::string port);
    bool bind(std::string port);
    void asycn_accept();
    void setTimer(TIMER_ID timerID,int milli);
    void cancelTimer(TIMER_ID timerID);
    void async_read( uint8_t* content,  int size);
    void async_write( uint8_t* content, int size);
    void close();
    std::string getRemoteIP();
  private:
    void connnectHandle(const boost::system::error_code& error,
      boost::asio::ip::tcp::resolver::iterator iterator);
    void acceptHandle();
    void readHandle(const boost::system::error_code ec, std::size_t bytes_transferred);
    void writeHandle(const boost::system::error_code ec, std::size_t bytes_transferred);
    void timerHandle(const boost::system::error_code ec, TIMER_ID timerId);
    SERVICE_DATA_TYPE type_;

    std::map<TIMER_ID, boost::shared_ptr<boost::asio::deadline_timer>> timers_map_;
    boost::scoped_ptr<boost::asio::ip::tcp::socket> socket_input_ptr_;
    boost::scoped_ptr<boost::asio::ip::tcp::socket> socket_output_ptr_;
    boost::scoped_ptr<boost::asio::ip::tcp::acceptor> acceptor_ptr_;
    boost::shared_ptr<boost::asio::io_service> io_service_ptr_;
    //the context object
    boost::shared_ptr<ctx_t> context_ptr_;

    //the service object associated with this socket
    boost::shared_ptr<service_t> service_ptr_;

    //the romote endpoint
    boost::asio::ip::tcp::endpoint remote_endpoint_;


  };


} // namespace umdgw

#endif // !__UMDGW_SOCKET_HPP_INCLUDED__

