#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/time.h>

#include <aui_common.h>
#include <aui_smc.h>

#include "va_setup.h"
#include "va_sc.h"
#include <ngl_types.h>
#include <ngl_log.h>

NGL_MODULE(VASC)

#define MAX_ATR_LN (33)
#define MAX_RESP_LEN (260)
#define PPS_MAX_LEN (6)
#define PPS_MIN_LEN (3)
#define PPSS ((char)0x0FF)

#define CMD_MAX_LEN (512)


#define VA_SC_SLEEP(x) usleep((x) * (1000))


#define CARD_STATUS_IDLE            0
#define CARD_STATUS_RESETING        (1<<0)
#define CARD_STATUS_RESET           (1<<1)
#define CARD_STATUS_GET_STATING     (1<<2)
#define CARD_STATUS_GOT_STATE       (1<<3)
#define CARD_STATUS_SEND_CMDING     (1<<4)
#define CARD_STATUS_SENT_CMD        (1<<5)
#define CARD_STATUS_ACTIVING        (1<<6)
#define CARD_STATUS_ACTIVED         (1<<7)
#define CARD_STATUS_DEACTIVING      (1<<8)
#define CARD_STATUS_DEACTIVED       (1<<9)
#undef NGLOG_DUMP
#define NGLOG_DUMP(a,b,c)

typedef struct va_sc_slot_data{
    aui_hdl handle;
    //SCIL_notify_t notify;
    unsigned long smc_id;
    int reseted;
    int protocol;
    int card_exist;
    int lock;
    int notify_card_status; // if card has got notify status
    unsigned long card_status;
    UINT8 pps1_value;
    char pps_buf[PPS_MAX_LEN];
    UINT8 cmd_buf[CMD_MAX_LEN];
    UINT32 cmd_size;
}va_sc_slot_data;

static va_sc_slot_data m_va_sc_slot[kVA_SETUP_NBMAX_SC];


static int m_conversion_factor[] = {
    372, 372, 558, 744, 1116, 1488, 1860, -1, -1, 512, 768, 1024, 1536, 2048, -1, -1
};
// Di=64 is valid in ISO/IEC 7816-3:2006
static int m_adjustment_factor[] = {
    -1, 1, 2, 4, 8, 16, 32, 64, 12, 20, -1, -1, -1, -1, -1, -1
};

static int m_sc_init = 0;

static pthread_mutex_t m_sc_mutex;
static pthread_cond_t  m_sc_cond;

static void va_sc_wait_send_cmd(DWORD dwScSlot)
{
    if (m_va_sc_slot[dwScSlot].lock){
        NGLOG_DEBUG("before wait the unlock of send command!\n");
 	pthread_mutex_lock(&m_sc_mutex);
	pthread_cond_wait(&m_sc_cond, &m_sc_mutex);
	pthread_mutex_unlock(&m_sc_mutex);
        NGLOG_DEBUG("after wait the unlock of send command!\n");
    }
}

static AUI_RTN_CODE *smc_init_cb(void *para)
{

    aui_smc_device_cfg_t *pconfig = (aui_smc_device_cfg_t*)para;
    NGLOG_VERBOSE("M3527 viaccess. running smc_init_cb\n");
    //pconfig++; // 3733 use slot 1
    pconfig->init_clk_trigger = 1;
    pconfig->init_clk_number = 1;
    pconfig->apd_disable_trigger = 1;
    pconfig->def_etu_trigger = 1;
    pconfig->default_etu = 372;
    pconfig->warm_reset_trigger = 1;
    pconfig->init_clk_array[0] = 3600000;
    
    pconfig->invert_detect = 1;

    pconfig->force_tx_rx_trigger = 1;
    pconfig->force_tx_rx_cmd = 0xdd;
    pconfig->force_tx_rx_cmd_len = 5;

    pconfig->disable_pps = 1;

    NGLOG_VERBOSE("disable_pps: %d\n", pconfig->disable_pps);
    return AUI_RTN_SUCCESS;
}


