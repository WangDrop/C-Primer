#ifndef FC_2_H
#define FC_2_H

typedef unsigned int UINT32;
typedef unsigned short UINT16;
typedef unsigned char UINT8;
const unsigned int MAX_SQ_LEN = 8192;
const unsigned int RET_FAIL = 0x0001;
const unsigned int OXID_MAX = 0x00FF;
const unsigned int OXID_MIN = 0x0000;
const unsigned int XID_UNKNOWN = 0xFFFF;
//const unsigned int LOW_EIGHT = 0x00FF;
const unsigned int RX_DESC_NUM_APP = 10000;
const unsigned int FC_MAX_LEN = 2048;
const unsigned int ZERO = 0x0000;
const unsigned int SEQID_MAX = 50;
const PortID PORT_ID_UNKNOWN = {0, 0, 0};
enum modeSendStatus{SEND_SUCCESS, SEND_FAILED};
enum exchStatus{ FC_EXCH_FREE, FC_EXCH_BUSY };
enum portStatus{ PORT_RDY, PORT_UNINIT };
enum seqStatus{ FC_SEQ_FREE, FC_SEQ_SENDING };
enum frameStatus{ FC_FRAME_SENDING, FC_FRAME_LAST };
enum isNewExch{ EXCH_NEW, EXCH_OLD };
enum FC1553Mode{NC_NT_00, NC_NT_01, NC_NT_10, NT_NC, 
	NT_NT_01, NT_NT_10, NONE_DATA_MODE, T_M_W_D, R_M_W_D};
enum seqPos{FIRST_SEQ, MIDDLE, LAST_SEQ};
bool port0RecvSeq;
bool port1RecvSeq;


/*全局变量*/
bool NEW_EXCH = true;
UINT32 seqNum = 0;
UINT32 exchNum = 0;

/*本地端口的信息*/
struct LocalPortStatus{
	UINT16        min_xid = ZERO;
	UINT16        max_xid = OXID_MAX;
	UINT16        next_id = ZERO;
	//struct fcExch * exchPointer[OXID_MAX];//想想还是下面这一种比较好一点
	struct fcExch exchPointer[OXID_MAX];
	portStatus status;
	isNewExch newExchange;
};
LocalPortStatus port[2];

/*Exchange结构体*/
struct fcExch{
	UINT16 xid;
	UINT16 oxid;
	UINT16 rxid;
	//UINT16 initializerID;//事先就应该填好的
	//UINT16 targetID;
	struct PortID  sid;
	struct PortID  did;
	UINT32 seqid;
	fcSeq fcSequence[SEQID_MAX];
	//struct fcSeq  *seq[SEQID_MAX];
	void * configPointer;		//Information of the data
	exchStatus status = FC_EXCH_FREE;
};
fcExch exchangesPort0[OXID_MAX];
fcExch exchangesPort1[OXID_MAX];

/*Sequence结构体*/
struct fcSeq{
	UINT8 seqid;
	UINT16 seq_cnt;
	//fcExch * exchangePointer = nullptr;
	seqStatus status = FC_SEQ_FREE;
	seqPos seqPos = MIDDLE;
	frameStatus fStatus = FC_FRAME_LAST;
};
//fcSeq fcSequence[SEQID_MAX];

/*将要发送的数据的结构体*/
//struct fc_exch_snd_data {
//	void *configPointer;  //Information of the data
//	UINT32  len;          //length of payload
//};

struct fc1553_data_info{
	UINT32 addr;
	UINT32 len;
	PortID sid;
	PortID did;
};

/*端口的相关信息*/
struct PortID
{
	UINT8 id0;
	UINT8 id1;
	UINT8 id2;
};

bool fc_exch_alloc(struct fcExch * exchP);
void fc_exch_alloc(struct fcExch * ep, LocalPortStatus * lps);
static UINT32 ntoh24(const UINT8 *p);
static struct fcSeq *fc_seq_alloc(struct fcExch * ep, UINT8 seq_id);
UINT32 fc_seq_send(UINT32 addr, UINT32 len, FC_1553_config * pconfig);
UINT32 fc_port_init(LocalPortStatus * localPortStatus);
UINT32 fcae_1553_mode_send(FC1553Mode mode, UINT32 addr, UINT32 len, FC_1553_config * pconfig);
void fc_seq_free(fcSeq * sp);
void fc_exch_free(fcExch * ep);


struct 	FC_1553_config
{
	unsigned char    r_ctl;
	struct PortID    d_id;
	unsigned char    cs_ctl;
	struct PortID    s_id;
	unsigned int	 fc_params3;	//TYPE和F_CTL字段，1553序列TYPE应为0x48
	unsigned char    seq_id;
	unsigned char    df_ctl;
	unsigned short   seq_cnt;
	unsigned short   ox_id;
	unsigned short   rx_id;
	unsigned int     fc_params4;	//相对偏移量，命令序列和状态序列中该字段值为0x00000000
	unsigned int     fc_1553_params;
	unsigned int     subaddr;		//子地址/模式字段
	unsigned int	 data_len;			//数据大小/模式码字段
	unsigned int     NT_status;		//NT-NT模式的数据交换，表示发送NT的状态,状态帧设置。注意这是状态帧的字段,命令帧可以不管它
	unsigned int     max_size;		//最大可接收字节数或NT-NT模式的数据交换中接收NT的状态，状态帧设置
	unsigned int	 mul_addr;		//多播地址或其他端口号
	unsigned int     other_subaddr;	//其他子地址
	unsigned int     addr;
	unsigned char	 critical_msg;
	unsigned char	 channel;
	unsigned char	 priority_queue;
	unsigned char	 class_of_service;
	unsigned int     payloadlen;
	unsigned int     row;
	unsigned int     framelen;
};

/*struct fc_frame_header {
UINT8          fh_r_ctl;	//routing control
UINT8          fh_d_id[3];	// Destination ID

UINT8          fh_cs_ctl;	// class of service control / pri
UINT8          fh_s_id[3];	// Source ID

UINT8          fh_type;		// see enum fc_fh_type below
UINT8          fh_f_ctl[3];	// frame control

UINT8          fh_seq_id;	// sequence ID
UINT8          fh_df_ctl;	// data field control
UINT16        fh_seq_cnt;	// sequence count

UINT16        fh_ox_id;		// originator exchange ID
UINT16        fh_rx_id;		// responder exchange ID
UINT32        fh_parm_offset;	// parameter or relative offset
};*/



#endif