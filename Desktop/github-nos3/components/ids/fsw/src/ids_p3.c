#include "ids_p3.h"
//TODO - see if still useful (needed if each task self-reports the alert directly)
#include "ids_app.h"


struct p3_rules p3_struct;

static FILE *log_spin;

static pthread_t thread_spin = 0;
static unsigned char is_t_spin_in_use = 0;
static pthread_t thread_cam = 0;
static unsigned char is_t_cam_in_use = 0;

#if defined P3_PROFILING_ENABLED
struct msg_couple msg_fm[2] = { { .MsgId=0x0808, .FC=0 }, { .MsgId=0x088a, .FC=0 } };
struct msg_couple msg_ds[3] = { { .MsgId=0x0808, .FC=0 }, { .MsgId=0x08b8, .FC=0 }, { .MsgId=0x08ba, .FC=0 } };
struct msg_couple msg_lc[2] = { { .MsgId=0x0808, .FC=0 }, { .MsgId=0x08a7, .FC=0 } };
struct msg_couple msg_sc[5] = { { .MsgId=0x0808, .FC=0 }, { .MsgId=0x18a9, .FC=4 }, { .MsgId=0x08aa, .FC=0 }, { .MsgId=0x18bb, .FC=2 }, { .MsgId=0x18a9, .FC=16 } };
struct msg_couple msg_cam[2] = { { .MsgId=0x0808, .FC=0 }, { .MsgId=0x08c8, .FC=0 } };
struct msg_couple msg_css[3] = { { .MsgId=0x0808, .FC=0 }, { .MsgId=0x0910, .FC=0 }, { .MsgId=0x0911, .FC=0 } };
struct msg_couple msg_adcs[9] = { { .MsgId=0x0808, .FC=0 }, { .MsgId=0x0940, .FC=0 }, { .MsgId=0x0941, .FC=0 }, { .MsgId=0x0942, .FC=0 }, { .MsgId=0x0943, .FC=0 }, { .MsgId=0x0944, .FC=0 }, { .MsgId=0x0945, .FC=0 }, { .MsgId=0x1992, .FC=3 }, { .MsgId=0x193a, .FC=5 } };
struct msg_couple msg_mag[3] = { { .MsgId=0x0808, .FC=0 }, { .MsgId=0x092a, .FC=0 }, { .MsgId=0x092b, .FC=0 } };
struct msg_couple msg_fss[3] = { { .MsgId=0x0808, .FC=0 }, { .MsgId=0x0920, .FC=0 }, { .MsgId=0x0921, .FC=0 } };
struct msg_couple msg_st[3] = { { .MsgId=0x0808, .FC=0 }, { .MsgId=0x0935, .FC=0 }, { .MsgId=0x0936, .FC=0 } };
struct msg_couple msg_torquer[2] = { { .MsgId=0x0808, .FC=0 }, { .MsgId=0x093a, .FC=0 } };
struct msg_couple msg_nav[3] = { { .MsgId=0x0808, .FC=0 }, { .MsgId=0x0870, .FC=0 }, { .MsgId=0x0871, .FC=0 } };
struct msg_couple msg_ids[3] = { { .MsgId=0x0808, .FC=0 }, { .MsgId=0x09fa, .FC=0 }, { .MsgId=0x09fb, .FC=0 } };
struct msg_couple msg_isl[2] = { { .MsgId=0x0808, .FC=0 }, { .MsgId=0x0872, .FC=0 } };
struct msg_couple msg_ci[3] = { { .MsgId=0x0885, .FC=0 }, { .MsgId=0x0884, .FC=0 }, { .MsgId=0x0808, .FC=0 } };
struct msg_couple msg_to[3] = { { .MsgId=0x0881, .FC=0 }, { .MsgId=0x0880, .FC=0 }, { .MsgId=0x0808, .FC=0 } };
struct msg_couple msg_eps[2] = { { .MsgId=0x0808, .FC=0 }, { .MsgId=0x091a, .FC=0 } };
struct msg_couple msg_imu[3] = { { .MsgId=0x0808, .FC=0 }, { .MsgId=0x0925, .FC=0 }, { .MsgId=0x0926, .FC=0 } };
struct msg_couple msg_isl_isl = { .MsgId=0x0808, .FC=0 };
struct msg_couple msg_nav_child[2] = { { .MsgId=0x0808, .FC=0 }, { .MsgId=0x0871, .FC=0 } };
struct msg_couple msg_fm_child = { .MsgId=0x0808, .FC=0 };
struct msg_couple msg_cam_child[2] = { { .MsgId=0x0808, .FC=0 }, { .MsgId=0x08c9, .FC=0 } };
struct msg_couple msg_rw[2] = { { .MsgId=0x0808, .FC=0 }, { .MsgId=0x0993, .FC=0 } };
struct msg_couple msg_time_1hz = { .MsgId=0x1811, .FC=0 };
struct msg_couple msg_time_tone[2] = { { .MsgId=0x1810, .FC=0 }, { .MsgId=0x1862, .FC=0 } };
struct msg_couple msg_cfe_time[3] = { { .MsgId=0x1860, .FC=0 }, { .MsgId=0x0808, .FC=0 }, { .MsgId=0x0805, .FC=0 } };
struct msg_couple msg_sch[41] = { { .MsgId=0x1873, .FC=0 }, { .MsgId=0x1874, .FC=0 }, { .MsgId=0x18b5, .FC=0 }, { .MsgId=0x18b4, .FC=0 }, { .MsgId=0x1871, .FC=0 }, { .MsgId=0x1871, .FC=1 }, { .MsgId=0x1921, .FC=1 }, { .MsgId=0x1911, .FC=1 }, { .MsgId=0x1926, .FC=1 }, { .MsgId=0x192b, .FC=1 }, { .MsgId=0x1936, .FC=1 }, { .MsgId=0x18c9, .FC=0 }, { .MsgId=0x1941, .FC=0 }, { .MsgId=0x1942, .FC=0 }, { .MsgId=0x1992, .FC=2 }, { .MsgId=0x1936, .FC=0 }, { .MsgId=0x18ab, .FC=0 }, { .MsgId=0x191b, .FC=0 }, { .MsgId=0x193b, .FC=0 }, { .MsgId=0x1940, .FC=3 }, { .MsgId=0x1940, .FC=4 }, { .MsgId=0x1940, .FC=5 }, { .MsgId=0x1940, .FC=6 }, { .MsgId=0x1940, .FC=7 }, { .MsgId=0x188d, .FC=0 }, { .MsgId=0x18aa, .FC=0 }, { .MsgId=0x1921, .FC=0 }, { .MsgId=0x1885, .FC=0 }, { .MsgId=0x1911, .FC=0 }, { .MsgId=0x1926, .FC=0 }, { .MsgId=0x18bc, .FC=0 }, { .MsgId=0x18a5, .FC=0 }, { .MsgId=0x1881, .FC=0 }, { .MsgId=0x192b, .FC=0 }, { .MsgId=0x0808, .FC=0 }, { .MsgId=0x1809, .FC=0 }, { .MsgId=0x180b, .FC=0 }, { .MsgId=0x180d, .FC=0 }, { .MsgId=0x180c, .FC=0 }, { .MsgId=0x1808, .FC=0 }, { .MsgId=0x199b, .FC=0 } };
struct msg_couple msg_cf[8] = { { .MsgId=0x08b0, .FC=0 }, { .MsgId=0x0808, .FC=0 } , { .MsgId=0x18b3, .FC=30 }, { .MsgId=0x18b3, .FC=24 }, { .MsgId=0x18b3, .FC=34 }, { .MsgId=0x18b6, .FC=31 }, { .MsgId=0x18b6, .FC=24 },{ .MsgId=0x18b6, .FC=34 } };
struct msg_couple msg_cfe_evs = { .MsgId=0x0801, .FC=0 };
struct msg_couple msg_cfe_sb = { .MsgId=0x0803, .FC=0 };
struct msg_couple msg_cfe_tbl = { .MsgId=0x0804, .FC=0 };
struct msg_couple msg_cfe_es[2] = { { .MsgId=0x0800, .FC=0 }, { .MsgId=0x0808, .FC=0 } };
struct msg_couple msg_ci_custom[23] = { { .MsgId=0x1880, .FC=2 }, { .MsgId=0x1870, .FC=2 }, { .MsgId=0x193a, .FC=2 }, { .MsgId=0x192a, .FC=2 }, { .MsgId=0x1925, .FC=2 }, { .MsgId=0x1910, .FC=2 }, { .MsgId=0x1920, .FC=2 }, { .MsgId=0x1935, .FC=2 }, { .MsgId=0x18c8, .FC=13 }, { .MsgId=0x1940, .FC=8 }, { .MsgId=0x1940, .FC=2 }, { .MsgId=0x1940, .FC=9 }, { .MsgId=0x1803, .FC=1 }, { .MsgId=0x18c8, .FC=12 }, { .MsgId=0x1806, .FC=12 }, { .MsgId=0x1806, .FC=10 }, { .MsgId=0x1920, .FC=0 }, { .MsgId=0x1910, .FC=0 }, { .MsgId=0x191b, .FC=0 }, { .MsgId=0x18b3, .FC=52 }, { .MsgId=0x18b3, .FC=30 }, { .MsgId=0x18b3, .FC=36 }, { .MsgId=0x199a, .FC=2 } };