static void va_sc_status_fun_cb(const unsigned int slot_num, unsigned int status)
{
    INT ret;
    aui_hdl hdl = NULL;
    unsigned char atr[MAX_ATR_LN];
    unsigned short atr_len = 0;
    
    NGLOG_VERBOSE("smart card %d %s (status = %u)\n",slot_num, status ? "detected" : "missing",  status);

    if (status){
        ret = VA_SC_CardInserted(slot_num);
        if (kVA_OK != ret)
            NGLOG_ERROR("VA_SC_CardInserted error, ret = %d\n", (int)ret);

    	if (aui_find_dev_by_idx(AUI_MODULE_SMC, slot_num, &hdl)) {
            
            ret = aui_smc_reset(hdl, atr, &atr_len, TRUE);
            if (AUI_RTN_SUCCESS != ret){
                NGLOG_ERROR("aui_smc_reset error, ret = %d\n", (int)ret);
            }
    	}
        
    }else{
        ret = VA_SC_CardExtracted(slot_num);
        if (kVA_OK != ret)
            NGLOG_ERROR("VA_SC_CardInserted error, ret = %d\n", (int)ret);

        //the reset state should be 0 when extract the card!
        m_va_sc_slot[slot_num].reseted = 0;
    }
    
    m_va_sc_slot[slot_num].card_exist = status;
    m_va_sc_slot[slot_num].notify_card_status = 1;

}

static aui_hdl va_sc_open(DWORD dwScSlot)
{
    AUI_RTN_CODE ret = AUI_RTN_FAIL;
    aui_hdl hdl = NULL;
    aui_smc_attr smc_attr;
    int cnt;
    int sleep_count = 100;
    
    if (aui_find_dev_by_idx(AUI_MODULE_SMC, dwScSlot, &hdl)) {
        memset(&smc_attr, 0, sizeof(aui_smc_attr));
        ret = aui_smc_init((p_fun_cb)smc_init_cb);
        if (AUI_RTN_SUCCESS != ret){
            NGLOG_ERROR("aui_smc_init error, ret = %d\n", (int)ret);
            return NULL;
        }
        
    	smc_attr.p_fn_smc_cb =(aui_smc_p_fun_cb)va_sc_status_fun_cb;
    	smc_attr.ul_smc_id = dwScSlot;
    	ret = aui_smc_open(&smc_attr,&hdl);
        if (AUI_RTN_SUCCESS != ret){
            NGLOG_ERROR("aui_smc_open error, ret = %d\n", (int)ret);
            return NULL;
        }

        while (cnt ++ < sleep_count){
            if (m_va_sc_slot[dwScSlot].notify_card_status)
                break;
            VA_SC_SLEEP(10);
        }
        if (!m_va_sc_slot[dwScSlot].notify_card_status){
            NGLOG_ERROR("Can not Get Smart card status(insert/extract)!!!\n");
        }
        
    }

    if (!m_sc_init){
        if(pthread_mutex_init(&m_sc_mutex, NULL) != 0) {
            NGLOG_ERROR("pthread_mutex_init fail!\n");
            return NULL;	
        } 
        if(pthread_cond_init(&m_sc_cond, NULL) != 0) {
            NGLOG_ERROR("pthread_cond_init fail!\n");
            return NULL;
        } 
        m_sc_init = 1;
    }


    return hdl;
}

//get the smaller DN, DN < DR
static int va_sc_get_smaller_DN(int DR)
{
    int i;
    int length;
    int factor_array[32] = {0};
    int factor_cnt = 0;
    int DN = -1;
    int max = 0;
    

    //step 1: copy the values that smaller than DR to factor_array
    length = sizeof(m_adjustment_factor)/sizeof(int);
    for (i = 0; i < length; i ++) {
        if ( (m_adjustment_factor[DR] > m_adjustment_factor[i]) &&
            (m_adjustment_factor[i] != -1) ) {
            factor_array[factor_cnt] = m_adjustment_factor[i];
            factor_cnt ++;
        }
    }

    // can not find the smaller array
    if (0 == factor_cnt) {
        for (i = 0; i < length; i ++) {
            if (1 == m_adjustment_factor[i]) {
                DN = i;
                break;
            }
        }
    } else {
        max = 0;
        //step 2: find the biggest value of factor_array[]
        for (i = 0; i < factor_cnt; i ++) {
            if (factor_array[i] > max){
                max = factor_array[i];
            }
        }

        //step 3: return the DN
        for (i = 0; i < length; i ++) {
            if (max == m_adjustment_factor[i]) {
                DN = i;
                break;
            }
        }
    }
    NGLOG_DEBUG("max = %d, DN = %d\n", max, DN);
    
    return DN;
    
}

