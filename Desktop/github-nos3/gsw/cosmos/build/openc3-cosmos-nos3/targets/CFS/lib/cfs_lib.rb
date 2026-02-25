require 'cosmos'
require 'cosmos/script'

#
# Definitions
#
CFS_CMD_SLEEP = 3


#
# Functions
#
def get_sc_hk()
    cmd("CFS_RADIO SC_SEND_HK")
    wait_check_packet("CFS_RADIO", "SC_HKTLM", 1, 3)
end

def get_debug_sc_hk()
    cmd("CFS SC_SEND_HK")
    wait_check_packet("CFS", "SC_HKTLM", 1, 3)
end

def get_cf_hk()
    cmd("CFS_RADIO CF_SEND_HK")
    wait_check_packet("CFS_RADIO", "CF_HKPACKET", 1, 10)
end

def cf_cmd(*command)
    count = tlm("CFS_RADIO CF_HKPACKET CMDCOUNTER") + 1

    if (count == 256)
        count = 0
    end

    cmd *command
    sleep(CFS_CMD_SLEEP)
    get_cf_hk()
    current = tlm("CFS_RADIO CF_HKPACKET CMDCOUNTER")
    if (current != count)
        # Try again
        cmd *command
        sleep(CFS_CMD_SLEEP)
        get_cf_hk()
        current = tlm("CFS_RADIO CF_HKPACKET CMDCOUNTER")
        if (current != count)
            # Third times the charm
            cmd *command
            sleep(CFS_CMD_SLEEP)
            get_cf_hk()
            current = tlm("CFS_RADIO CF_HKPACKET CMDCOUNTER")
            if (current != count)
                # 4
                cmd *command
                sleep(CFS_CMD_SLEEP)
                get_cf_hk()
                current = tlm("CFS_RADIO CF_HKPACKET CMDCOUNTER")
                if (current != count)
                    # 5
                    cmd *command
                    sleep(CFS_CMD_SLEEP)
                    get_cf_hk()
                    current = tlm("CFS_RADIO CF_HKPACKET CMDCOUNTER")
                    if (current != count)
                        # Last check
                        get_cf_hk()
                    end
                end
            end
        end
    end
    check("CFS_RADIO CF_HKPACKET CMDCOUNTER >= #{count}")
end

def get_fm_hk()
    cmd("CFS_RADIO FM_SEND_HK")
    wait_check_packet("CFS_RADIO", "FM_HOUSEKEEPINGPKT", 1, 10)
end

def get_fm_hk_debug()
    cmd("CFS FM_SEND_HK")
    wait_check_packet("CFS", "FM_HOUSEKEEPINGPKT", 1, 10)
end

def fm_cmd(*command)
    count = tlm("CFS_RADIO FM_HOUSEKEEPINGPKT COMMANDCOUNTER") + 1

    if (count == 256)
        count = 0
    end

    cmd *command
    sleep(CFS_CMD_SLEEP)
    get_fm_hk()
    current = tlm("CFS_RADIO FM_HOUSEKEEPINGPKT COMMANDCOUNTER")
    if (current != count)
        # Try again
        cmd *command
        sleep(CFS_CMD_SLEEP)
        get_fm_hk()
        current = tlm("CFS_RADIO FM_HOUSEKEEPINGPKT COMMANDCOUNTER")
        if (current != count)
            # Third times the charm
            cmd *command
            sleep(CFS_CMD_SLEEP)
            get_fm_hk()
            current = tlm("CFS_RADIO FM_HOUSEKEEPINGPKT COMMANDCOUNTER")
            if (current != count)
                # 4
                cmd *command
                sleep(CFS_CMD_SLEEP)
                get_fm_hk()
                current = tlm("CFS_RADIO FM_HOUSEKEEPINGPKT COMMANDCOUNTER")
                if (current != count)
                    # 5
                    cmd *command
                    sleep(CFS_CMD_SLEEP)
                    get_fm_hk()
                    current = tlm("CFS_RADIO FM_HOUSEKEEPINGPKT COMMANDCOUNTER")
                    if (current != count)
                        # Last check
                        get_fm_hk()
                    end
                end
            end
        end
    end
    check("CFS_RADIO FM_HOUSEKEEPINGPKT COMMANDCOUNTER >= #{count}")
end

