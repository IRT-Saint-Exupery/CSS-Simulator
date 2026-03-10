#include "ids_p2.h"
#include "ids_p1.h"

#if defined IDS_P2_ENABLED
extern struct P2_struct P2_data;
#else
struct P2_struct P2_data; //TODO - verifiy if this does not introduce a bias inmemory evaluation !!
#endif

#ifdef IDS_P1_ENABLED
extern struct P1_struct * P1_struct_ptr;
#endif

void init_P2()
{
	P2_data.buffer_last_time = 0;
	P2_data.last_time_too_quick = 0;
	P2_data.cfdp_is_ongoing = false;
	probe2_init();
	P2_data.rules =(rule_t *) &probe2_rules;
}

void init_P2_buffers()
{
	// TODO - ADD INIT FOR STATE TABLE & include this in init_P2()
	for (int i = 0; i<2/*HARDCODED THRESHOLD, EDIT LATER*/; i++)
	{
		P2_data.last_MsgIds[i]=0;
		P2_data.last_FCs[i]=0;
	}
}

void update_Log_P2(FILE * log_file, OS_time_t simtime, TC_t *tc_frame_struct, /*uint8* buffer,*/ unsigned int seconds, struct timespec *realtime, uint16_t msgId, int16_t fc, uint8_t is_tc, int8_t ids_output, uint8_t attack_tag)
{
	fprintf(log_file, "%lld,%d.%06ld,%04x,%d,%d,%d,%d,", simtime.ticks, seconds, realtime->tv_nsec/1000, msgId, fc, is_tc, attack_tag, ids_output);
	for (uint16 i = 0; i < tc_frame_struct->tc_pdu_len; i++)
	{
		//packet
		fprintf(log_file, "%02x", tc_frame_struct->tc_pdu[i]);
	}
	fprintf(log_file, "\n");
	fflush(log_file);

	//IDS - keep track of last packet's time of arrival, replace by P1 info later
	P2_data.buffer_last_time = simtime.ticks;
	P2_data.last_MsgIds[0] = ((tc_frame_struct->tc_pdu[0]<<8) | tc_frame_struct->tc_pdu[1]);
	P2_data.last_FCs[0] = tc_frame_struct->tc_pdu[6];
}

void p2_cond_action_processing(subrule_t* subrule, rule_t *rule, int i/*rule nb*/, uint16_t msgId, uint16_t fc, uint8_t *param_ptr, char * is_accepted, char * raise_alert, char * rule_matched)
{
	uint8_t state_val_holder;
	uint8_t cond_val_holder;

	cond_val_holder = subrule->header.start_state; // WILL NOT WORK IF MULTIPLE COND

	//printf("[i=%d]rule cond : %s, strncmp cond " " : %d, state_table[state_id] = %d, cond_val_holder : %d\n", i, bus_rules.cond_tab[i], strncmp(bus_rules.cond_tab[i], " ", 2/*TEST*/), bus_rules.state_table[bus_rules.state_id[i]], cond_val_holder);
	if (rule->holder == cond_val_holder)
	{
		//cond is valid
		printf("[P2 - TC Analyzer] Match for rule %d\n", i);
		printf("[P2 - TC Analyzer] Action : ");

		//NOT ROBUST TO |E1,D ! only D or E1 atm !
		if (subrule->header.action.measure == DROP)
		{
			printf("DROP\n");
			*is_accepted = false;
			*raise_alert = true;
            //printf("\033[1mIS_ACCEPTED & RAISE_ALERT OK\033[0m\n");
		}	
		else if (subrule->header.action.measure == ALERT)
		{
			printf("ALERT\n");
			*raise_alert = true;
		}
		else
		{
			printf("Switch to %d\n", subrule->header.action.value);
			
			state_val_holder = subrule->header.action.state;
			
			printf("[P2 - TC Analyzer] changing state of rule %d from %d to %d\n", i, rule->holder, state_val_holder);
			rule->holder = state_val_holder;
		}
		*rule_matched = true;
		//printf("\033[1mRULE_MATCHED OK\033[0m\n");
	}
	else
	{
		//printf("cond is not valid\n");
	}
}