static void *va_sc_reset_thread(void *arg)
{
    AUI_RTN_CODE ret = AUI_RTN_FAIL;
    DWORD slot_num = *(DWORD*)arg;
    unsigned char TS0, TD1, index = 1;
    int T = 0;
    int set_pps = 0;
    unsigned char atr[MAX_ATR_LN];
    int write_len, read_len;
    int i;
    int va_ret;
    unsigned char FI, DI;
    int Fi, Di, etu;
    unsigned char FR, DR;
    int Fr, Dr;
    unsigned char FN, DN;
    
    unsigned char tx_buffer[4], rx_buffer[280];
    unsigned short atr_len = 0;
    UINT8 pps1_value = 0;
    int ret_val = kVA_OK;
    aui_smc_param_t smc_param;
    
    pthread_detach(pthread_self());
    NGLOG_DEBUG("slot num: %d", (unsigned int)slot_num);

    pps1_value = m_va_sc_slot[slot_num].pps1_value;
    
    //pps1_value = 0x18;    

    ret = aui_smc_reset(m_va_sc_slot[slot_num].handle, atr, &atr_len, TRUE);
    NGLOG_VERBOSE("aui_smc_reset(slot:%d)=%d pps1_value=%X" ,slot_num,ret, pps1_value);
    if (AUI_RTN_SUCCESS != ret){
        ret_val = kVA_ERROR;
        goto reset_thread_exit;
    }
    NGLOG_DUMP("ATR: ", atr, atr_len);

    FR = (pps1_value >> 4) & 0x0F;
    DR = pps1_value & 0x0F;
    Fr = m_conversion_factor[FR];
    Dr = m_adjustment_factor[DR];
    NGLOG_VERBOSE("FR:%d, DR:%d, Fr:%d, Dr:%d, Fd: 372, Dd: 1",   (int)FR, (int)DR, (int)Fr, (int)Dr);

    if (pps1_value){
	 if ((atr[1] & 0x10) && (atr_len >= 2)){
		set_pps = 1;

		FI = (atr[2] >> 4) & 0x0F;
		DI = atr[2] & 0x0F;
		Fi = m_conversion_factor[FI];
		Di = m_adjustment_factor[DI];
		if (Fr > Fi)
 		    FN = FI;
		else
		    FN = FR;
			
		if (Dr > Di)
		    DN = DI;
		else
		    DN = DR;
			
		pps1_value =  (FN << 4)| (DN & 0x0f);
	}else{
		//do not set pps if no TA[0] in ATR;
		set_pps = 0; 
		pps1_value = 0x11;
	}
	NGLOG_VERBOSE("pps1_value = 0x%x", pps1_value);
		
	//So far, according to 7816-3 SPEC, user can not modify the F/D
	// arbitrarily, and PPS is set(AT1) in smart card driver when reset,
	// so here we do not set PPS again. Else if Viaccess may return error in
	// read()
	//set_pps = 0;
		
	index = 1;//atr[0] is initial charater
	TS0 = atr[index++];
	if (TS0 & 0x10) index++;
	if (TS0 & 0x20) index++;
	if (TS0 & 0x40) index++;
	if (TS0 & 0x80) { // if T0 bit8=1, TD1 exist.
		TD1 = atr[index];
		NGLOG_VERBOSE("TD1=0x%02X", TD1);
		if(!(TD1 & 0x10)) { 
			// if TD1 bit5=0, TA2 is not exist, card in negotiable mode, can set PPS
			T = TD1 & 0x0F;
			NGLOG_VERBOSE("Smart card is negotiable mode");
		} else {
			// if TD1 bit5=1, TA2 exist, card in specific mode. can not set PPS
			NGLOG_VERBOSE("Smart card is specific mode");
			set_pps = 0;
		}
	} else {
		TD1 = 0;
		T = 0;
	}
	NGLOG_VERBOSE("set_pps=%d, TS0=0x%x", set_pps, (unsigned int)TS0);
    }else{
	//do not set pps if pps1_value = 0;
	set_pps = 0; 
    }	
    NGLOG_VERBOSE("final pps1_value = 0x%x", pps1_value);

retry_pps:
    if (set_pps){

        memset(&smc_param, 0, sizeof(smc_param));
    	memset(rx_buffer, 0x00, sizeof(rx_buffer));
        
        //send PPS request
    	write_len = 4;
    	read_len = 4;
    	tx_buffer[0] = 0xFF;
    	tx_buffer[1] = 0x10 | T;
    	tx_buffer[2] = pps1_value;
    	tx_buffer[3] = 0x00;
    	for (i = 0; i < 3; i++) {
    		tx_buffer[3] ^= tx_buffer[i];
    	}
        NGLOG_DUMP("Send PPS: ", tx_buffer, 4);

    	FI = (tx_buffer[2] >> 4) & 0x0F;
    	DI = tx_buffer[2] & 0x0F;
    	Fi = m_conversion_factor[FI];
    	Di = m_adjustment_factor[DI];

        NGLOG_VERBOSE("F:%d, D:%d, Fi:%d, Di:%d, Fd: 372, Dd: 1", (int)FI, (int)DI, (int)Fi, (int)Di);

        //send PPS command to smart card
        ret = aui_smc_setpps(m_va_sc_slot[slot_num].handle, tx_buffer, write_len, rx_buffer, &read_len);
        if (AUI_RTN_SUCCESS != ret){
            NGLOG_ERROR("aui_smc_setpps error, ret = %d", (int)ret);
            ret_val = kVA_ERROR;
            goto reset_thread_exit;
        }
        NGLOG_DUMP("Get from PPS: ", rx_buffer, 4);
        NGLOG_VERBOSE("Fn:%d, Dn:%d",(int)((rx_buffer[2] >> 4) & 0x0F), (int)(rx_buffer[2] & 0x0F));

        if (memcmp(tx_buffer, rx_buffer, 4) == 0){

    		etu = Fi / Di;
    		smc_param.m_nETU = etu;
    		if (T == 0) {
    			smc_param.m_e_protocol = EM_AUISMC_PROTOCOL_T0;
    		} else if (T == 1) {
    			smc_param.m_e_protocol = EM_AUISMC_PROTOCOL_T1;
    		}
    		ret = aui_smc_param_set(m_va_sc_slot[slot_num].handle, &smc_param);
            if (AUI_RTN_SUCCESS != ret){
                NGLOG_ERROR("aui_smc_param_set error, ret = %d", (int)ret);
                ret_val = kVA_ERROR;
                goto reset_thread_exit;
            }
            
        }else{
            // not support the pps, retry with smaller DN
            FN = (pps1_value >> 4) & 0x0F;
            DR = pps1_value & 0x0F;
            if (1 == DR){
                //DR = 1, can not set pps with the smallest value, do not need to try again
                pps1_value = m_va_sc_slot[slot_num].pps1_value;
                NGLOG_VERBOSE("can not set pps with the smallest value, do not need to try");
                
            }else{
                DN = va_sc_get_smaller_DN(DR);
                pps1_value =  (FN << 4)| (DN & 0x0f);
                NGLOG_VERBOSE("retry pps1_value=0x%x", (unsigned int)pps1_value);
                goto retry_pps;
            }
        }
    }    
    
    m_va_sc_slot[slot_num].reseted = 1;

reset_thread_exit:
    m_va_sc_slot[slot_num].card_status = CARD_STATUS_RESET;
    if (kVA_OK == ret_val){
        NGLOG_VERBOSE("pps1_value=0x%x", (unsigned int)pps1_value);
        va_ret = VA_SC_ResetDone(slot_num, atr_len, atr, pps1_value);
        if (va_ret){
            NGLOG_ERROR("VA_SC_ResetDone error, ret = %d", va_ret);
        }
    }else{
        va_ret = VA_SC_ResetFailed(slot_num);
        if (va_ret){
            NGLOG_ERROR("VA_SC_ResetFailed error, ret = %d", va_ret);
        }
    }
    
    NGLOG_VERBOSE("Reset Done!");

#if 0
    memset(atr, 0, sizeof(atr));
    ret = aui_smc_reset(m_va_sc_slot[slot_num].handle, atr, &atr_len, TRUE);
    if (AUI_RTN_SUCCESS != ret){
    NGLOG_ERROR("aui_smc_reset error, ret = %d\n", (int)ret);
    ret_val = kVA_ERROR;
    }
    else
    {
        NGLOG_DEBUG("aui_smc_reset OK!");
    }
    NGLOG_DEBUG("ATR 11!\n");
    NGLOG_DUMP("ATR: ", atr, atr_len);
#endif            

    pthread_exit(NULL);
    
}

