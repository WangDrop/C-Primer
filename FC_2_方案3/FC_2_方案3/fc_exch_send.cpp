#include "fc_1553.h"
//UINT32 fc_exch_send(struct fc_exch_snd_data * data, LocalPortStatus * lps)//这里传入的应该是port[0]的status。
//{
//	if ((*lps).status == PORT_UNINIT){
//		fc_port_init(lps);
//	}
//	static struct fcExch *ep = nullptr;
//	static struct fcSeq *sp = nullptr;
//	if ((*lps).newExchange == EXCH_NEW){
//		(*lps).newExchange = EXCH_OLD;//这个在点击交互按钮的时候应该把它设置成true
//		fc_exch_alloc(ep, lps);		  //在1553中重新选择交互模式的里面在把它清成0；
//		if (!ep || !ep->fcSequence[seqNum]){
//			return RET_FAIL;/*exch以及seq分配失败*/
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
//}//Exch的Free是在函数的外面去做的

UINT32 fcae_1553_mode_send(FC1553Mode mode, fc1553_data_info * data , FC_1553_config * pconfig)
{
	static int i = 0;
	if (port[0].status = PORT_UNINIT){
		fc_port_init(&port[0]);	//端口信息可以定义成全局的，直接访问
	}
	static struct fcExch *ep = nullptr;
	static struct fcSeq *sp = nullptr;
	if ((port[0]).newExchange == EXCH_NEW){
		(port[0]).newExchange = EXCH_OLD;//这个在点击交互按钮的时候应该把它设置成true
		fc_exch_alloc(ep, &port[0]);		  //在1553中重新选择交互模式的里面在把它清成0；
		if (!ep || !ep->fcSequence[seqNum]){
			return RET_FAIL;/*exch以及seq分配失败*/
		}
		//sp = ep->seq;
		sp = ep->fcSequence[seqNum];
		sp->seqPos = FIRST_SEQ;//刚刚第一个exch中的第一个seq，设其为第一个seq。
		ep->oxid = ep->xid;
		ep->seqid++;
		ep->status = FC_EXCH_BUSY;
		sp->status = FC_SEQ_SENDING;

		//配置pconfig之中的信息
		if (data->len > MAX_SQ_LEN){
			pconfig->payloadlen = MAX_SQ_LEN;
			pconfig->addr = data->addr;
			data->len -= MAX_SQ_LEN;
			data->addr += MAX_SQ_LEN;//这些地方的单位都需要看一下啊啊啊
		}
		else{
			pconfig->payloadlen = data->len;
			data->addr += data->len;
			data->len = 0;
		}
		pconfig->r_ctl = 0x0048;	//这个填充的对吗
		pconfig->d_id = data->did;
		pconfig->s_id = data->sid;
		pconfig->seq_id = ep->seqid;
		pconfig->seq_cnt = 0;
		pconfig->ox_id = ep->oxid;
		pconfig->rx_id = ep->rxid;
		
	}
	//else{ //不要因为新的版本是阻塞在下面的
	//	ep = port[0].exchPointer[exchNum];
	//	sp = fc_seq_alloc(ep);
	//	ep->seqid++;
	//	/*ep->seq = sp;*///这个暂时是不要的啊啊。现在是通过数组的方式
	//	ep->status = FC_EXCH_BUSY;
	//	sp->status = FC_SEQ_SENDING;
	//}
	fc_seq_send(mode, sp, pconfig);
	/*fc_seq_send(mode ,sp, data);*/
	while (i < (data->len / MAX_SQ_LEN + 1)){
		if ("收到一个序列的话:可以用Qt的型号机制解决"){
			ep = port[0].exchPointer[exchNum];
			sp = fc_seq_alloc(ep);
			ep->seqid++;
			/*ep->seq = sp;*///这个暂时是不要的啊啊。现在是通过数组的方式
			ep->status = FC_EXCH_BUSY;
			sp->status = FC_SEQ_SENDING;
			fc_seq_send(mode, sp, pconfig);
			++i;
		}
	}
	//fc_exch_free()
	//{
	//	/*
	//	将exch的status等等信息
	//	都全部都置位才行。
	//	*/
	//}
	return SEND_SUCCESS;
}

void fc_exch_alloc(struct fcExch * ep, LocalPortStatus * lps)
{
	while (lps->exchPointer[lps->next_id].status != FC_EXCH_FREE){	//找到第一个free的xid
		lps->next_id = lps->next_id == OXID_MAX ? OXID_MIN : lps->next_id + 1;
	}	//在接收或者是交换完成的时候需要将上面给的FC_EXCH_FREE重置

	ep = &lps->exchPointer[lps->next_id];
	ep->xid = lps->next_id;//oxid在发送的时候分配
	ep->rxid = XID_UNKNOWN;
	//PortID由上一层来填
	ep->status = FC_EXCH_BUSY;
	ep->seqid = 0;
	fc_seq_init(ep);
	/*ep->seq = fc_seq_alloc(ep);*/
	//上面这句事实上应该换成fc_seq_init,这个以后在加上。
	lps->next_id = lps->next_id == OXID_MAX ? OXID_MIN : lps->next_id + 1;
}

void fc_seq_init(struct fcExch * ep)
{
	for (int i = 0; i < SEQID_MAX; i++){
		ep->fcSequence[i].seqid = 0;//在上层之后增长了吗？
		ep->fcSequence[i].seq_cnt = 0;//下层发送的时候暂时就不增长了。
		ep->fcSequence[i].status = FC_SEQ_FREE;//使用了之后要将其置为非FREE
		ep->fcSequence[i].seqPos = MIDDLE;
		ep->fcSequence[i].fStatus = FC_FRAME_LAST;//暂时不考虑切割等等情况
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
		lpStatus->exchPointer[i].seqid = ZERO;	//这个有个最大值，seqid max
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
	while (sp->status != FC_SEQ_FREE){	//考虑一下free掉sequence怎么处理
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
		else if("还需要发送数据帧"){
			//fc_ae_1553_send_data(seqPointer, pconfig);
		}
		else{
			//fc_ae_1553-send_status(seqPointer, pconfig);
		}
		break;

	case NC_NT_01:

	}

	//受限不考虑切割的问题啊啊 啊
	//if (seqPointer->seqPos == FIRST_SEQ){
	//	fc_1553_send_order;
	//}
	//while (data->len > FC_MAX_LEN){
	//	seqPointer->fStatus = FC_FRAME_SENDING;
	//	data->len -= FC_MAX_LEN;//调用等时候应该给这个
	//	seqPointer->seq_cnt++;
	//	fc_frame_send(seqPointer, data->configPointer);
	//}
	//if (data->len > 0){
	//	seqPointer->fStatus = FC_FRAME_LAST;
	//	seqPointer->seq_cnt++;
	//	fc_frame_send(seqPointer, data->configPointer);
	//}
	//seqPointer->status = FC_SEQ_FREE;  在这里释放对不对？
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
//			fc1553Config->payloadlen = FC_MAX_LEN;//每个帧只发这么长的数据
//			Send1553(hDevice1, *fc1553Config);
//			fc1553Config->addr += FC_MAX_LEN;//发完之后更改一下偏移地址。
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
	//对config字段做一定的配置，之后就可以发送了，具体的照着原来的填充命令字的来做就行了
	Send1553(hDevice1, pconfig);
}