#ifndef __UMDGW_CTX_HPP_INCLUDED__
#define __UMDGW_CTX_HPP_INCLUDED__

#include"io_service_pool.hpp"
#include<boost/shared_ptr.hpp>
#include<boost/scoped_ptr.hpp>
#include<boost/enable_shared_from_this.hpp>
#include<boost/noncopyable.hpp>
#include<unordered_map>
#include<map>
#include<set>
#include<string>
#include<vector>
#include<stdint.h>
#include"service.hpp"
#include"pipe.hpp"
#include"socket.hpp"
#include"config.hpp"

namespace umdgw {
  class socket_t;
  class service_t;

  class ctx_t
  :public boost::enable_shared_from_this<ctx_t>,
    private boost::noncopyable
  {
  public:
    ctx_t(const config_ctx_t& config);
    ~ctx_t();

    //assigan a socket to the service object.
    boost::shared_ptr<socket_t> createSocket(boost::shared_ptr<service_t> service);

    //assign a unique output_service_pool
    boost::shared_ptr<io_service_pool_t> createOutPutServicePool(int ioThreads);
    
    //publish the message_ts to the pipes which sub the service's outputing message_t.
    void pubmessage_t(std::string serviceName, uint8_t* content, int size);
     
    //the global service pool belongs to the ctx object
    boost::scoped_ptr<io_service_pool_t> input_service_pool_ptr_;
    boost::scoped_ptr<io_service_pool_t> output_service_pool_ptr_;

  protected:
 
    //service table, each service has an unique id
    std::unordered_map<std::string,int> services_;
    int service_id_;

    //service publish pipe set
    std::map<int, std::set<boost::shared_ptr<pipe_t>>>  pub_pipe_set_;




  };

}

#endif // !__UMDGW_CTX_HPP_INCLUDED__