static void *va_sc_send_cmd_thread(void *arg)
{
    AUI_RTN_CODE ret = AUI_RTN_FAIL;
    DWORD slot_num = *(DWORD*)arg;
    int read_cmd = 0;
    short actlen = 0;
    unsigned char *cmd_head = NULL;
    unsigned char cmd_head_tmp[5];
    unsigned char *cmd_data = NULL;
    short cmd_head_size;
    short cmd_data_size;
    unsigned char recv_buffer[CMD_MAX_LEN];   
    INT va_ret;
    unsigned char *cmd_response = NULL;
    UINT32 cmd_response_size;
    int i;
    int apdu_case_1 = 0;
    

    pthread_detach(pthread_self());
    
    NGLOG_VERBOSE("slot num: %d, cmd_size=%d", (unsigned int)slot_num, (int)m_va_sc_slot[slot_num].cmd_size);
    
    cmd_head_size = 5;

    if (cmd_head_size == m_va_sc_slot[slot_num].cmd_size){
        NGLOG_VERBOSE("It is smart card read cmmmand!");    
        read_cmd = 1;
    }
    
    if (m_va_sc_slot[slot_num].cmd_size == 4) {  
        memcpy(cmd_head_tmp, m_va_sc_slot[slot_num].cmd_buf, 4);
        cmd_head_tmp[4] = 0;
        cmd_head = cmd_head_tmp;
        read_cmd = 1;
        apdu_case_1 = 1;
    }else {
        cmd_head = (unsigned char*)(m_va_sc_slot[slot_num].cmd_buf);
    }
   
    cmd_data = (unsigned char*)(m_va_sc_slot[slot_num].cmd_buf + cmd_head_size);
    if (m_va_sc_slot[slot_num].cmd_size > cmd_head_size){
        cmd_data_size = m_va_sc_slot[slot_num].cmd_size - cmd_head_size;
    }else{
        //cmd_head_size = m_va_sc_slot[slot_num].cmd_size;
        cmd_data_size = 0;
    }
    
    //step1: send command head(5 bytes)
    NGLOG_DUMP("cmd_head:", cmd_head, cmd_head_size);
    ret = aui_smc_raw_write(m_va_sc_slot[slot_num].handle, cmd_head, cmd_head_size, &actlen);
    if (AUI_RTN_SUCCESS != ret){
        NGLOG_ERROR("aui_smc_raw_write error, ret = %d", (int)ret);
        goto send_thread_exit;
    }
    NGLOG_VERBOSE("actlen: %d, cmd_data_size=%d", actlen, (int)cmd_data_size);

    //step2: get command response, if no command data, no need to get command head response,
    // get SW1 and SW2 is enough.
    if (cmd_data_size || read_cmd){
        for (i = 0; i < 10; i++){
            ret = aui_smc_raw_read(m_va_sc_slot[slot_num].handle, recv_buffer, 1, &actlen);
            if (AUI_RTN_SUCCESS != ret){
                NGLOG_ERROR("aui_smc_raw_read error, ret = %d", (int)ret);
                goto send_thread_exit;
            }
             
            if (recv_buffer[0] == 0x60) {
                NGLOG_VERBOSE("NULL BYTE detected");
                continue;        
            }else if ((apdu_case_1 == 1) && (recv_buffer[0] == cmd_head[1])){
                NGLOG_VERBOSE("ACK received");
                continue;
            }else if ((apdu_case_1 == 1) && (recv_buffer[0] == (cmd_head[1] ^ 0xFF))){
                NGLOG_VERBOSE("ACK (INS ^ FF) received");
                continue;
            }else {
                NGLOG_VERBOSE("SW1 detected");
                break;
            }
        }
        if (i >= 10) {
            NGLOG_ERROR("smart card read data response sw1 timeout, recv_buffer[0]=0x%x",
                (unsigned int)(recv_buffer[0]));
            ret = AUI_RTN_FAIL;
            goto send_thread_exit;
        }
        NGLOG_DUMP("SC send head ACK: ", recv_buffer, actlen);

        if (((recv_buffer[0] >> 4) == 0x06) ||  ((recv_buffer[0] >> 4) == 0x09)){
                // if the response is 0x6X or 0x9X(!= 0x60), it means SW1, now read SW2
             ret = aui_smc_raw_read(m_va_sc_slot[slot_num].handle, recv_buffer+1, 1, &actlen);
             if (AUI_RTN_SUCCESS != ret){
                 NGLOG_ERROR("aui_smc_raw_read SW2 error, ret = %d", (int)ret);
                 goto send_thread_exit;
             }
             NGLOG_DUMP("SC SW1, SW2: ", recv_buffer, 2);
             m_va_sc_slot[slot_num].card_status = CARD_STATUS_SENT_CMD;
             va_ret = VA_SC_CommandDone(slot_num, 2, recv_buffer);
             goto send_thread_exit;
         }
    }

    if (!read_cmd){ // send data

        //step3: send command data
        if (cmd_data_size)
        {
            ret = aui_smc_raw_write(m_va_sc_slot[slot_num].handle, cmd_data, cmd_data_size, &actlen);
            if (AUI_RTN_SUCCESS != ret){
                NGLOG_ERROR("aui_smc_raw_read error, ret = %d", (int)ret);
                goto send_thread_exit;
            }
        }

        //step4.1:  get command response(SW1)
        cmd_response = recv_buffer;
        cmd_response_size = 2;
        for (i = 0; i < 10; i++){
            ret = aui_smc_raw_read(m_va_sc_slot[slot_num].handle, cmd_response, 1, &actlen);
            if (AUI_RTN_SUCCESS != ret){
                NGLOG_ERROR("aui_smc_raw_read SW1 error, ret = %d", (int)ret);
                goto send_thread_exit;
            }
            NGLOG_DUMP("SC send data response SW1: ", cmd_response, actlen);
            if (recv_buffer[0] != 0x60)
                break;
        }
        if (i >= 10) {
            NGLOG_ERROR("smart card read data response sw1 timeout, recv_buffer[0]=0x%x",(unsigned int)(recv_buffer[0]));
            ret = AUI_RTN_FAIL;
            goto send_thread_exit;
        }

        //step4.2:  get command response(SW2)        
        ret = aui_smc_raw_read(m_va_sc_slot[slot_num].handle, cmd_response+1, 1, &actlen);
        if (AUI_RTN_SUCCESS != ret){
            NGLOG_ERROR("aui_smc_raw_read SW2 error, ret = %d", (int)ret);
            goto send_thread_exit;
        }
        NGLOG_DUMP("SC send data response SW2: ", cmd_response+1, actlen);

        m_va_sc_slot[slot_num].card_status = CARD_STATUS_SENT_CMD;
        va_ret = VA_SC_CommandDone(slot_num, cmd_response_size, cmd_response);
        if (va_ret){
            NGLOG_ERROR("VA_SC_CommandDone error, ret = %d", (int)va_ret);
            goto send_thread_exit;
        }
    }else{// read data
        //step3: read data
        short read_length = m_va_sc_slot[slot_num].cmd_buf[4];
        NGLOG_VERBOSE("smart card read length=%d", (int)read_length);
        if (0 == read_length)
            read_length = 256;
        
        ret = aui_smc_raw_read(m_va_sc_slot[slot_num].handle, recv_buffer, read_length, &actlen);
        if (AUI_RTN_SUCCESS != ret){
            NGLOG_ERROR("aui_smc_raw_read error, ret = %d", (int)ret);
            goto send_thread_exit;
        }
        NGLOG_DUMP("SC read data: ", recv_buffer, actlen);

        //step4.1:  get command response(SW1)
        int i;
        cmd_response = recv_buffer+read_length;
        cmd_response_size = read_length+2;
        for (i = 0; i < 10; i++){
            ret = aui_smc_raw_read(m_va_sc_slot[slot_num].handle, cmd_response, 1, &actlen);
            if (AUI_RTN_SUCCESS != ret){
                NGLOG_ERROR("aui_smc_raw_read SW1 error, ret = %d", (int)ret);
                goto send_thread_exit;
            }
            NGLOG_DUMP("SC send data response SW1: ", cmd_response, actlen);
            if (recv_buffer[0] != 0x60)
                break;
        }
        if (i >= 10) {
            NGLOG_ERROR("smart card read data response sw1 timeout, recv_buffer[0]=0x%x",
                (unsigned int)(recv_buffer[0]));
            ret = AUI_RTN_FAIL;
            goto send_thread_exit;
        }

        //step4.2:  get command response(SW2)        
        ret = aui_smc_raw_read(m_va_sc_slot[slot_num].handle, cmd_response+1, 1, &actlen);
        if (AUI_RTN_SUCCESS != ret){
            NGLOG_ERROR("aui_smc_raw_read SW2 error, ret = %d", (int)ret);
            goto send_thread_exit;
        }
        NGLOG_DUMP("SC send data response SW2: ", cmd_response+1, actlen);

        NGLOG_DUMP("SC send reding data: ", recv_buffer, cmd_response_size);
        m_va_sc_slot[slot_num].card_status = CARD_STATUS_SENT_CMD;
        va_ret = VA_SC_CommandDone(slot_num, cmd_response_size, recv_buffer);
        if (va_ret){
            NGLOG_ERROR("VA_SC_CommandDone error, ret = %d\n", (int)va_ret);
            goto send_thread_exit;
        }
        
    }
    
send_thread_exit:

    //m_va_sc_slot[slot_num].card_status = CARD_STATUS_SENT_CMD;

    if (ret)
        VA_SC_CommandFailed(slot_num);
    
    NGLOG_VERBOSE("send command Done!");
    pthread_exit(NULL);
}