char is_command_allowed(TC_t *tc_frame_struct, OS_time_t simtime, int8_t *alert_rule_nb)
{
	char is_accepted = true; //by default the commands are accepted
	*alert_rule_nb = -1;

	uint16_t msgId;
	uint16_t fc;
	
	//printf("\033[1mPROCESSING PACKET P2\033[0m\n");

	if (tc_frame_struct->tc_pdu_len > 0)
	{
		//TODO - externalize this block when changing code with new TC_Analyzer api
		msgId = ((tc_frame_struct->tc_pdu[0]<<8) | tc_frame_struct->tc_pdu[1]);

		// if it is a TC extract the FC, else set it to 0
		if (msgId & 0x1000)
		{
    		fc = tc_frame_struct->tc_pdu[6];
		}
		else
		{
			fc = 0;
		}
	}
	else
	{
		msgId = 0;
		fc = 0;
	}

	#if defined IDS_P1_ENABLED && defined CFDP_FLOOD_LIFT
	struct timespec realtime;
    timespec_get(&realtime, TIME_UTC);
	
	//If the threshold for the CFDP transaction is reached it is assumed to be over -> timeout & mark it as not ongoing
	if ((P2_data.cfdp_is_ongoing)&&(realtime.tv_sec - P2_data.cfdp_start_sec > CFDP_TIMEOUT_THRESHOLD))
	{
		//printf("TIMEOUT, ANTI-FLOODING BACK UP\n");
		sem_wait(&(P1_struct_ptr->cfdp_sem));
		P1_struct_ptr->cfdp_is_ongoing=false;
		//printf("P1_cfdp_is_ongoing : %d\n", P1_struct_ptr->cfdp_is_ongoing);
		sem_post(&(P1_struct_ptr->cfdp_sem));
		P2_data.cfdp_is_ongoing = false;
	}

	//If a CFDP transaction is starting, inform P1 that the anti-flooding needs to be lifted (for a limited time)
	//TODO - RE-ACTIVATE THE ANTI-FLOODING
	if ((msgId==0x18b3)&&(fc==0x24))
	{
		//printf("CFDP transac packet\n");
		if (!P2_data.cfdp_is_ongoing)
		{
			//printf("CFDP transaction starting\n");
			sem_wait(&(P1_struct_ptr->cfdp_sem));
			//printf("P1_cfdp_is_ongoing : %d\n", P1_struct_ptr->cfdp_is_ongoing);
			if (!P1_struct_ptr->cfdp_is_ongoing)
				P1_struct_ptr->cfdp_is_ongoing=true;
			sem_post(&(P1_struct_ptr->cfdp_sem));
			P2_data.cfdp_is_ongoing = true;
			P2_data.cfdp_start_sec = realtime.tv_sec;
		}
	}
	#endif

	//printf("MsgId : %02x, FC : %d\n",msgId,fc);

	//printf("buffer last time : %lld\n", P2_data.buffer_last_time);

	/* If there was another packet before, and we are not under a CFDP file transfer, begin packet rate checks */ 
	//TODO - maybe just allow packets with the MSGID of the transaction and not every packet
	// but would have to take into account metadata packets with no MsgId ?
	if ((P2_data.buffer_last_time != 0)&&(!P2_data.cfdp_is_ongoing))
	{
		int64 diff_timing = (simtime.ticks - P2_data.buffer_last_time)/OS_TIME_TICKS_PER_MSEC;
		//printf("diff timing P2 : %lld ms\n", diff_timing);
		
		if (diff_timing < P2_DOS_THRESHOLD)
		{
			if (tc_frame_struct->tc_pdu_len != 0)
			{
				//catch what p1 cannot see
				if (/*(P2_data.last_time_too_quick != 0)&&*/(P2_data.last_MsgIds[0]==msgId)&&(P2_data.last_FCs[0]==fc))
				{	
					if (P2_data.last_time_too_quick < P2_DOS_PACKET_THRESHOLD)
					{
						P2_data.last_time_too_quick++;
					}
					else
					{
						/* If last packets were too quick and this one too, and threshold reached */
						printf("[P2] - Slow anti-flooding : TOO FAST + same packet\n");
						//if ( **Same packet verification**
						is_accepted = false;
					}
				}
				else
				{
					P2_data.last_time_too_quick = 0;
					/* This packet came too quickly, watch out when processing next packet */
					//P2_data.last_time_too_quick = 1;
				}
			}
		}
		else
		{
			/* Reset last packet too quick verification */
			//P2_data.last_time_too_quick = 0;
		}
	}

	if(is_accepted)
	{
		cond_processing_fct_ptr cond_fct_ptr = &p2_cond_action_processing;
		is_accepted = analyzer_check_packet(P2_data.rules,
			 								tc_frame_struct->tc_pdu,
											(size_t) tc_frame_struct->tc_pdu_len,
											msgId, 
											fc, 
											alert_rule_nb, 
											P2_RULES_MODULE_NUMBER, 
											cond_fct_ptr, 
											PROBE2_RULE_NUMBER);
	}

	return is_accepted;
}
