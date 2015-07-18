#include "fc_2.h"
#include <iostream>

UINT32 fc_exch_send(struct fc_exch_snd_data *data, LocalPortStatus * lps, void *  configPointer)
{
	if ((*lps).status == PORT_UNINIT){
		fc_exch_init(lps);
		(*lps).status = PORT_OK;
	}
	struct fcExch *ep = nullptr;
	struct fcSeq *sp = nullptr;
	fc_exch_alloc(ep, lps);
	if (!ep){
		return RET_FAIL;/*exch分配失败*/
	}
	ep->seqid++;
	sp = ep->seq;
	sp->snd_header = data->header;
	sp->snd_payload = data->payload;
	sp->snd_len = data->len;
	sp->snd_offset = 0;
	fc_exch_set_addr(ep, ntoh24(sp->snd_header->fh_s_id), ntoh24(sp->snd_header->fh_d_id));
	sp->snd_header->fh_cs_ctl = 0;
	ep->status = FC_EX_SQ_ACTIVE;
	sp->status = FC_SEQ_SENDING;
	if (!fc_seq_send(sp)){//这个明天来实现啊啊啊
		fc_seq_send_over_deal(sp, 0);//释放资源  写到这里啊啊
	}
}


static struct fcSeq *fc_seq_alloc(struct fcExch * ep, UINT8 seq_id)
{
	struct fcSeq * sp;
	sp = ep->seq;
	sp->seqid = seq_id;
	sp->snd_header = NULL;
	sp->snd_payload = NULL;
	sp->snd_len = 0;
	sp->snd_offset = 0;
	sp->snd_cnt = 0;
	return sp;
}

UINT32 fc_seq_send(fcSeq * sp)
{
	struct fc_frame *fp;
	struct fc_frame_header *fh;
	UINT32 remaining = 0;
	UINT32 t_len = FC_MAX_LEN, tlen = 0;
	void *data;
	remaining = sp->snd_len - sp->snd_offset;
	while (remaining > 0){
		tlen = t_len < remaining ? t_len : remaining;
	}
}

static void fc_exch_set_addr(struct fcExch *ep, UINT32 orig_id, UINT32 resp_id)
{
		ep->sid = orig_id;
		ep->did = resp_id;
}

void fc_exch_alloc(struct fcExch * ep, LocalPortStatus * lps)
{
	while (!(lps->exchPointer[lps->next_id].status == FC_EXCH_FREE)){
		lps->next_id = lps->next_id == OXID_MAX ? 0 : lps->next_id + 1;
	}
	ep = &lps->exchPointer[lps->next_id];
	lps->next_id = lps->next_id == OXID_MAX ? 0 : lps->next_id + 1;
	ep->rxid = XID_UNKNOWN;
	lps->exchPointer[lps->next_id].status = FC_EXCH_BUSY;
	ep->seqid = 0;
	fc_seq_alloc(ep, ep->seqid);
}
struct fc_frame *fc_frame_alloc(UINT32 len)
{
	struct fc_frame *fp = NULL;

	/*
	* Note: Since len will often be a constant multiple of 4,
	* this check will usually be evaluated and eliminated at compile time.
	*/
}

bool fc_exch_init(LocalPortStatus * localPortStatus)
{
	LocalPortStatus lpStatus;
	if (!(lpStatus.exchPointer = new fcExch[OXID_MAX]))
		return RET_FAIL;
	for (UINT32 i = 0; i < OXID_MAX; i++){
		lpStatus.exchPointer[i].rxid = XID_UNKNOWN;
		lpStatus.exchPointer[i].status = FC_EXCH_FREE;
	}
	return 0;
}
		
static UINT32 ntoh24(const UINT8 *p)
{
	return (p[0] << 16) | (p[1] << 8) | p[2];
}


