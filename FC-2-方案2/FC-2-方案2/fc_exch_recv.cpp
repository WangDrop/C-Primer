#include <iostream>
#include "FC_2.h"
UINT32 fc_exch_recv(UINT8 * status, UINT32 * delay, UINT32 * buf_offset, UINT32 * len, LocalPortStatus * lps)
{
	fcExch * tmpEp = new fcExch;
	static fcExch * ep = nullptr;
	UINT32 totalLenth = 0;
	fc_seq_recv(status, delay, buf_offset, len, tmpEp);
	totalLenth += *len;
	while ((tmpEp->seq->status == FC_SEQ_SENDING)){
		fc_seq_recv(status, delay, buf_offset, len, tmpEp);
		totalLenth += *len;
	}
	*len = totalLenth;
	unsigned char* ptr = ptrdata1 + buf_offset;
	tmpEp->oxid = *((UINT16 *)(ptr + 16));
	get_free_xid(tmpEp);
	tmpEp->rxid = tmpEp->xid;
	tmpEp->sid = *((PortID *)(ptr + 1));	//将did转换成远端的sid
	tmpEp->did = *((PortID *)(ptr + 5));
	tmpEp->seqid = *((UINT8 *)(ptr + 12));
	tmpEp->configPointer = lps;
	tmpEp->status = FC_EXCH_BUSY;
	
	
	if ((*lps).status == PORT_UNINIT){
		fc_exch_init(lps);
	}
	if ((*lps).newExchange == EXCH_NEW){
		(*lps).newExchange = EXCH_OLD;//这个在点击交互按钮的时候应该把它设置成true
		fc_exch_alloc(ep, lps);
		if (!ep){
			return RET_FAIL;/*exch分配失败*/
		}
		*ep = *tmpEp;
		delete tmpEp;
		ep->seqid++;
		ep->status = FC_EX_SQ_ACTIVE;
		ep->seq->status = FC_SEQ_SENDING;
	}
	else{
		sp = fc_seq_alloc(ep);	//写到这里了，但是暂时还有很多的细节没有完成。
		*ep = *tmpEp;
		delete tmpEp;
		ep->seqid++;
		ep->status = FC_EX_SQ_ACTIVE;
		ep->seq->status = FC_SEQ_SENDING;
	}

	
	fc_seq_send(sp, data);
}

UINT32 fc_seq_recv(UINT8 * status, UINT32 * delay, UINT32 * buf_offset, UINT32 * len, fcExch * ep)
{
	fcSeq * sp = new fcSeq;	 //这个当发送完成之后应该记得要去释放他。
	unsigned char* ptr = ptrdata1 + buf_offset;
	fc_frame_recv(status, delay, buf_offset, len, sp);
	ep->seq = sp;
	return 0;
}

UINT32 fc_frame_recv(UINT8 * status, UINT32 * delay, UINT32 * buf_offset, UINT32 * len, fcSeq * sp)
{
	get_current_rx_status(status, delay, buf_offset, len);
	unsigned char* ptr = ptrdata1 + buf_offset;
	sp->seq_cnt = (UINT16)(ptr + 14);
	sp->seqid = (UINT8)(ptr + 12);
	sp->exchangePointer = nullptr;
}

int get_current_rx_status(unsigned char *status, unsigned int *delay, unsigned int *buf_offset, unsigned int *len)
{
	rx_app_desc_buf_virt = (struct rx_ctl_tab_app *)((char*)ptrdata1 + 0x80000);//0x80000相当于位置0，//接收方向ELS帧描述符队列rx_ctl_tab_app
	long long rd_ptr = rx_app_desc_buf_virt->read_head;             /* get read pointer of desc */
	long long wr_ptr = rx_app_desc_buf_virt->write_head;            /* get write pointer of desc */
	if (rd_ptr < wr_ptr)//如果是读指针数小于写指针数的话
	{
		//qDebug()<<"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
		//*wr = wr_ptr;
		//*rd = rd_ptr;
		rd_ptr = rx_app_desc_buf_virt->read_head%RX_DESC_NUM_APP; //最后这个宏定义的值是10000，相当于循环存储
		*status = rx_app_desc_buf_virt->entity[rd_ptr].status;
		*len = rx_app_desc_buf_virt->entity[rd_ptr].len;
		*buf_offset = rx_app_desc_buf_virt->entity[rd_ptr].addr;

		// (*m_cntf)++;//应该是帧的计数相当于
		rx_app_desc_buf_virt->read_head++;

		if (rx_app_desc_buf_virt->read_head == rx_app_desc_buf_virt->write_head)
		{
			QueryPerformanceFrequency(&Frequency);
			QueryPerformanceCounter(&Counter_end);
			//qDebug()<<"Counter_end: "<<Counter_end.QuadPart<<"Frequency: "<<Frequency.QuadPart;
			*delay = (int)(((Counter_end.QuadPart - Counter_start.QuadPart) * 1000000) / Frequency.QuadPart);
			//qDebug()<<"delay:"<<*delay<<" us";
		}
		return 1;
	}

	return 0;
}

UINT32 get_free_xid(fcExch * exchangePointer)
{
	for (int i = 0; i < OXID_MAX; i++){
		if (exchanges[i].status == FC_EXCH_FREE){
			exchangePointer->xid = i;
			return 0;
		}
	}
	return 1;
}