INT VA_SC_Reset(DWORD dwScSlot, UINT8 uiPPSNegotiationValue)
{
    int ret_val = kVA_OK;

    aui_hdl hdl = NULL;

    pthread_t va_reset_t;
    
    NGLOG_DEBUG("parameter: dwScSlot=%d, uiPPSNegotiationValue=0x%x!",(int)dwScSlot, (unsigned int)uiPPSNegotiationValue);
    
    if (dwScSlot > kVA_SETUP_NBMAX_SC){
        NGLOG_ERROR("dwScSlot is error: %d", (int)dwScSlot);
        return kVA_INVALID_PARAMETER;
    }
    if (m_va_sc_slot[dwScSlot].card_status & CARD_STATUS_RESETING){
        NGLOG_ERROR("Smart card in reset processing");
        return kVA_RESOURCE_BUSY;
    }

    va_sc_wait_send_cmd(dwScSlot);

    m_va_sc_slot[dwScSlot].card_status = CARD_STATUS_RESETING;

    if(!(hdl = va_sc_open(dwScSlot))){
        NGLOG_ERROR("va_sc_open error!");
        m_va_sc_slot[dwScSlot].card_status = CARD_STATUS_IDLE;
        return kVA_ERROR;
    }
    if (!m_va_sc_slot[dwScSlot].card_exist){
        NGLOG_ERROR("Smart card is not exist");
        m_va_sc_slot[dwScSlot].card_status = CARD_STATUS_IDLE;
        return kVA_INSERT_CARD_FIRST;
    }
    m_va_sc_slot[dwScSlot].handle = hdl;
    m_va_sc_slot[dwScSlot].pps1_value = uiPPSNegotiationValue;
    m_va_sc_slot[dwScSlot].smc_id = dwScSlot;

    if (pthread_create(&va_reset_t, NULL, va_sc_reset_thread, (void*)(&m_va_sc_slot[dwScSlot].smc_id)) != 0){
	NGLOG_ERROR("create thread failure!");
	return kVA_ERROR;
    }
    return ret_val;
}

