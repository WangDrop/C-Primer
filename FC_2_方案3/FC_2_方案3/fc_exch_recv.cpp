#include <iostream>
#include "fc_1553.h"

//发送端不做切割，但是接收端应该做帧额度重新组装，切割在驱动部分完成

fcExch tmpExch;
fcSeq tmpSeq;
UINT32 fc_exch_recv(UINT8 * status, UINT32 * delay, UINT32 * buf_offset, UINT32 * len, LocalPortStatus * lps)
{
	if ((*lps).status == PORT_UNINIT){
		fc_port_init(lps);
	}
	fcExch * tmpEp = &tmpExch;
	static fcExch * ep = nullptr;
	//UINT32 totalLenth = 0;
	fc_seq_recv(status, delay, buf_offset, len, tmpEp);
	//totalLenth += *len;
	/*while ((tmpEp->seq->status == FC_SEQ_SENDING)){
	fc_seq_recv(status, delay, buf_offset, len, tmpEp);
	totalLenth += *len;
	}*/
	//*len = totalLenth;
	unsigned char* ptr = ptrdata1 + *buf_offset;
	tmpEp->oxid = *((UINT16 *)(ptr + 16));
	tmpEp->rxid = *((UINT16 *)(ptr + 18));//下面如果不是0xFFFF的话就是第一次，应该寻找并且分配一个rxid
	if (tmpEp->rxid == XID_UNKNOWN){
		get_free_xid(tmpEp);
		tmpEp->rxid = tmpEp->xid;
	}
	tmpEp->sid = *((PortID *)(ptr + 1));	//将did转换成远端的sid
	tmpEp->did = *((PortID *)(ptr + 5));
	tmpEp->seqid = *((UINT8 *)(ptr + 12)) + 1;	//seq_id还是上面的seq_id,不用清成0
	tmpEp->configPointer = lps;
	tmpEp->seq = nullptr;
	tmpEp->status = FC_EXCH_BUSY;
	*port[1].exchPointer = *tmpEp;
	//	port[1].status = PORT_RDY;
	port[1].newExchange = EXCH_OLD;
	return 0;
}


UINT32 fc_ae_1553_mode_recv(UINT8 * status, UINT32 * delay, UINT32 * buf_offset, UINT32 * len, LocalPortStatus * lps)
{
	fcExch * ep = &tmpExch;
	//fcSeq * sp = &tmpSeq;
	fc_seq_recv(status, delay, buf_offset, len, ep);
}

UINT32 fc_seq_recv(UINT8 * status, UINT32 * delay, UINT32 * buf_offset, UINT32 * len, fcExch * ep)
{
	fcSeq * sp = &tmpSeq;	 //这个当发送完成之后应该记得要去释放他。
	fc_frame_recv(status, delay, buf_offset, len, sp);
	unsigned char* ptr = ptrdata1 + *buf_offset;
	ep->seq = sp;
	sp->exchangePointer = ep;
	return 0;
}

UINT32 fc_frame_recv(UINT8 * status, UINT32 * delay, UINT32 * buf_offset, UINT32 * len, fcSeq * sp)
{
	get_current_rx_status(status, delay, buf_offset, len);
	unsigned char* ptr = ptrdata1 + *buf_offset;
	sp->seq_cnt = (UINT16)(ptr + 14);
	sp->seqid = (UINT8)(ptr + 12);
	sp->status = FC_SEQ_SENDING;
	sp->seqPos = FIRST_SEQ;
	sp->fStatus = FC_FRAME_LAST;
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
		if (exchangesPort1[i].status == FC_EXCH_FREE && i != exchangePointer->oxid){
			exchangePointer->xid = i;
			return 0;
		}
	}
	return 1;
}