static struct task_profile profile_tab[33/*Total task number*/] = {

	{ "FM", msg_fm, 2 },
	{ "DS", msg_ds, 3 },
	{ "LC", msg_lc, 2 },
	{ "SC", msg_sc, 5 },
	{ "CAM", msg_cam, 2 },
	{ "CSS", msg_css, 3 },
	{ "ADCS", msg_adcs, 9 },
	{ "MAG", msg_mag, 3 },
	{ "FSS", msg_fss, 3 },
	{ "ST", msg_st, 3 },
	{ "TORQUER", msg_torquer, 2 },
	{ "NAV", msg_nav, 3 },
	{ "IDS", msg_ids, 3 },
	{ "ISL", msg_isl, 2 },
	{ "CI", msg_ci, 3 },
	{ "TO", msg_to, 3 },
	{ "EPS", msg_eps, 2 },
	{ "IMU", msg_imu, 3 },
	{ "ISL_ISL", &msg_isl_isl, 1 },
	{ "NAV_CHILD_TASK", msg_nav_child, 2 },
	{ "FM_CHILD_TASK", &msg_fm_child, 1 },
	{ "CAM_CHILD_TASK", msg_cam_child, 2 },
	{ "RW", msg_rw, 2 },
	{ "TIME_1HZ_TASK", &msg_time_1hz, 1 },
	{ "TIME_TONE_TASK", msg_time_tone, 2 },
	{ "CFE_TIME", msg_cfe_time, 3 },
	{ "SCH", msg_sch, 41 },
	{ "CF", msg_cf, 2 },
	{ "CFE_EVS", &msg_cfe_evs, 1 },
	{ "CFE_SB", &msg_cfe_sb, 1 },
	{ "CFE_TBL", &msg_cfe_tbl, 1 },
	{ "CFE_ES", msg_cfe_es, 2 },
	{ "CI Custom Main Task", msg_ci_custom, 23 }
};
#endif