INT VA_SC_GetState(DWORD dwScSlot, tVA_SC_State *pScState)
{
    
    NGLOG_DEBUG("parameter: dwScSlot=%d, pScState=0x%x!",     (int)dwScSlot, (unsigned int)pScState);

    if ((dwScSlot > kVA_SETUP_NBMAX_SC) || (NULL == pScState)){
        NGLOG_ERROR("dwScSlot is error: %d", (int)dwScSlot);
        return kVA_INVALID_PARAMETER;
    }

    if (va_sc_open(dwScSlot)){
        if (m_va_sc_slot[dwScSlot].card_exist){
            if (m_va_sc_slot[dwScSlot].reseted)
                *pScState = eCARD_READY;
            else
                *pScState = eCARD_INSERTED;
        }else
            *pScState = eCARD_EXTRACTED;
    }
    NGLOG_DEBUG("pScState = %d", *pScState);
    return kVA_OK;
}

INT VA_SC_SendCommand(DWORD dwScSlot, UINT32 uiCommandSize, BYTE *pCommand)
{
    int ret_val = kVA_OK;
    pthread_t va_send_cmd_t;
    aui_hdl hdl = NULL;
    int ret;

    NGLOG_VERBOSE("parameter: dwScSlot=%d, uiCommandSize=%d!",(int)dwScSlot, (int)uiCommandSize);

    if (dwScSlot > kVA_SETUP_NBMAX_SC){
        NGLOG_ERROR("dwScSlot is error: %d", (int)dwScSlot);
        return kVA_INVALID_PARAMETER;
    }
    if (!pCommand){
        NGLOG_ERROR("pCommand is NULL");
        return kVA_INVALID_PARAMETER;
    }
    if (uiCommandSize == 0){
        NGLOG_ERROR("command size is 0");
        return kVA_INVALID_PARAMETER;
    }
    
    NGLOG_DUMP("CMD: ", pCommand, uiCommandSize);
    
    if (m_va_sc_slot[dwScSlot].card_status & CARD_STATUS_SEND_CMDING){
        NGLOG_ERROR("Smart card in send command processing");
        return kVA_RESOURCE_BUSY;
    }
    m_va_sc_slot[dwScSlot].card_status = CARD_STATUS_SEND_CMDING;
    hdl = va_sc_open(dwScSlot);
    if (!hdl){
        NGLOG_ERROR("va_sc_open error!");
        ret_val = kVA_ERROR;
        goto send_exit;
    }
    if (!m_va_sc_slot[dwScSlot].card_exist){
        NGLOG_ERROR("Smart card is not exist");
        ret_val = kVA_INSERT_CARD_FIRST;
        goto send_exit;
    }
    if (!m_va_sc_slot[dwScSlot].reseted){
        NGLOG_ERROR("Rest card first!");
        ret_val = kVA_RESET_CARD_FIRST;
        goto send_exit;
    }

    memset(&(m_va_sc_slot[dwScSlot].cmd_buf), 0, CMD_MAX_LEN);
    m_va_sc_slot[dwScSlot].cmd_size = uiCommandSize;
    memcpy(&(m_va_sc_slot[dwScSlot].cmd_buf), pCommand, uiCommandSize);

    ret = pthread_create(&va_send_cmd_t, NULL, va_sc_send_cmd_thread, (void*)(&m_va_sc_slot[dwScSlot].smc_id));
    if (ret){
	NGLOG_ERROR("create thread failure!, ret = %d!", ret);
	ret_val =  kVA_ERROR;
        goto send_exit;
    }
    
send_exit:    
    if (kVA_OK != ret_val)
        m_va_sc_slot[dwScSlot].card_status = CARD_STATUS_IDLE;    
    
    return ret_val;
}