def fm_cmd_debug(*command)
    count = tlm("CFS FM_HOUSEKEEPINGPKT COMMANDCOUNTER") + 1

    if (count == 256)
        count = 0
    end

    cmd *command
    sleep(CFS_CMD_SLEEP)
    get_fm_hk_debug()
    current = tlm("CFS FM_HOUSEKEEPINGPKT COMMANDCOUNTER")
    if (current != count)
        # Try again
        cmd *command
        sleep(CFS_CMD_SLEEP)
        get_fm_hk_debug()
        current = tlm("CFS FM_HOUSEKEEPINGPKT COMMANDCOUNTER")
        if (current != count)
            # Third times the charm
            cmd *command
            sleep(CFS_CMD_SLEEP)
            get_fm_hk_debug()
            current = tlm("CFS FM_HOUSEKEEPINGPKT COMMANDCOUNTER")
            if (current != count)
                # 4
                cmd *command
                sleep(CFS_CMD_SLEEP)
                get_fm_hk_debug()
                current = tlm("CFS FM_HOUSEKEEPINGPKT COMMANDCOUNTER")
                if (current != count)
                    # 5
                    cmd *command
                    sleep(CFS_CMD_SLEEP)
                    get_fm_hk_debug()
                    current = tlm("CFS FM_HOUSEKEEPINGPKT COMMANDCOUNTER")
                    if (current != count)
                        # Last check
                        get_fm_hk_debug()
                    end
                end
            end
        end
    end
    check("CFS FM_HOUSEKEEPINGPKT COMMANDCOUNTER >= #{count}")
end

def get_fm_files(directory, offset)
    cmd("CFS_RADIO FM_GET_DIR_PKT with DIRECTORY '#{directory}', DIRLISTOFFSET #{offset}, GETSIZETIMEMODE 0xFFFFFFFF")
    wait_check_packet("CFS_RADIO", "FM_DIRLISTPKT", 1, 10)
end

def get_fm_files_debug(directory, offset)
    cmd("CFS FM_GET_DIR_PKT with DIRECTORY '#{directory}', DIRLISTOFFSET #{offset}, GETSIZETIMEMODE 0xFFFFFFFF")
    wait_check_packet("CFS", "FM_DIRLISTPKT", 1, 10)
end

def cfdp_file_req_c1(sc_file_name, gsw_file_name, num_attemps, request_timeout, complete_timeout)
    # Prepare  
    cf_sent = tlm("CFS_RADIO CF_HKPACKET CH0_FILES_SENT")
    cf_cmd_cnt = tlm("CFS_RADIO CF_HKPACKET CMDCOUNTER")
    active = tlm("CFS_RADIO CF_HKPACKET CH0_ACTIVE_Q_FILECNT")
    pending = tlm("CFS_RADIO CF_HKPACKET CH0_PENDING_Q_FILECNT")
    inprogress = tlm("CFDP CFDP_ENGINE_HK ENG_INPROGRESSTRANS")
       
    # Request 
    attempts = num_attemps
    while(attempts > 0)
        cmd("CFS_RADIO CF_PLAYBACK_FILE with CLASS 1, CHAN 0, PRIORITY 1, PRESERVE KEEP, PEER_ENTITY_ID '0.21', SRCFILENAME '#{sc_file_name}', DSTFILENAME '#{gsw_file_name}'")
        timeout = request_timeout
        while (timeout > 0)
            puts("Attempt #{num_attemps - attempts} - Trying to confirm command... #{request_timeout - timeout}")
            sleep(1)
            timeout = timeout - 1

            latest_active = tlm("CFS_RADIO CF_HKPACKET CH0_ACTIVE_Q_FILECNT")
            latest_cf_cmd_cnt = tlm("CFS_RADIO CF_HKPACKET CMDCOUNTER")
            latest_pending = tlm("CFS_RADIO CF_HKPACKET CH0_PENDING_Q_FILECNT")
            latest_inprogress = tlm("CFDP CFDP_ENGINE_HK ENG_INPROGRESSTRANS")

            if ( (latest_active > active) ||
                 (latest_cf_cmd_cnt > cf_cmd_cnt) ||
                 (latest_pending > pending) ||
                 (latest_inprogress > inprogress) )
                # Confirmed command received
                puts("Confirmed request of #{sc_file_name}")
                timeout = -1
            end            
        end
        if (timeout == -1)
            attempts = -1
        else
            attempts = attempts - 1
        end
    end
    
    # Wait until complete
    timeout = complete_timeout
    while(timeout > 0)
        puts("Waiting for downlink of #{sc_file_name} to complete... #{complete_timeout - timeout}")
        sleep(1)
        timeout = timeout - 1
        
        latest_cf_sent = tlm("CFS_RADIO CF_HKPACKET CH0_FILES_SENT")
        
        if (latest_cf_sent > cf_sent)
            # File transfer complete!
            puts("Confirmed transfer complete of #{gsw_file_name}!")
            timeout = 0    
        end
    end