void init_p3()
{
	probe3_init();
	p3_struct.rules =(rule_t *) &probe3_rules;
}

void update_Log_P3(FILE * log_file, OS_time_t *simtime, unsigned int seconds, struct timespec *realtime, uint16_t msgId, int16_t fc, uint8_t is_tc, CFE_ES_AppId_t AppId, char *TaskName, CFE_SB_Buffer_t *Content, size_t ContentSize, uint8_t attack_tag, int8_t is_allowed)
{
	//fprintf(log_file,"PSP ticks : %lld, MsgId : %04x, FC : %d, AppId : %d, TaskName : %s, Message : ", simtime->ticks/*/OS_TIME_TICKS_PER_USEC*/, msgId, fc, AppId, TaskName);
	//														time in ticks 							   real time in µsec												Tag - anomaly or not (if yes 2 - or 3 ??? so & with 2 (0b10)) IDS verdict (is_allowed)
	fprintf(log_file,"%lld,%d.%06ld,%04x,%d,%d,%d,%s,%d,%d,", simtime->ticks/*/OS_TIME_TICKS_PER_USEC*/, seconds, realtime->tv_nsec/1000, msgId, fc, is_tc, AppId, TaskName, attack_tag,  is_allowed);
	for(unsigned int i = 0; i<ContentSize; i++){
		//fprintf(log_fd,"%02x", MsgBuf[i]);
		fprintf(log_file,"%02x", Content->Msg.Byte[i]);
	}
	fprintf(log_file,"\n");
	fflush(log_file);
}