INT VA_SC_Activate(DWORD dwScSlot)
{
    aui_hdl hdl = NULL;
    AUI_RTN_CODE ret = AUI_RTN_FAIL;
    
    NGLOG_DEBUG("parameter: dwScSlot=%d!", (int)dwScSlot);

    if (dwScSlot > kVA_SETUP_NBMAX_SC){
        NGLOG_ERROR("dwScSlot is error: %d", (int)dwScSlot);
        return kVA_INVALID_PARAMETER;
    }

    va_sc_wait_send_cmd(dwScSlot);
    
    if (!m_va_sc_slot[dwScSlot].card_exist){
        NGLOG_ERROR("Smart card is not exist");
        return kVA_INSERT_CARD_FIRST;
    }

    if(!(hdl = va_sc_open(dwScSlot))){
        NGLOG_ERROR("va_sc_open error!");
        return kVA_ERROR;
    }

    if((ret = aui_smc_active(hdl))!=AUI_RTN_SUCCESS){
        NGLOG_ERROR("aui_smc_active error, ret = %d", (int)ret);
        return kVA_NOT_IMPLEMENTED;
    }

    return kVA_OK;
}

INT VA_SC_Deactivate(DWORD dwScSlot)
{
    aui_hdl hdl = NULL;
    AUI_RTN_CODE ret = AUI_RTN_FAIL;
    
    NGLOG_DEBUG("parameter: dwScSlot=%d!", (int)dwScSlot);

    if (dwScSlot > kVA_SETUP_NBMAX_SC){
        NGLOG_ERROR("dwScSlot is error: %d", (int)dwScSlot);
        return kVA_INVALID_PARAMETER;
    }

    va_sc_wait_send_cmd(dwScSlot);
    
    if (!m_va_sc_slot[dwScSlot].card_exist){
        NGLOG_ERROR("Smart card is not exist");
        return kVA_INSERT_CARD_FIRST;
    }

    hdl = va_sc_open(dwScSlot);
    if (!hdl){
        NGLOG_ERROR("va_sc_open error!");
        return kVA_ERROR;
    }

    ret = aui_smc_deactive(hdl);
    if (AUI_RTN_SUCCESS != ret){
        NGLOG_ERROR("aui_smc_deactive error, ret = %d", (int)ret);
        return kVA_NOT_IMPLEMENTED;
    }
    return kVA_OK;
}

