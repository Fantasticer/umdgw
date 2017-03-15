#ifndef __UMDGW_SZBINARY_message_t_DEFINE_HPP_INLCUDED__
#define __UMDGW_SZBINARY_message_t_DEFINE_HPP_INLCUDED__
#include<stdint.h>

namespace umdgw {
#pragma warning(disable:4200) //zero-sized array in struct/union
#pragma pack(push,1)

  //消息头
  struct SZB_Head {
    uint32_t MsgType; //消息类型
    uint32_t BodyLength; //消息体长度
  };
  
  //消息尾
  struct SZB_Tail {
    uint32_t Checksum;  //校验和
  };

  //登录消息
  struct SZB_Logon {
    SZB_Head header;                  //消息头，msgType=1
    uint8_t SenderCompID[20];         //发送方代码
    uint8_t TargetCompID[20];         //接收方代码
    int32_t HeartBtInt;               //心跳间隔，单位是秒。用户行情系统登陆时提供给行情网关
    uint8_t Password[16];             //密码
    uint8_t DefaultApplVerID[32];     //二进制协议版本
  };

  //注销消息
  struct SZB_Logout {
    SZB_Head header;          //消息头，msgType=2
    int32_t SessionStatus;    //退出时的会话状态
    uint8_t Text[200];        //文本
  };

  //心跳消息
  struct SZB_Heartbeat {
    SZB_Head header;     //消息头，msgType=3,bodylength=0
  };

  //频道心跳,频道心跳发送间隔为 3 秒。
  struct SZB_Channel_Heartbeat {
    SZB_Head header;     //消息头,msgType=390095
    uint16_t ChannelNo;  //频道代码
    int64_t ApplLastSeqNum; //最后一条行情消息的记录号
    uint8_t EndOfChannel;   //频道结束标志
  };

  //重传消息，VSS 申请逐笔行情数据或公告信息重传消息定义
  struct SZB_Resend {
    SZB_Head header;            //消息头，msgType=390094
    uint8_t ResendType;         //重传种类 1 = 逐笔行情数据 2 = 公告信息
    uint16_t ChannelNo;         //频道代码
    int64_t ApplBegSeqNum;      //起始序号,当ResendType = 1 时生效,指定记录序号的范围。

    int64_t ApplendSeqNum;      //结束序号当 ResendType = 1 
                                //时生效指定记录序号的范围.当 ApplEndSeqNum = 0 时，
                                //行情网关会将 ApplEndSeqNum 设置为收到重传请求时， 
                                //该频道数据在内存中的最大值。

    uint8_t NewsID[8];          // 公告唯一标识当 ResendType=2 时生效为空时表示申请公告概要
    uint8_t ResendStatus;       // 重传状态
                                // 仅在行情网关前置机返回给用户
                                // 行情系统服务器时有效
                                // 1 = 完成
                                // 2 = 部分完成（有部分请求的数据
                                // 没有返回）
                                // 3 = 无权限
                                // 4 = 数据不可用

    uint8_t RejectText[16];         //  文本
                                    //  仅在行情网关前置机返回给用户
                                    //  行情系统服务器时有效
                                    //  如果请求被行情网关前置机拒绝，
                                    //  错误代码在这个域返回

  };

  //MDGW 定时向 VSS 发送快照行情频道统计消息定义
  //快照行情频道统计消息发送间隔为 15 秒。
  struct SZB_Channel_MD_Statistic {
    SZB_Head header;                  //消息头，msgType=390090
    int64_t OrigTime;                 //数据生成时间，本地时间戳YYYYMMDDHHMMSSsss（毫秒）
    uint16_t ChannelNo;               //频道代码
    uint32_t NoMDStreamID;        //行情类别个数
    struct {
      uint8_t MDStreamID[3];          //行情类别
      uint32_t StockNum;              //证券只数
      uint8_t TradingPhaseCode[8];    //  闭市状态 第 0 位 T = 连续竞价（全部证券尚未闭市) E = 已闭市（全部证券已闭市）
    } MDStreamItems[];                //行情类别数组
  };