void* time_thread_fct_spin(void *arg)
{
	#ifdef P3_SATSPIN_DEBUG
	printf("spin thread startup\n");
	#endif
	do
	{
		sleep(8);
		printf("[P3 - SPIN] ALERT, %s\n",(const char *)arg);
		log_spin= fopen("/tmp/logattack_spin.txt", "a");
		struct timespec realtime;
		time_t timestamp = time(NULL);
		struct tm* timeInfos = localtime(&timestamp);
		timespec_get(&realtime, TIME_UTC);
		fprintf(log_spin, "[P3] SPIN ALERT at : %ld.%09ld / %02d:%02d:%02d\n", realtime.tv_sec, realtime.tv_nsec, timeInfos->tm_hour, timeInfos->tm_min, timeInfos->tm_sec);
		fclose(log_spin);
	} while (p3_struct.rules[5].holder==2);

	//bus_rules.state_table[2] = 0; //TRICK TO RESET STATE MACHINE AFTER AN ALERT IS RAISED !! TODO - CHANGE IT SO THAT DURING ATTACKS WE JUST SEND ALERTS EVERY 10 SEC
	
	return NULL;
}

void* time_thread_fct_cam(void *arg)
{
	#ifdef P3_CAMRESULTS_WORKAROUND_DEBUG
	printf("démarrage thread cam\n");
	#endif
	OS_time_t starttime;	
	OS_time_t currtime;	
	CFE_PSP_GetTime(&starttime);
	int i = 0;
	do
	{
		sleep(1);
		CFE_PSP_GetTime(&currtime);
		i++;
	} while (currtime.ticks - starttime.ticks < CAM_RESULT_TIMEOUT);
	
	((struct p3_rules *)arg)->flow_count[0] = 0;
	((struct p3_rules *)arg)->rules[4].holder = 0;
	*(&is_t_cam_in_use)=0;
	printf("[P3 - TC Analyzer] cam result timeout, number of sleeps : %d\n", i);
	printf("[P3 - TC Analyzer] changing state[1] from %d to %d\n", 1, 0);

	return NULL;
}