INT VA_SC_Lock(DWORD dwScSlot)
{
    NGLOG_DEBUG("parameter: dwScSlot=%d!", (int)dwScSlot);
    
    if (dwScSlot > kVA_SETUP_NBMAX_SC){
        NGLOG_ERROR("dwScSlot is error: %d", (int)dwScSlot);
        return kVA_INVALID_PARAMETER;
    }
    if (!m_va_sc_slot[dwScSlot].card_exist){
        NGLOG_ERROR("Smart card is not exist");
        return kVA_INSERT_CARD_FIRST;
    }

    if (1 == m_va_sc_slot[dwScSlot].lock){
        NGLOG_ERROR("Smart card %d already lock!", (int)dwScSlot);
        return kVA_RESOURCE_ALREADY_LOCKED;
    }
    m_va_sc_slot[dwScSlot].lock = 1;
	return kVA_OK;
}

INT VA_SC_Unlock(DWORD dwScSlot)
{
    NGLOG_DEBUG("parameter: dwScSlot=%d!", (int)dwScSlot);

    if (dwScSlot > kVA_SETUP_NBMAX_SC){
        NGLOG_ERROR("dwScSlot is error: %d", (int)dwScSlot);
        return kVA_INVALID_PARAMETER;
    }
    if (!m_va_sc_slot[dwScSlot].card_exist){
        NGLOG_ERROR("Smart card is not exist");
        return kVA_INSERT_CARD_FIRST;
    }

    if (0 == m_va_sc_slot[dwScSlot].lock){
        NGLOG_ERROR("Smart card %d already unlock!", (int)dwScSlot);
        return kVA_RESOURCE_NOT_LOCKED;
    }
    m_va_sc_slot[dwScSlot].lock = 0;
	pthread_mutex_lock(&m_sc_mutex);
	pthread_cond_signal(&m_sc_cond); 
	pthread_mutex_unlock(&m_sc_mutex);
        
    return kVA_OK;
}

