#ifndef __UMDGW_SERVICE_HPP_INCLUDED__
#define __UMDGW_SERVICE_HPP_INCLUDED__

#include<string>
#include<boost/shared_ptr.hpp>
#include<boost/noncopyable.hpp>
#include<boost/enable_shared_from_this.hpp>
#include "ctx.hpp"
#include"io_service_pool.hpp"
#include"socket.hpp"
#include"config.hpp"
namespace umdgw {

  class service_t 
  {
  public:
    service_t() {};
    virtual ~service_t() {};

    virtual void init(std::string name,
      std::string addr, std::string port,
      SERVICE_DATA_TYPE dataType,
      SERVICE_IO_TYPE ioType, int ioPoolSize,
      boost::shared_ptr<ctx_t> context) {
    }

    virtual void run() {
    }
    virtual void stop() {
    }

    virtual bool processor(int processorType) {
      return true;
    }

    virtual void doConnect(){
    }
    virtual void handleConnect(int errorcode, std::string detail){
    }

    virtual void doRead() {
    }
    virtual void handleRead(int errorcode,std::string detail,int bytes_transferred) {
    }

    virtual void doWrite() {
    }
    virtual void handleWrite(int errorcode,std::string detail,int bytes_transferred) {
    }

    virtual void doSetTimer(TIMER_ID timerID,int milli) {
    }
    virtual void handleTimer(TIMER_ID timerID) {
    }
    virtual void doCancelTimer(TIMER_ID timerID) {
    }
    virtual void ErrorHandle(int errorcode, std::string detail) {

    }
    SERVICE_DATA_TYPE getDataType() {
      return data_type_;
    }
    SERVICE_IO_TYPE getIoType() {
      return io_type_;
    }

    boost::shared_ptr<io_service_pool_t> getInputServices() {
      return input_services_pool_;
    }

    boost::shared_ptr<io_service_pool_t> getOutputServices() {
      return output_services_pool_;
    }

  protected:
    //the ip address or the file path
    std::string addr_;
    
    //the service port
    std::string port_;

    //the name of the service
    std::string name_;
    
    //the service data type
    SERVICE_DATA_TYPE data_type_;

    // the service io type
    SERVICE_IO_TYPE io_type_;

    //the context 
    boost::shared_ptr<ctx_t> context_;

    //the associated socket
    boost::shared_ptr<socket_t> socket_;

    //the input service pool
    boost::shared_ptr<io_service_pool_t> input_services_pool_;

    //the output services pool
    boost::shared_ptr<io_service_pool_t> output_services_pool_;
  };
}
#endif // !__UMDGW_SERVICE_HPP_INCLUDED__

