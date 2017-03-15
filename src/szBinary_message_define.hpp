#ifndef __UMDGW_SZBINARY_message_t_DEFINE_HPP_INLCUDED__
#define __UMDGW_SZBINARY_message_t_DEFINE_HPP_INLCUDED__
#include<stdint.h>

namespace umdgw {
#pragma warning(disable:4200) //zero-sized array in struct/union
#pragma pack(push,1)

  //��Ϣͷ
  struct SZB_Head {
    uint32_t MsgType; //��Ϣ����
    uint32_t BodyLength; //��Ϣ�峤��
  };
  
  //��Ϣβ
  struct SZB_Tail {
    uint32_t Checksum;  //У���
  };

  //��¼��Ϣ
  struct SZB_Logon {
    SZB_Head header;                  //��Ϣͷ��msgType=1
    uint8_t SenderCompID[20];         //���ͷ�����
    uint8_t TargetCompID[20];         //���շ�����
    int32_t HeartBtInt;               //�����������λ���롣�û�����ϵͳ��½ʱ�ṩ����������
    uint8_t Password[16];             //����
    uint8_t DefaultApplVerID[32];     //������Э��汾
  };

  //ע����Ϣ
  struct SZB_Logout {
    SZB_Head header;          //��Ϣͷ��msgType=2
    int32_t SessionStatus;    //�˳�ʱ�ĻỰ״̬
    uint8_t Text[200];        //�ı�
  };

  //������Ϣ
  struct SZB_Heartbeat {
    SZB_Head header;     //��Ϣͷ��msgType=3,bodylength=0
  };

  //Ƶ������,Ƶ���������ͼ��Ϊ 3 �롣
  struct SZB_Channel_Heartbeat {
    SZB_Head header;     //��Ϣͷ,msgType=390095
    uint16_t ChannelNo;  //Ƶ������
    int64_t ApplLastSeqNum; //���һ��������Ϣ�ļ�¼��
    uint8_t EndOfChannel;   //Ƶ��������־
  };

  //�ش���Ϣ��VSS ��������������ݻ򹫸���Ϣ�ش���Ϣ����
  struct SZB_Resend {
    SZB_Head header;            //��Ϣͷ��msgType=390094
    uint8_t ResendType;         //�ش����� 1 = ����������� 2 = ������Ϣ
    uint16_t ChannelNo;         //Ƶ������
    int64_t ApplBegSeqNum;      //��ʼ���,��ResendType = 1 ʱ��Ч,ָ����¼��ŵķ�Χ��

    int64_t ApplendSeqNum;      //������ŵ� ResendType = 1 
                                //ʱ��Чָ����¼��ŵķ�Χ.�� ApplEndSeqNum = 0 ʱ��
                                //�������ػὫ ApplEndSeqNum ����Ϊ�յ��ش�����ʱ�� 
                                //��Ƶ���������ڴ��е����ֵ��

    uint8_t NewsID[8];          // ����Ψһ��ʶ�� ResendType=2 ʱ��ЧΪ��ʱ��ʾ���빫���Ҫ
    uint8_t ResendStatus;       // �ش�״̬
                                // ������������ǰ�û����ظ��û�
                                // ����ϵͳ������ʱ��Ч
                                // 1 = ���
                                // 2 = ������ɣ��в������������
                                // û�з��أ�
                                // 3 = ��Ȩ��
                                // 4 = ���ݲ�����

    uint8_t RejectText[16];         //  �ı�
                                    //  ������������ǰ�û����ظ��û�
                                    //  ����ϵͳ������ʱ��Ч
                                    //  ���������������ǰ�û��ܾ���
                                    //  �������������򷵻�

  };

  //MDGW ��ʱ�� VSS ���Ϳ�������Ƶ��ͳ����Ϣ����
  //��������Ƶ��ͳ����Ϣ���ͼ��Ϊ 15 �롣
  struct SZB_Channel_MD_Statistic {
    SZB_Head header;                  //��Ϣͷ��msgType=390090
    int64_t OrigTime;                 //��������ʱ�䣬����ʱ���YYYYMMDDHHMMSSsss�����룩
    uint16_t ChannelNo;               //Ƶ������
    uint32_t NoMDStreamID;        //����������
    struct {
      uint8_t MDStreamID[3];          //�������
      uint32_t StockNum;              //֤ȯֻ��
      uint8_t TradingPhaseCode[8];    //  ����״̬ �� 0 λ T = �������ۣ�ȫ��֤ȯ��δ����) E = �ѱ��У�ȫ��֤ȯ�ѱ��У�
    } MDStreamItems[];                //�����������
  };

