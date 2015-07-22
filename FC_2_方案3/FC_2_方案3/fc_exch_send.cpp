#include "fc_1553.h"
//UINT32 fc_exch_send(struct fc_exch_snd_data * data, LocalPortStatus * lps)//���ﴫ���Ӧ����port[0]��status��
//{
//	if ((*lps).status == PORT_UNINIT){
//		fc_port_init(lps);
//	}
//	static struct fcExch *ep = nullptr;
//	static struct fcSeq *sp = nullptr;
//	if ((*lps).newExchange == EXCH_NEW){
//		(*lps).newExchange = EXCH_OLD;//����ڵ��������ť��ʱ��Ӧ�ð������ó�true
//		fc_exch_alloc(ep, lps);		  //��1553������ѡ�񽻻�ģʽ�������ڰ������0��
//		if (!ep || !ep->fcSequence[seqNum]){
//			return RET_FAIL;/*exch�Լ�seq����ʧ��*/
//		}
//		//sp = ep->seq;
//		sp = ep->fcSequence[seqNum];
//		ep->oxid = ep->xid;
//		ep->seqid++;
//		ep->status = FC_EXCH_BUSY;
//		sp->status = FC_SEQ_SENDING;
//	}
//	else{
//		ep = lps->exchPointer[exchNum];
//		sp = fc_seq_alloc(ep);
//		ep->seqid++;
//		ep->seq = sp;
//		ep->status = FC_EXCH_BUSY;
//		sp->status = FC_SEQ_SENDING;
//	}
//	fc_seq_send(sp, data);
//	fc_seq_free(sp);
//}//Exch��Free���ں���������ȥ����

UINT32 fcae_1553_mode_send(FC1553Mode mode, fc1553_data_info * data , FC_1553_config * pconfig)
{
	static int i = 0;
	if (port[0].status = PORT_UNINIT){
		fc_port_init(&port[0]);	//�˿���Ϣ���Զ����ȫ�ֵģ�ֱ�ӷ���
	}
	static struct fcExch *ep = nullptr;
	static struct fcSeq *sp = nullptr;
	if ((port[0]).newExchange == EXCH_NEW){
		(port[0]).newExchange = EXCH_OLD;//����ڵ��������ť��ʱ��Ӧ�ð������ó�true
		fc_exch_alloc(ep, &port[0]);		  //��1553������ѡ�񽻻�ģʽ�������ڰ������0��
		if (!ep || !ep->fcSequence[seqNum]){
			return RET_FAIL;/*exch�Լ�seq����ʧ��*/
		}
		//sp = ep->seq;
		sp = ep->fcSequence[seqNum];
		sp->seqPos = FIRST_SEQ;//�ոյ�һ��exch�еĵ�һ��seq������Ϊ��һ��seq��
		ep->oxid = ep->xid;
		ep->seqid++;
		ep->status = FC_EXCH_BUSY;
		sp->status = FC_SEQ_SENDING;

		//����pconfig֮�е���Ϣ
		if (data->len > MAX_SQ_LEN){
			pconfig->payloadlen = MAX_SQ_LEN;
			pconfig->addr = data->addr;
			data->len -= MAX_SQ_LEN;
			data->addr += MAX_SQ_LEN;//��Щ�ط��ĵ�λ����Ҫ��һ�°�����
		}
		else{
			pconfig->payloadlen = data->len;
			data->addr += data->len;
			data->len = 0;
		}
		pconfig->r_ctl = 0x0048;	//������Ķ���
		pconfig->d_id = data->did;
		pconfig->s_id = data->sid;
		pconfig->seq_id = ep->seqid;
		pconfig->seq_cnt = 0;
		pconfig->ox_id = ep->oxid;
		pconfig->rx_id = ep->rxid;
		
	}
	//else{ //��Ҫ��Ϊ�µİ汾�������������
	//	ep = port[0].exchPointer[exchNum];
	//	sp = fc_seq_alloc(ep);
	//	ep->seqid++;
	//	/*ep->seq = sp;*///�����ʱ�ǲ�Ҫ�İ�����������ͨ������ķ�ʽ
	//	ep->status = FC_EXCH_BUSY;
	//	sp->status = FC_SEQ_SENDING;
	//}
	fc_seq_send(mode, sp, pconfig);
	/*fc_seq_send(mode ,sp, data);*/
	while (i < (data->len / MAX_SQ_LEN + 1)){
		if ("�յ�һ�����еĻ�:������Qt���ͺŻ��ƽ��"){
			ep = port[0].exchPointer[exchNum];
			sp = fc_seq_alloc(ep);
			ep->seqid++;
			/*ep->seq = sp;*///�����ʱ�ǲ�Ҫ�İ�����������ͨ������ķ�ʽ
			ep->status = FC_EXCH_BUSY;
			sp->status = FC_SEQ_SENDING;
			fc_seq_send(mode, sp, pconfig);
			++i;
		}
	}
	//fc_exch_free()
	//{
	//	/*
	//	��exch��status�ȵ���Ϣ
	//	��ȫ������λ���С�
	//	*/
	//}
	return SEND_SUCCESS;
}

