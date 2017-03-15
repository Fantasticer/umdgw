#include "socket.hpp"
namespace umdgw {
  socket_t::socket_t(boost::shared_ptr<ctx_t> ctx,
    boost::shared_ptr<service_t> service):
    context_ptr_(ctx),service_ptr_(service)
  {
  }

  socket_t::~socket_t() {

  }

  bool socket_t::init(SERVICE_DATA_TYPE type) {
    type_ = type;
    bool rt = true;
    if (type_ == STREAM) {
      if (service_ptr_->getIoType() == SERVICE_IO_TYPE::SIN) {
        io_service_ptr_ = service_ptr_->getInputServices()->getNextService();
        socket_input_ptr_.reset(new boost::asio::ip::tcp::socket(*io_service_ptr_));

      }
      else if (service_ptr_->getIoType() == SERVICE_IO_TYPE::SACCEPT) {
        io_service_ptr_ = service_ptr_->getOutputServices()->getNextService();
        acceptor_ptr_.reset(new boost::asio::ip::tcp::acceptor(*io_service_ptr_));
        boost::asio::ip::tcp::acceptor::reuse_address option(true);
        boost::system::error_code ec;
        acceptor_ptr_->set_option(option, ec);
        if (ec) {
          acceptor_ptr_.reset();
          rt = false;
        }
      }
      else if (service_ptr_->getIoType() == SERVICE_IO_TYPE::SOUT) {
        io_service_ptr_ = service_ptr_->getOutputServices()->getNextService();
        socket_output_ptr_.reset(new boost::asio::ip::tcp::socket(*io_service_ptr_));
        boost::asio::ip::tcp::no_delay option(true);
        boost::system::error_code ec;
        socket_output_ptr_->set_option(option, ec);
        if (ec) {

          socket_output_ptr_.reset();
          rt = false;
        }
      }
    }
    else if (type_ == FILE) {

    }

    return rt;
  }
  void socket_t::connect(std::string addr,std::string port) {
    if (!socket_input_ptr_){
      service_ptr_->handleConnect(-1, "invalid socket");
      return;
    }
    if (type_ == STREAM) {
      boost::asio::ip::tcp::resolver resolver(*io_service_ptr_);
      boost::asio::ip::tcp::resolver::query query(addr,port);
      boost::system::error_code ec;
      boost::asio::ip::tcp::resolver::iterator endpoint_it = resolver.resolve(query,ec);
      boost::asio::ip::tcp::resolver::iterator endpoiont_end;

      //can not resolve any valid endpoint
      if (endpoint_it == endpoiont_end) {
        service_ptr_->handleConnect(ec.value(), ec.message());
        return;
      }
      
      boost::asio::async_connect(*socket_input_ptr_, endpoint_it,
        boost::bind(&socket_t::connnectHandle, shared_from_this(),_1,_2));
    }
    else if (type_ == FILE) {

    }
  }

  void socket_t::connnectHandle(const boost::system::error_code& error,
    boost::asio::ip::tcp::resolver::iterator iterator) {

    if (error == boost::asio::error::operation_aborted) {
      return;
    }
    if (error) {
      service_ptr_->handleConnect(error.value(), error.message());
    }
    else {
      boost::asio::ip::tcp::no_delay option(true);
      boost::system::error_code ec;
      socket_input_ptr_->set_option(option, ec);
      if (ec) {
        std::cout << ec.message() << std::endl;
        socket_input_ptr_.reset();
      }
      remote_endpoint_ = *iterator;
      service_ptr_->handleConnect(error.value(), error.message());
    }

  }

  std::string socket_t::getRemoteIP() {
    return remote_endpoint_.address().to_string();
  }

  bool socket_t::bind(std::string port) {
    bool ret = true;
    if (!acceptor_ptr_) {
      ret = false;
      return ret;
    }
    int p = 0;
    p = atoi(port.c_str());
    boost::system::error_code ec;
    while (p > 0) {
      boost::asio::ip::tcp::endpoint ed(boost::asio::ip::tcp::v4(), p);
      acceptor_ptr_->open(ed.protocol(),ec);
      if (ec)
        break;

      acceptor_ptr_->bind(ed, ec);
      break;
    }
    if (ec) {
      ret = false;
    }
    return ret;
  }

  void socket_t::asycn_accept() {

  }

  void socket_t::acceptHandle() {

  }

  void socket_t::setTimer(TIMER_ID timerID,int milli) {
    if (timers_map_.count(timerID) == 0) {
      boost::shared_ptr<boost::asio::deadline_timer> p(new boost::asio::deadline_timer
      (*io_service_ptr_, boost::posix_time::milliseconds(milli)));

      timers_map_[timerID] = p;
      timers_map_[timerID]->async_wait(
        boost::bind(&socket_t::timerHandle, shared_from_this(), _1, timerID));
    }
    else {
      timers_map_[timerID]->cancel();
      timers_map_[timerID]->expires_from_now(boost::posix_time::milliseconds(milli));

      timers_map_[timerID]->async_wait(
        boost::bind(&socket_t::timerHandle, shared_from_this(), _1, timerID));

    }
  }

  void socket_t::cancelTimer(TIMER_ID timerID) {

    if (timers_map_.count(timerID) == 0) {

    }
    else {
      timers_map_[timerID]->cancel();
    }
  }

  void socket_t::timerHandle(const boost::system::error_code ec, TIMER_ID timerId) {
    if (ec == boost::asio::error::operation_aborted) {
      return;
    }
    if (!ec) {
      service_ptr_->handleTimer(timerId);
    }
  }

  void  socket_t::async_read(uint8_t* content,int size) {
    if (type_ == SERVICE_DATA_TYPE::STREAM) {
      if (service_ptr_->getIoType() == SIN) {
        socket_input_ptr_->async_read_some(boost::asio::buffer(content, size),
          boost::bind(&socket_t::readHandle, shared_from_this(),
            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
      }

    }
  }

  void socket_t::readHandle(const boost::system::error_code ec, std::size_t bytes_transferred) {
    if (ec == boost::asio::error::operation_aborted) {
      return;
    }
    service_ptr_->handleRead(ec.value(), ec.message(),bytes_transferred);
  }

  void  socket_t::async_write(uint8_t* content,int size) {
    if (type_ == STREAM) {
      if (service_ptr_->getIoType() == SIN) {
        boost::asio::async_write(*socket_input_ptr_,
          boost::asio::buffer(content, size),
          boost::bind(&socket_t::writeHandle, shared_from_this(),
            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
      }
    }
  }

  void socket_t::close() {
    if (type_ == STREAM) {
      if (service_ptr_->getIoType() == SIN) {
        boost::system::error_code ec;
        socket_input_ptr_->shutdown(boost::asio::ip::tcp::socket::shutdown_both,ec);
        socket_input_ptr_->close(ec);
      }
    }
  }

  void socket_t::writeHandle(const boost::system::error_code ec, std::size_t bytes_transferred) {
    if (ec == boost::asio::error::operation_aborted) {
      return;
    }
    service_ptr_->handleWrite(ec.value(), ec.message(),bytes_transferred);
  }






}