  //ҵ��ܾ���Ϣ
  //��һ��Ӧ�ò���Ϣ����Ự����򵫲�����ҵ������ʱ��ʹ��ҵ��ܾ���Ϣ����
  //�ܾ�������Ϣ��Ҫ���ڶ��û��걨�Ĵ�����ش���Ϣ���û���Ϣ������Ϣ���лر���
  struct SZB_Business_Reject {
    SZB_Head header;  //��Ϣͷ��msgType=8
    int64_t RefSeqNum;        // ���ܾ���Ϣ����Ϣ���
    uint32_t RefMsgType;       //���ܾ�����Ϣ����
    uint8_t BusinessRejectRefID[10];  //���ܾ���Ϣ��Ӧ��ҵ��� ID��
    uint16_t BusinessRejectReason; //�ܾ�ԭ��
    uint8_t BusinessRejectText[50];   //�ܾ�ԭ��˵��
  };


  //֤ȯʵʱ״̬ MsgType=390013
  struct SZB_Security_Status {
    SZB_Head header;               //��Ϣͷ��msgType=390013
    int64_t OrigTime;                // ��������ʱ��
    uint16_t ChannelNo;              // Ƶ������
    uint8_t SecurityID[8];              // ֤ȯ����
    uint8_t SecurityIDSource[4];       // ֤ȯ����Դ 101=����֤ȯ������
    uint8_t FinancialStatus[8];         // ÿ���ֽڽ�ʾһ��״̬������ͬʱ
                                      // ÿ���ֽڽ�ʾ����״̬
    uint32_t NoSwitch;               // ���ظ���
    struct {
      uint16_t SecuritySwitchType;
      uint16_t SecuritySwitchStatus;
    } switchs[];                     // ��������

  };
  // ���� MsgType=390012
  struct SZB_Bulletin {
    SZB_Head header;                  //��Ϣͷ��msgType=390012
    int64_t OrigTime;                // ��������ʱ��
    uint16_t ChannelNo;              // Ƶ������
    uint8_t NewsID[8];                  // ����ID
    uint8_t Headline[128];               // �������
    uint8_t RawDataFormat[8];          // ���������ݸ�ʽ��ȡֵΪTXT��PDF��DOC�ȵ�
    uint32_t RawDataLength;         // ���������ݳ���
    uint8_t RawData[];                 // ����������
  };

  // ��������

  struct SZB_SnapShot_Head {
    SZB_Head header;                  //��Ϣͷ��msgType=3xxx11
    int64_t OrigTime;                // ��������ʱ��
    uint16_t ChannelNo;              // Ƶ������
    uint8_t MDStreamID[3];             // �������
    uint8_t SecurityID[8];              // ֤ȯ����
    uint8_t SecurityIDSource[4];       // ֤ȯ����Դ 101=����֤ȯ������
    uint8_t TradingPhaseCode[8];       // ��Ʒ�����Ľ��׽׶δ��� ��0λ��S=����������ǰ�� O=���̼��Ͼ��� 
                                      // T=�������� B=���� C=���̼��Ͼ��� E=�ѱ��� H=��ʱͣ�� A=�̺���
                                      // ��1λ��0=����״̬ 1=ȫ��ͣ��
    int64_t PrevClosePx;            // ���ռ�
    int64_t NumTrades;               // �ɽ�����
    int64_t TotalVolumeTrade;       // �ɽ�����
    int64_t TotalValueTrade;        // �ɽ��ܽ��
  };

  // 4.4.3.1 ���о���ҵ��������� MsgType=300111

  // MDStreamID=010 �ֻ�����Ʊ������ծȯ�ȣ����о��۽��׿���
  // MDStreamID=020 ��Ѻʽ�ع����׿�������
  // MDStreamID=030 ծȯ������������
  // MDStreamID=040 ��Ȩ���о��۽��׿�������

  struct MDEntry{
    uint8_t MDEntryType[2];            // ������Ŀ��� 0=���� 1=���� 2=����� 4=���̼� 7=��߼� 8=��ͼ�
                                       // x1= ����һ x2=������ x3=������ܣ���������Ȩƽ���ۣ�
                                       // x4=�������ܣ���������Ȩƽ���ۣ� x5=��Ʊ��ӯ��һ x6=��Ʊ��ӯ�ʶ� 
                                       // x7=����T-1�վ�ֵ x8=����ʵʱ�ο���ֵ(����ETF��IOPV)
                                       // x9=Ȩ֤����� xe=��ͣ�� xf=��ͣ�� xg=��Լ�ֲ���
    int64_t MDEntryPx;              // �۸�
    int64_t MDEntrySize;            // ����
    uint16_t MDPriceLevel;          // �����̵�λ
    int64_t NumberOfOrders;         // ��λ��ί�б��� Ϊ0��ʾ����ʾ
    uint32_t NoOrders;               // ��λ��ʾί�б���,Ϊ0��ʾ����ʾ
    int64_t OrderQty[0];             // ί������,������uNoOrders����
  };