end

# Jam spacecraft mission elapsed time with the current time on the ground
# Note that this call requires the "leo_bus" target to be in use
def set_met_from_ground()
    # Constant number of seconds from Jan 1, 1958 to Jan 1, 1970 (4383 days)
    seconds_from_tai_to_unix_epoch = 378691200
    set_met_timeout = 30
    attempts = 0
    max_attempts = 5
    set_met_success = false

    # Obtain starting command counters
    wait_check_packet("CFS_RADIO", "CFE_TIME_HKPACKET", 1, set_met_timeout)
    time_cmd_counter = tlm("CFS_RADIO CFE_TIME_HKPACKET CMDCOUNTER")
    wait_check_packet("LEO_BUS_RADIO", "LEO_BUS_HK_TLM_T", 1, set_met_timeout)
    leo_bus_cmd_counter = tlm("LEO_BUS_RADIO LEO_BUS_HK_TLM_T COMMANDCOUNT")

    while attempts < max_attempts and set_met_success == false
        # Obtain current unix timestamp in UTC from ruby
        current_time = Time.now.getutc.to_i

        # Convert current unix time to TAI time
        cfs_seconds = current_time + seconds_from_tai_to_unix_epoch
        # Subseconds must be 0 or cFS will reject the new time
        cfs_subseconds = 0

        # Send the command to cFS
        cmd("CFS_RADIO CFE_TIME_SET_MET with SECONDS #{cfs_seconds}, MICROSECONDS #{cfs_subseconds}")
        wait_check_packet("CFS_RADIO", "CFE_TIME_HKPACKET", 1, set_met_timeout)
        new_time_cmd_counter = tlm("CFS_RADIO CFE_TIME_HKPACKET CMDCOUNTER")
        if new_time_cmd_counter > time_cmd_counter
            set_met_success = true
        end
        attempts = attempts + 1
    end
    raise 'Failed to set cFS MET!' if attempts >= max_attempts

    # Reset previous number of attempts
    attempts = 0
    set_met_success = false

    while attempts < max_attempts and set_met_success == false
        # Tell LEO_BUS to save the new time to disk
        cmd("LEO_BUS_RADIO LEO_BUS_SAVE_MET_TO_FILE_CC")
        wait_check_packet("LEO_BUS_RADIO", "LEO_BUS_HK_TLM_T", 1, set_met_timeout)
        new_leo_bus_cmd_counter = tlm("LEO_BUS_RADIO LEO_BUS_HK_TLM_T COMMANDCOUNT")
        if new_leo_bus_cmd_counter > leo_bus_cmd_counter
            set_met_success = true
        end
        attempts = attempts + 1
    end
    raise 'Failed to send LEO_BUS SAVE_MET_TO_FILE command!' if attempts >= max_attempts
end

def set_met()
    # Constant number of seconds from Jan 1, 1958 to Jan 1, 1970 (4383 days)
    seconds_from_tai_to_unix_epoch = 378691200
    set_met_timeout = 5
    set_met_success = false

    # Request current spacecraft time
    cmd("CFS_RADIO CFE_TIME_SEND_HK")

    # Obtain current unix timestamp from ruby
    current_time = Time.now.to_i

    # Convert current unix time to TAI time
    cfs_seconds = current_time + seconds_from_tai_to_unix_epoch
    # Subseconds must be 0 or cFS will reject the new time
    cfs_subseconds = 0

    # Send the command to cFS
    cmd("CFS_RADIO CFE_TIME_SET_MET with SECONDS #{cfs_seconds}, MICROSECONDS #{cfs_subseconds}")
end

def set_met_debug()
    # Constant number of seconds from Jan 1, 1958 to Jan 1, 1970 (4383 days)
    seconds_from_tai_to_unix_epoch = 378691200
    set_met_timeout = 5
    set_met_success = false

    # Request current spacecraft time
    cmd("CFS CFE_TIME_SEND_HK")

    # Obtain current unix timestamp from ruby
    current_time = Time.now.to_i

    # Convert current unix time to TAI time
    cfs_seconds = current_time + seconds_from_tai_to_unix_epoch
    # Subseconds must be 0 or cFS will reject the new time
    cfs_subseconds = 0

    # Send the command to cFS
    cmd("CFS CFE_TIME_SET_MET with SECONDS #{cfs_seconds}, MICROSECONDS #{cfs_subseconds}")
end