void p3_cond_action_processing(subrule_t* subrule, rule_t *rule, int i/*rule nb*/, uint16_t msgId, uint16_t fc, uint8_t *param_ptr, char * is_accepted, char * raise_alert, char * rule_matched)
{
	uint8_t state_val_holder;
	uint8_t cond_val_holder;

	if ((msgId == 0x18C8)&&(fc == 12))
	{
		p3_struct.flow_count[0]++; //increment the number of photos to wait for
		#ifdef P3_CAMRESULTS_WORKAROUND_DEBUG
		printf("\033[1mflow_count++ : %d\033[0m\n", p3_struct.flow_count[0]);
		#endif
		if (is_t_cam_in_use==0)
		{
			pthread_create(&thread_cam, NULL, time_thread_fct_cam,(void*) &p3_struct);
			is_t_cam_in_use = 1;
		}
		#ifdef P3_CAMRESULTS_WORKAROUND_DEBUG
		else
		{
			printf("[P3] Warning : thread_cam already monitoring time !\n");
		}
		#endif
	}

	if ((msgId == 0x0808) && (subrule->rule_body.rule_parameters.val_type == STRING) && (strncmp("CAM EXP3 Complete", (char *)param_ptr, subrule->rule_body.rule_parameters.length/8)==0))
	{
		if (p3_struct.flow_count[0]>0)
		{
			p3_struct.flow_count[0]--; //decrement
			#ifdef P3_CAMRESULTS_WORKAROUND_DEBUG
			printf("\033[1mflow_count-- : %d\033[0m\n", p3_struct.flow_count[0]);
			#endif
			if (is_t_cam_in_use != 0) //is already in use
			{
				// we restart the timeout counter
				pthread_cancel(thread_cam);
				is_t_cam_in_use = 0;
				if (p3_struct.flow_count[0]>0)
				{
					pthread_create(&thread_cam, NULL, time_thread_fct_cam,(void*) &p3_struct);
					is_t_cam_in_use = 1;
				}
			}
			else
			{
				printf("[P3] Warning : trying to stop thread_cam when it is not in use !\n");
			}
		}
	}

	cond_val_holder = subrule->header.start_state; // WILL NOT WORK IF MULTIPLE COND

	//printf("[i=%d]rule cond : %s, strncmp cond " " : %d, state_table[state_id] = %d, cond_val_holder : %d\n", i, bus_rules.cond_tab[i], strncmp(bus_rules.cond_tab[i], " ", 2/*TEST*/), bus_rules.state_table[bus_rules.state_id[i]], cond_val_holder);
	if (rule->holder == cond_val_holder)
	{
		//cond is valid
		printf("[P3 - TC Analyzer] Match for rule %d\n", i);
		printf("[P3 - TC Analyzer] Action : ");

		//NOT ROBUST TO |E1,D ! only D or E1 atm !
		if (subrule->header.action.measure == DROP)
		{
			printf("DROP\n");
			*is_accepted = false;
			*raise_alert = true;
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
			
			//printf("msgId = %02x, fc = %d, cur_state = %d, next_state = %d\n",msgId,fc,bus_rules.state_table[bus_rules.state_id[i]],state_val_holder);	
			if ((msgId == 0x1992)&&(fc == 3))
			{
				if ((rule->holder==1)&&(state_val_holder==2))
				{
					#ifdef P3_SATSPIN_DEBUG
					printf("on arme la bombe\n");
					#endif
					if (!is_t_spin_in_use)
					{
						pthread_create(&thread_spin, NULL, time_thread_fct_spin,(void*) "[POSSIBLE TUMBLING]");
						is_t_spin_in_use = 1;
					}
					else
					{
						printf("[P3] Warning : trying to launch multiple instances of thread_spin !\n");
					}
				}
			}
			if ((msgId == 0x1940)&&(fc == 2))
			{
				if ((rule->holder==2)&&(state_val_holder==0))
				{
					#ifdef P3_SATSPIN_DEBUG
					printf("on defuse la bombe\n");
					#endif
					if (is_t_spin_in_use)
					{
						pthread_cancel(thread_spin);
						is_t_spin_in_use = 0;
					}
					else
					{
						printf("[P3] Warning : trying to stop thread_spin when it is not in use !\n");
					}
				}
			}
			
			if ((msgId == 0x0808) && (subrule->rule_body.rule_parameters.val_type == STRING) && 
			(strncmp("CAM EXP3 Complete", (char *)param_ptr, subrule->rule_body.rule_parameters.length/8)==0) && (p3_struct.flow_count[0]!=0))
			{
				#ifdef P3_CAMRESULTS_WORKAROUND_DEBUG
				printf("\033[1mno state change, waiting for other pictures (%d)\033[0m\n", bus_rules.flow_count[0]);
				#endif
				;//Do nothing as we are still waiting for other pictures to go by before transitioning to the state where restart is allowed
			}
			else
			{
				printf("[P3 - TC Analyzer] changing state of rule %d from %d to %d\n", i, rule->holder, state_val_holder);
				rule->holder = state_val_holder;
			}
		}
		*rule_matched = true;
	}
	else
	{
		//printf("cond is not valid\n");
	}
}



