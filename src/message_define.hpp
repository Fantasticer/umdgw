
#ifndef __UMDGW_MESSAGE_DEFINE_HPP_INCLUDED__
#define __UMDGW_MESSAGE_DEFINE_HPP_INCLUDED__

#include <string>

namespace umdgw {

/// Define the message type
enum MessageType {
  MSG_TYPE_LOGIN = 0,
  MSG_TYPE_LOGOUT,
  MSG_TYPE_HEARTBEAT,
  MSG_TYPE_DATA,
  MSG_TYPE_COMPRESSED
};

/// currently supported protocols
enum MessageProtocolType {
  MSG_PROTOCOL_SIMPLE = 0,
  MSG_PROTOCOL_SH_STEP,
  MSG_PROTOCOL_SZ_STEP,
  MSG_PROTOCOL_SZ_BIN
};

/// the max compress level currently supported
#define MAX_COMPRESS_LEVEL        2
/// the minimum size of the output buffer
#define MIN_COMPRESS_OUT_SIZE     80
/// the minimum size to be compressed
#define MIN_COMPRESS_SIZE         20

/// the customized app version
#define CUSTOM_APP_VERSION_PREFIX ("ZORK_MDGW_V1")

/// Define the logon message
struct LogonMessage {
  std::string username;
  std::string pwd;
	std::string target_id;
  int heartbeat_interval;
  std::string app_ver_id;
  std::string app_ext_id;
	std::string cstm_app_ver_id;
  bool reset_seq_num;
  int nex_expected_seq_num;
  int sending_seq_num;
  bool update_only;
  int compress_level;

  // create the message
  LogonMessage()
    : heartbeat_interval(0)
    , reset_seq_num(true)
    , nex_expected_seq_num(1)
    , sending_seq_num(1)
    , update_only(false)
    , compress_level(0)
  {}
};

/// Define the logout message
struct LogoutMessage {
  std::string username;
	std::string target_id;
  int status;
  std::string text;

  // create the message
  LogoutMessage()
    : status(0)
  {}
};

/// Define the heartbeat message
struct HeartbeatMessage {
  std::string username;
	std::string target_id;

  // create the message
  HeartbeatMessage() 
  {}
};

} //

#endif // __UMDGW_MESSAGE_DEFINE_HPP_INCLUDED__