  //业务拒绝消息
  //当一个应用层消息满足会话层规则但不满足业务层规则时，使用业务拒绝消息进行
  //拒绝。该消息主要用于对用户申报的错误的重传消息和用户信息报告消息进行回报。
  struct SZB_Business_Reject {
    SZB_Head header;  //消息头，msgType=8
    int64_t RefSeqNum;        // 被拒绝消息的消息序号
    uint32_t RefMsgType;       //被拒绝的消息类型
    uint8_t BusinessRejectRefID[10];  //被拒绝消息对应的业务层 ID。
    uint16_t BusinessRejectReason; //拒绝原因
    uint8_t BusinessRejectText[50];   //拒绝原因说明
  };


  //证券实时状态 MsgType=390013
  struct SZB_Security_Status {
    SZB_Head header;               //消息头，msgType=390013
    int64_t OrigTime;                // 数据生成时间
    uint16_t ChannelNo;              // 频道代码
    uint8_t SecurityID[8];              // 证券代码
    uint8_t SecurityIDSource[4];       // 证券代码源 101=深圳证券交易所
    uint8_t FinancialStatus[8];         // 每个字节揭示一种状态，最多可同时
                                      // 每个字节揭示八种状态
    uint32_t NoSwitch;               // 开关个数
    struct {
      uint16_t SecuritySwitchType;
      uint16_t SecuritySwitchStatus;
    } switchs[];                     // 开关数组

  };
  // 公告 MsgType=390012
  struct SZB_Bulletin {
    SZB_Head header;                  //消息头，msgType=390012
    int64_t OrigTime;                // 数据生成时间
    uint16_t ChannelNo;              // 频道代码
    uint8_t NewsID[8];                  // 公告ID
    uint8_t Headline[128];               // 公告标题
    uint8_t RawDataFormat[8];          // 二进制数据格式，取值为TXT、PDF、DOC等等
    uint32_t RawDataLength;         // 二进制数据长度
    uint8_t RawData[];                 // 二进制数据
  };

  // 快照行情

  struct SZB_SnapShot_Head {
    SZB_Head header;                  //消息头，msgType=3xxx11
    int64_t OrigTime;                // 数据生成时间
    uint16_t ChannelNo;              // 频道代码
    uint8_t MDStreamID[3];             // 行情类别
    uint8_t SecurityID[8];              // 证券代码
    uint8_t SecurityIDSource[4];       // 证券代码源 101=深圳证券交易所
    uint8_t TradingPhaseCode[8];       // 产品所处的交易阶段代码 第0位：S=启动（开市前） O=开盘集合竞价 
                                      // T=连续竞价 B=休市 C=收盘集合竞价 E=已闭市 H=临时停牌 A=盘后交易
                                      // 第1位：0=正常状态 1=全天停牌
    int64_t PrevClosePx;            // 昨收价
    int64_t NumTrades;               // 成交笔数
    int64_t TotalVolumeTrade;       // 成交总量
    int64_t TotalValueTrade;        // 成交总金额
  };

  // 4.4.3.1 集中竞价业务行情快照 MsgType=300111

  // MDStreamID=010 现货（股票，基金，债券等）集中竞价交易快照
  // MDStreamID=020 质押式回购交易快照行情
  // MDStreamID=030 债券分销快照行情
  // MDStreamID=040 期权集中竞价交易快照行情

  struct MDEntry{
    uint8_t MDEntryType[2];            // 行情条目类别 0=买入 1=卖出 2=最近价 4=开盘价 7=最高价 8=最低价
                                       // x1= 升跌一 x2=升跌二 x3=买入汇总（总量及加权平均价）
                                       // x4=卖出汇总（总量及加权平均价） x5=股票市盈率一 x6=股票市盈率二 
                                       // x7=基金T-1日净值 x8=基金实时参考净值(包括ETF的IOPV)
                                       // x9=权证溢价率 xe=涨停价 xf=跌停价 xg=合约持仓量
    int64_t MDEntryPx;              // 价格
    int64_t MDEntrySize;            // 数量
    uint16_t MDPriceLevel;          // 买卖盘档位
    int64_t NumberOfOrders;         // 价位总委托笔数 为0表示不揭示
    uint32_t NoOrders;               // 价位揭示委托笔数,为0表示不揭示
    int64_t OrderQty[0];             // 委托数量,个数由uNoOrders决定
  };

  struct SZBSnapshot {
    SZB_SnapShot_Head head;             // 公共快照头 
    uint32_t NoMDEntries;           // 行情条目个数
                                    // SZBQMDEntryItem entry_items[0];
    //MDEntry SnapShotEntries[];
  };

  // 4.4.3.2 盘后定价交易业务行情快照 MsgType=300611

