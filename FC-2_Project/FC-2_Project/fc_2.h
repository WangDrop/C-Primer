#ifndef FC_2_H
#define FC_2_H
typedef unsigned long UINT32;
typedef unsigned short UINT16;
typedef unsigned char UINT8;
const unsigned long EXCH_MAX_SQ_LEN = 8192;
const unsigned long RET_FAIL = 0x0001;
const unsigned long OXID_MAX = 0x00FF;
const unsigned long XID_UNKNOWN = 0xFFFF;
const unsigned long FC_MAX_LEN = 2048;
const unsigned short ZERO = 0;
enum exch_status{ FC_EXCH_FREE, FC_EXCH_BUSY, FC_EX_SQ_ACTIVE };
enum port_status{ PORT_OK, PORT_UNINIT};
enum seqStatus{SEQ_IDLE, FC_SEQ_SENDING};


struct fcSeq{
	UINT8 seqid;
	struct fc_frame_header *snd_header;
	UINT8 *snd_payload;
	UINT32 snd_len;
	UINT32 snd_offset;
	UINT16 snd_cnt;
	seqStatus status = SEQ_IDLE;
};
fc_frame frameTable[1000];

//typedef struct fc_frame{
//	UINT8 *data; /*  include frame_header and data_field  */
//	UINT32 len; /*  fc frame header length and data field, not include sof and fc_crc and eof  */
//	UINT32 fr_sof;
//	UINT32 fr_eof;
//	UINT32 fr_crc;
//} fc_frame;

struct fcExch{
	UINT16 xid;
	UINT16 oxid;
	UINT16 rxid;
//	UINT16 initializerID;
	UINT32 sid;
	UINT32 did;
	UINT32 seqid;
	struct fcSeq * seq;
	exch_status status = FC_EXCH_FREE;
};

/*本地端口的信息*/
struct LocalPortStatus{
	UINT16        min_xid = 0;
	UINT16        max_xid = OXID_MAX;
	UINT16        next_id = 0;
	struct fcExch * exchPointer;
	port_status status;

};

struct fc_frame_header {
	UINT8          fh_r_ctl;	/* routing control */
	UINT8          fh_d_id[3];	/* Destination ID */

	UINT8          fh_cs_ctl;	/* class of service control / pri */
	UINT8          fh_s_id[3];	/* Source ID */

	UINT8          fh_type;		/* see enum fc_fh_type below */
	UINT8          fh_f_ctl[3];	/* frame control */

	UINT8          fh_seq_id;	/* sequence ID */
	UINT8          fh_df_ctl;	/* data field control */
	UINT16        fh_seq_cnt;	/* sequence count */

	UINT16        fh_ox_id;		/* originator exchange ID */
	UINT16        fh_rx_id;		/* responder exchange ID */
	UINT32        fh_parm_offset;	/* parameter or relative offset */
};

struct fc_exch_snd_data {
	struct fc_frame_header *header;    /*header*/
	UINT8  *payload;           /*payload*/
	UINT32  len;          /*length of payload*/
};

bool fc_exch_alloc(struct fcExch * exchP);
void fc_exch_alloc(struct fcExch * ep, LocalPortStatus * lps);
static UINT32 ntoh24(const UINT8 *p);
static struct fcSeq *fc_seq_alloc(struct fcExch * ep, UINT8 seq_id);
#endif