char is_command_allowed_bus(CFE_SB_Buffer_t *Content, size_t ContentSize, uint16_t msgId, uint16_t fc, char *Taskname, int8_t *alert_rule_nb)
{	
	char is_accepted = true; //by default the commands are accepted
	*alert_rule_nb = -1; //by default no rule is blocking the packet

	#ifdef P3_PROFILING_ENABLED
	char is_present = false;
	/*
	 * ADD PROFILE VERIFICATION HERE
	 * DO NOT BLOCK FROM CI_CUSTOM, ONLY RAISE ALERTS -> keep commands allowed !
	*/
	for(int i = 0; i<33/*Total task number*/; i++)
	{
		//check if the Taskname is equal to a taskname in the task profiles
		if (strncmp(profile_tab[i].taskname, Taskname, strlen(profile_tab[i].taskname))==0)
		{
			if (profile_tab[i].couples_nb == 1)
			{
				//no need to iterate, check MsgId and FC
				if ((profile_tab[i].couples_tab->MsgId == msgId)&&(profile_tab[i].couples_tab->FC == fc))
				{
					is_present = true;
					//keep the packet allowed
				}
				break;
			}

			for(int j = 0; j<profile_tab[i].couples_nb; j++)
			{
				if ((profile_tab[i].couples_tab[j].MsgId == msgId)&&(profile_tab[i].couples_tab[j].FC == fc))
				{
					is_present = true;
					break;
				}
			}

			if(is_present)
			{
				break;
			}
		}
	}

	// if there is no couple matching any profile
	if (!is_present)
	{
		printf("Taskname : %s\n",Taskname);

		//if the taskname is "CI Custom Main Task"
		if (strncmp(Taskname, "CI Custom Main Task", strlen("CI Custom Main Task"))==0)
		{
			//Raise a detection but do not block !
			printf("[P3 - App Profiling] Unusal packet coming from the ground, be careful !\n");
			#ifdef IDS_TELEM_ENABLED
			IDS_RaiseAlert(5, 0, ContentSize, strlen(Taskname), Content->Msg.Byte, Taskname);
			#endif
			//TODO - ADD CODE FOR DETECTION EVENT
		}
		else if (msgId == 0x0808)
		{
			//Raise a detection but do not block ! (Event messages can be legitimate, but rare ones may still need to be notified to the ground, still to discuss)
			printf("[P3 - App Profiling] Unusual event for EVS, task = %s\n", Taskname);
			#ifdef IDS_TELEM_ENABLED
			IDS_RaiseAlert(5, 0, ContentSize, strlen(Taskname), Content->Msg.Byte, Taskname);
			#endif
			//TODO - ADD CODE FOR DETECTION EVENT - BE CAREFUL WITH INTERACTION WITH RULE CAM CRASH
		}
		else
		{
			// block packet
			printf("[P3 - App Profiling] Unauthorized packet blocked\n");
			is_accepted = false;
			#ifdef IDS_TELEM_ENABLED
			IDS_RaiseAlert(5, 0, ContentSize, strlen(Taskname), Content->Msg.Byte, Taskname);
			#endif
		}
	}
	#endif
	

	if(is_accepted)
	{
		cond_processing_fct_ptr cond_fct_ptr = &p3_cond_action_processing;
		is_accepted = analyzer_check_packet(p3_struct.rules, Content->Msg.Byte, ContentSize, msgId, fc, alert_rule_nb, P3_RULES_MODULE_NUMBER, cond_fct_ptr, PROBE3_RULE_NUMBER);
	}

	return is_accepted;
}