  struct SZB_BTSnapshot {
    SZB_SnapShot_Head head;             // 公共快照头 
    uint32_t NoMDEntries;           // 行情条目个数
    struct {
      uint8_t MDEntryType[2];          // 行情条目类别 0=买入 1=卖出
      int64_t MDEntryPx;            // 价格
      int64_t MDEntrySize;          // 数量
    } BTSnapshotEntries[0];
  };

  // 4.4.3.3 指数行情快照 MsgType=309011
  struct SZB_IndexSnapshot {
    SZB_SnapShot_Head head;             // 公共快照头 MsgType=309011
    uint32_t NoMDEntries;           // 行情条目个数
    struct {
      uint8_t MDEntryType[2];          // 行情条目类别 0=买入 1=卖出
      int64_t MDEntryPx;            // 价格
    } IndexSnapshotEntries[0];
  };

  // 4.4.3.3 成交量统计行情快照 MsgType=309111
  struct SZB_Deal_Statistic_Snapshot {
    SZB_SnapShot_Head head;             // 公共快照头 MsgType=309011
    uint32_t NoMDEntries;           // 行情条目个数
    uint32_t DealStatisticSnapshotEntries[0]; //统计量指标样本个数
  };
  // the common head for step order
  struct SZB_StepOrder_Head {
    SZB_Head header;                   //消息头
    uint16_t ChannelNo;              // 频道代码
    int64_t ApplSeqNum;              // 消息记录号 从1开始计数
    uint8_t MDStreamID[3];             // 行情类别
    uint8_t SecurityID[8];              // 证券代码
    uint8_t SecurityIDSource[4];       // 证券代码源 101=深圳证券交易所
    int64_t Price;                    // 委托价格
    int64_t OrderQty;                // 委托数量
    uint8_t Side;                        // 买卖方向 1=买 2=卖 G=借入 F=借出
    int64_t TransactTime;            // 委托时间
  };

  // 4.4.4 逐笔委托行情 MsgType=30XX92
  // 4.4.4.1 集中竞价业务逐笔委托 MsgType=300192

  // MDStreamID=011 现货（股票，基金，债券等）集中竞价交易逐笔行情
  // MDStreamID=021 质押式回购交易逐笔行情
  // MDStreamID=041 期权集中竞价交易逐笔行情
  struct SZB_StepOrder {
    SZB_StepOrder_Head head;            // the head
    uint8_t ordType;                    // 订单类别 1=市价 2=限价 U=本方最优
  };

  struct SZB_BTStepOrder {
    SZB_StepOrder_Head head;            // the head
    uint8_t ConfirmID[8];               // 定价行情约定号
    uint8_t Contactor[12];               // 联系人
    uint8_t ContactInfo[30];            // 联系方式
  };

  struct SZB_ASStepOrder {
    SZB_StepOrder_Head head;            // the head
    uint16_t ExpirationDays;         // 期限
    uint8_t ExpirationType;          // 期限类型
  };

  // 4.4.5 逐笔成交行情 MsgType=30XX91
  // MsgType=300191

  // MDStreamID=011 现货（股票，基金，债券等）集中竞价交易逐笔行情
  // MDStreamID=021 质押式回购交易逐笔行情
  // MDStreamID=041 期权集中竞价交易逐笔行情
  // MsgType=300591
  // MsgType=300791
  struct SZB_StepTrade {
    SZB_Head header;                   //消息头
    uint16_t ChannelNo;              // 频道代码
    int64_t ApplSeqNum;              // 消息记录号 从1开始计数
    uint8_t MDStreamID[3];             // 行情类别
    int64_t BidApplSeqNum;          // 买方委托索引,从1开始计数,0表示无对应委托
    int64_t OfferApplSeqNum;        // 卖方委托索引,从1开始计数,0表示无对应委托
    uint8_t SecurityID[8];              // 证券代码
    uint8_t SecurityIDSource[4];       // 证券代码源 101=深圳证券交易所
    int64_t LastPx;                  // 委托价格
    int64_t LastQty;                 // 委托数量
    uint8_t ExecType;                   // 成交类别 4=撤销 F=成交
    int64_t TransactTime;            // 委托时间

  };

#pragma pack(pop)
}// namespace umdgw



#endif // !__UMDGW_SZBINARY_message_t_DEFINE_HPP_INLCUDED__