  struct SZBSnapshot {
    SZB_SnapShot_Head head;             // ��������ͷ 
    uint32_t NoMDEntries;           // ������Ŀ����
                                    // SZBQMDEntryItem entry_items[0];
    //MDEntry SnapShotEntries[];
  };

  // 4.4.3.2 �̺󶨼۽���ҵ��������� MsgType=300611

  struct SZB_BTSnapshot {
    SZB_SnapShot_Head head;             // ��������ͷ 
    uint32_t NoMDEntries;           // ������Ŀ����
    struct {
      uint8_t MDEntryType[2];          // ������Ŀ��� 0=���� 1=����
      int64_t MDEntryPx;            // �۸�
      int64_t MDEntrySize;          // ����
    } BTSnapshotEntries[0];
  };

  // 4.4.3.3 ָ��������� MsgType=309011
  struct SZB_IndexSnapshot {
    SZB_SnapShot_Head head;             // ��������ͷ MsgType=309011
    uint32_t NoMDEntries;           // ������Ŀ����
    struct {
      uint8_t MDEntryType[2];          // ������Ŀ��� 0=���� 1=����
      int64_t MDEntryPx;            // �۸�
    } IndexSnapshotEntries[0];
  };

  // 4.4.3.3 �ɽ���ͳ��������� MsgType=309111
  struct SZB_Deal_Statistic_Snapshot {
    SZB_SnapShot_Head head;             // ��������ͷ MsgType=309011
    uint32_t NoMDEntries;           // ������Ŀ����
    uint32_t DealStatisticSnapshotEntries[0]; //ͳ����ָ����������
  };
  // the common head for step order
  struct SZB_StepOrder_Head {
    SZB_Head header;                   //��Ϣͷ
    uint16_t ChannelNo;              // Ƶ������
    int64_t ApplSeqNum;              // ��Ϣ��¼�� ��1��ʼ����
    uint8_t MDStreamID[3];             // �������
    uint8_t SecurityID[8];              // ֤ȯ����
    uint8_t SecurityIDSource[4];       // ֤ȯ����Դ 101=����֤ȯ������
    int64_t Price;                    // ί�м۸�
    int64_t OrderQty;                // ί������
    uint8_t Side;                        // �������� 1=�� 2=�� G=���� F=���
    int64_t TransactTime;            // ί��ʱ��
  };

  // 4.4.4 ���ί������ MsgType=30XX92
  // 4.4.4.1 ���о���ҵ�����ί�� MsgType=300192

  // MDStreamID=011 �ֻ�����Ʊ������ծȯ�ȣ����о��۽����������
  // MDStreamID=021 ��Ѻʽ�ع������������
  // MDStreamID=041 ��Ȩ���о��۽����������
  struct SZB_StepOrder {
    SZB_StepOrder_Head head;            // the head
    uint8_t ordType;                    // ������� 1=�м� 2=�޼� U=��������
  };

  struct SZB_BTStepOrder {
    SZB_StepOrder_Head head;            // the head
    uint8_t ConfirmID[8];               // ��������Լ����
    uint8_t Contactor[12];               // ��ϵ��
    uint8_t ContactInfo[30];            // ��ϵ��ʽ
  };

  struct SZB_ASStepOrder {
    SZB_StepOrder_Head head;            // the head
    uint16_t ExpirationDays;         // ����
    uint8_t ExpirationType;          // ��������
  };

  // 4.4.5 ��ʳɽ����� MsgType=30XX91
  // MsgType=300191

  // MDStreamID=011 �ֻ�����Ʊ������ծȯ�ȣ����о��۽����������
  // MDStreamID=021 ��Ѻʽ�ع������������
  // MDStreamID=041 ��Ȩ���о��۽����������
  // MsgType=300591
  // MsgType=300791
  struct SZB_StepTrade {
    SZB_Head header;                   //��Ϣͷ
    uint16_t ChannelNo;              // Ƶ������
    int64_t ApplSeqNum;              // ��Ϣ��¼�� ��1��ʼ����
    uint8_t MDStreamID[3];             // �������
    int64_t BidApplSeqNum;          // ��ί������,��1��ʼ����,0��ʾ�޶�Ӧί��
    int64_t OfferApplSeqNum;        // ����ί������,��1��ʼ����,0��ʾ�޶�Ӧί��
    uint8_t SecurityID[8];              // ֤ȯ����
    uint8_t SecurityIDSource[4];       // ֤ȯ����Դ 101=����֤ȯ������
    int64_t LastPx;                  // ί�м۸�
    int64_t LastQty;                 // ί������
    uint8_t ExecType;                   // �ɽ���� 4=���� F=�ɽ�
    int64_t TransactTime;            // ί��ʱ��

  };

#pragma pack(pop)
}// namespace umdgw



#endif // !__UMDGW_SZBINARY_message_t_DEFINE_HPP_INLCUDED__

