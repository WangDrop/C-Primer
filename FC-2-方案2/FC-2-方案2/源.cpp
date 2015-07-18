#include "FC_2.h"
UINT32 seqNum = 0;


UINT32 fc_exch_send(struct fc_exch_snd_data *data, LocalPortStatus * lps)
{
	if ((*lps).status == PORT_UNINIT){
		fc_exch_init(lps);
	}
	static struct fcExch *ep = nullptr;
	static struct fcSeq *sp = nullptr;
	if ((*lps).newExchange == EXCH_NEW){
		(*lps).newExchange = EXCH_OLD;//这个在点击交互按钮的时候应该把它设置成true
		fc_exch_alloc(ep, lps);
		if (!ep){
			return RET_FAIL;/*exch分配失败*/
		}
		ep->oxid = ep->xid;
		ep->seqid++;
		sp = ep->seq;
		ep->status = FC_EX_SQ_ACTIVE;
		sp->status = FC_SEQ_SENDING;
	}
	else{
		sp = fc_seq_alloc(ep);
		ep->seqid++;
		ep->seq = sp;
		sp = ep->seq;
		ep->status = FC_EX_SQ_ACTIVE;
		sp->status = FC_SEQ_SENDING;
	}
	fc_seq_send(sp, data);
}

bool fc_exch_init(LocalPortStatus * localPortStatus)
{
	LocalPortStatus lpStatus;
	lpStatus.exchPointer = 
		;
	for (UINT32 i = 0; i < OXID_MAX; i++){
		lpStatus.exchPointer[i].rxid = XID_UNKNOWN;
		lpStatus.exchPointer[i].status = FC_EXCH_FREE;
	}
	(*localPortStatus).status = PORT_RDY;
	return 0;
}

void fc_exch_alloc(struct fcExch * ep, LocalPortStatus * lps)
{
	while (!(lps->exchPointer[lps->next_id].status == FC_EXCH_FREE)){
		lps->next_id = lps->next_id == OXID_MAX ? OXID_MIN : lps->next_id + 1;
	}	//在接收或者是交换完成的时候需要将上面给的FC_EXCH_FREE重置
	ep = &lps->exchPointer[lps->next_id];
	ep->xid = lps->next_id;
	ep->rxid = XID_UNKNOWN;
	lps->exchPointer[lps->next_id].status = FC_EXCH_BUSY;
	ep->seqid = 0;
	ep->seq = fc_seq_alloc(ep, ep->seqid);
	lps->next_id = lps->next_id == OXID_MAX ? 0 : lps->next_id + 1;
}

static struct fcSeq *fc_seq_alloc(struct fcExch * ep)
{

	struct fcSeq * sp = (seqNum == 100) ? &fcSequence[(seqNum++) % 100] : &fcSequence[seqNum++];
	sp->seqid = ep->seqid;
	sp->seq_cnt = 0;
	sp->exchangePointer = ep;
	return sp;
}

UINT32 fc_seq_send(fcSeq * seqPointer, struct fc_exch_snd_data *data)
{
	while (data->len > FC_MAX_LEN){
		seqPointer->status = FC_SEQ_SENDING;
		data->len -= FC_MAX_LEN;//调用等时候应该给这个
		seqPointer->seq_cnt++;
		fc_frame_send(seqPointer, data->configPointer);
	}
	if (data->len > 0){
		seqPointer->status = FC_SEQ_IDLE;
		seqPointer->seq_cnt++;
		fc_frame_send(seqPointer, data->configPointer);
	}
	seqPointer->status = FC_SEQ_IDLE;
}


void fc_frame_send(fcSeq * seqPointer, void * configPointer)
{
	if ((UINT8)((UINT8*)configPointer + 8) == 0X48){
		FC_1553_config  * fc1553Config = (FC_1553_config*)configPointer;
		fc1553Config->s_id.id0 = seqPointer->exchangePointer->sid.id0;
		fc1553Config->s_id.id1 = seqPointer->exchangePointer->sid.id1;
		fc1553Config->s_id.id2 = seqPointer->exchangePointer->sid.id2;
		fc1553Config->d_id.id0 = seqPointer->exchangePointer->did.id0;
		fc1553Config->d_id.id1 = seqPointer->exchangePointer->did.id1;
		fc1553Config->d_id.id2 = seqPointer->exchangePointer->did.id2;
		fc1553Config->ox_id = seqPointer->exchangePointer->oxid;
		fc1553Config->rx_id = seqPointer->exchangePointer->rxid;
		fc1553Config->seq_id = seqPointer->exchangePointer->seqid;
		fc1553Config->seq_cnt = seqPointer->seq_cnt;
		fc1553Config->payloadlen = FC_MAX_LEN;//每个帧只发这么长的数据
		Send1553(hDevice1, *fc1553Config);
		fc1553Config->addr += FC_MAX_LEN;//发完之后更改一下偏移地址。
	}
	else if{

	}
}

static UINT32 ntoh24(const UINT8 *p)
{
	return (p[0] << 16) | (p[1] << 8) | p[2];
}