void fc_exch_alloc(struct fcExch * ep, LocalPortStatus * lps)
{
	while (lps->exchPointer[lps->next_id].status != FC_EXCH_FREE){	//�ҵ���һ��free��xid
		lps->next_id = lps->next_id == OXID_MAX ? OXID_MIN : lps->next_id + 1;
	}	//�ڽ��ջ����ǽ�����ɵ�ʱ����Ҫ���������FC_EXCH_FREE����

	ep = &lps->exchPointer[lps->next_id];
	ep->xid = lps->next_id;//oxid�ڷ��͵�ʱ�����
	ep->rxid = XID_UNKNOWN;
	//PortID����һ������
	ep->status = FC_EXCH_BUSY;
	ep->seqid = 0;
	fc_seq_init(ep);
	/*ep->seq = fc_seq_alloc(ep);*/
	//���������ʵ��Ӧ�û���fc_seq_init,����Ժ��ڼ��ϡ�
	lps->next_id = lps->next_id == OXID_MAX ? OXID_MIN : lps->next_id + 1;
}

void fc_seq_init(struct fcExch * ep)
{
	for (int i = 0; i < SEQID_MAX; i++){
		ep->fcSequence[i].seqid = 0;//���ϲ�֮����������
		ep->fcSequence[i].seq_cnt = 0;//�²㷢�͵�ʱ����ʱ�Ͳ������ˡ�
		ep->fcSequence[i].status = FC_SEQ_FREE;//ʹ����֮��Ҫ������Ϊ��FREE
		ep->fcSequence[i].seqPos = MIDDLE;
		ep->fcSequence[i].fStatus = FC_FRAME_LAST;//��ʱ�������и�ȵ����
	}
}

UINT32 fc_port_init(LocalPortStatus * localPortStatus)
{
	LocalPortStatus *lpStatus = localPortStatus;
	lpStatus->exchPointer = exchangesPort0;
	for (UINT32 i = 0; i < OXID_MAX; i++){
		lpStatus->exchPointer[i].xid = ZERO;
		lpStatus->exchPointer[i].oxid = ZERO;
		lpStatus->exchPointer[i].rxid = XID_UNKNOWN;
		lpStatus->exchPointer[i].sid = PORT_ID_UNKNOWN;
		lpStatus->exchPointer[i].did = PORT_ID_UNKNOWN;
		lpStatus->exchPointer[i].seqid = ZERO;	//����и����ֵ��seqid max
		//lpStatus->exchPointer[i].seq = nullptr;
		lpStatus->exchPointer[i].configPointer = nullptr;
		lpStatus->exchPointer[i].status = FC_EXCH_FREE;
	}
	(*localPortStatus).status = PORT_RDY;
	return 0;
}


static struct fcSeq * fc_seq_alloc(struct fcExch * ep)
{
	struct fcSeq * sp = &ep->fcSequence[(seqNum++) % 100];
	while (sp->status != FC_SEQ_FREE){	//����һ��free��sequence��ô����
		struct fcSeq * sp = &ep->fcSequence[(seqNum++) % 100];
	}
	sp->seqid = ep->seqid;
	sp->seq_cnt = 0;
	/*sp->exchangePointer = ep;*/
	return sp;
}

UINT32 fc_seq_send(FC1553Mode  mode,  fcSeq * seqPointer, FC_1553_config * pconfig)
{
	switch (mode){
	case NC_NT_00:
		if (pconfig->seq_id == 0){
			//fc_ae_1553_send_order(seqPointer, pconfig);
		}
		else if("����Ҫ��������֡"){
			//fc_ae_1553_send_data(seqPointer, pconfig);
		}
		else{
			//fc_ae_1553-send_status(seqPointer, pconfig);
		}
		break;

	case NC_NT_01:

	}

	//���޲������и�����Ⱑ�� ��
	//if (seqPointer->seqPos == FIRST_SEQ){
	//	fc_1553_send_order;
	//}
	//while (data->len > FC_MAX_LEN){
	//	seqPointer->fStatus = FC_FRAME_SENDING;
	//	data->len -= FC_MAX_LEN;//���õ�ʱ��Ӧ�ø����
	//	seqPointer->seq_cnt++;
	//	fc_frame_send(seqPointer, data->configPointer);
	//}
	//if (data->len > 0){
	//	seqPointer->fStatus = FC_FRAME_LAST;
	//	seqPointer->seq_cnt++;
	//	fc_frame_send(seqPointer, data->configPointer);
	//}
	//seqPointer->status = FC_SEQ_FREE;  �������ͷŶԲ��ԣ�
}

void fc_seq_free(fcSeq * sp)
{
	sp->seqid = ZERO;
	sp->seq_cnt = ZERO;
	sp->status = FC_SEQ_FREE;
	sp->fStatus = FC_FRAME_LAST;
}


//void fc_frame_send(fcSeq * seqPointer, void * configPointer)
//{
//	if ((UINT8)((UINT8*)configPointer + 8) == 0X48){
//		FC_1553_config  * fc1553Config = (FC_1553_config*)configPointer;
//		fc1553Config->ox_id = seqPointer->exchangePointer->oxid;
//		fc1553Config->rx_id = seqPointer->exchangePointer->rxid;
//		fc1553Config->seq_id = seqPointer->exchangePointer->seqid;
//		fc1553Config->seq_cnt = seqPointer->seq_cnt;
//		if (seqPointer->fStatus == FC_FRAME_SENDING){
//			fc1553Config->payloadlen = FC_MAX_LEN;//ÿ��ֻ֡����ô��������
//			Send1553(hDevice1, *fc1553Config);
//			fc1553Config->addr += FC_MAX_LEN;//����֮�����һ��ƫ�Ƶ�ַ��
//		}
//		else{
//			Send1553(hDevice1, *fc1553Config);
//		}
//
//	}
//	else if{
//
//	}
//}

void fc_ae_1553_send_order(fcSeq * seqPointer, FC_1553_config * pconfig)
{
	//��config�ֶ���һ�������ã�֮��Ϳ��Է����ˣ����������ԭ������������ֵ�����������
	Send1553(hDevice1, pconfig);
}