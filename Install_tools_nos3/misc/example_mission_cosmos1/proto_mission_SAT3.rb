# INIT Mission 
# Activate telemetry
cmd("CFS TO_ENABLE_OUTPUT with DEST_IP '127.0.0.1', DEST_PORT 5011")
wait(3)
# Activate Sensors
cmd("NOVATEL_OEM615 NOVATEL_OEM615_ENABLE_CC")
wait(1)
cmd("GENERIC_TORQUER GENERIC_TORQUER_ENABLE_CC")
wait(1)
cmd("GENERIC_MAG GENERIC_MAG_ENABLE_CC")
wait(1)
cmd("GENERIC_IMU GENERIC_IMU_ENABLE_CC")
wait(1)
cmd("GENERIC_CSS GENERIC_CSS_ENABLE_CC")
wait(1)
cmd("GENERIC_FSS GENERIC_FSS_ENABLE_CC")
wait(1)
cmd("GENERIC_STAR_TRACKER GENERIC_STAR_TRACKER_ENABLE_CC")
wait(10)
# Check Arducam 
cmd("ARDUCAM CAM_HW_CHECK_CC")
wait(10)

# LOOPS on orbits
# loop on 10 orbits 
10.times do |orbit|
# Altitude is about 1300km so duration should be about 110 min 
# each orbit should be shifted by about 28 deg in longitude
shiftLON = 28
wait(1)
# Activate ADCS modes
cmd("GENERIC_ADCS GENERIC_ADCS_SET_MOMENTUM_MANAGEMENT_CC with GNC_MODE 'ON'")
# start the mission - Earth Pointing
wait(1)
cmd("GENERIC_ADCS GENERIC_ADCS_SET_MODE_CC with GNC_MODE 'NORMAL_MODE'")
wait(1)
LON = 9.1824-shiftLON*orbit
eval("cmd('GENERIC_ADCS GENERIC_ADCS_SET_TARGET_CC with LATITUDE 45.4685, LONGITUDE #{LON}')") # Milano #orbit 1
wait(1)
wait_check_expression("tlm('NOVATEL_OEM615 NOVATEL_OEM615_DATA_TLM ECEF_Z') < 5550000", 100000)
wait(1)
cmd("CFS CFE_SB_RESET_CTRS")
wait(1)
# Photo 1
cmd("ARDUCAM CAM_EXP3_CC") 
wait(60)
LON = 14.2681-shiftLON*orbit
eval("cmd('GENERIC_ADCS GENERIC_ADCS_SET_TARGET_CC with LATITUDE 40.8518, LONGITUDE #{LON}')") # Napoli #orbit 1
wait(1)
cmd("CFS CFE_SB_RESET_CTRS")
wait(1)
wait_check_expression("tlm('NOVATEL_OEM615 NOVATEL_OEM615_DATA_TLM ECEF_Z') < 5000000", 100000)
# Photo 2
cmd("ARDUCAM CAM_EXP3_CC")
wait(60)
LON = 33.3823-shiftLON*orbit
eval("cmd('GENERIC_ADCS GENERIC_ADCS_SET_TARGET_CC with LATITUDE 35.1856, LONGITUDE #{LON}')") # Nicosia #orbit 1
wait(1)
cmd("CFS CFE_SB_RESET_CTRS")
wait(10)
cmd("CFS CFE_EVS_RESET_ALL_FILTERS with APPNAME 'CFE_SB'") # TBD
wait(1)
wait_check_expression("tlm('NOVATEL_OEM615 NOVATEL_OEM615_DATA_TLM ECEF_Z') < 3800000", 100000)
# Photo 3
cmd("ARDUCAM CAM_EXP3_CC")
wait(300)
# Do some cleaning 
cmd("CFS CFE_ES_CLEAR_ERLOG")
wait(1)
cmd("CFS CFE_ES_CLEAR_SYSLOG")
wait(1)
cmd("CFS CFE_SB_RESET_CTRS")
wait(1)
wait(5)
# Switch to SUNSAFE
cmd("GENERIC_ADCS GENERIC_ADCS_SET_MODE_CC with GNC_MODE 'SUNSAFE_MODE'")
wait(3)
cmd("GENERIC_FSS GENERIC_FSS_NOOP_CC")
wait(3)
cmd("GENERIC_CSS GENERIC_CSS_NOOP_CC")
wait(3)
cmd("GENERIC_EPS GENERIC_EPS_REQ_HK")
wait(3)
cmd("CFS CFE_EVS_RESET_ALL_FILTERS with APPNAME 'CFE_SB'") # TBD
wait(3)
cmd("CFS CF_NOOP")
wait(3)
cmd("CFS CFE_SB_RESET_CTRS")
wait(3)
# Switch to BDOT
wait_check_expression("tlm('NOVATEL_OEM615 NOVATEL_OEM615_DATA_TLM ECEF_Z') < 0", 100000)
cmd("GENERIC_ADCS GENERIC_ADCS_SET_MODE_CC with GNC_MODE 'BDOT_MODE'")
wait(1000)
cmd("GENERIC_ADCS GENERIC_ADCS_SET_MODE_CC with GNC_MODE 'SUNSAFE_MODE'")
wait(1000)
cmd("GENERIC_ADCS GENERIC_ADCS_SET_MOMENTUM_MANAGEMENT_CC with GNC_MODE 'OFF'")
wait(1)
cmd("GENERIC_ADCS GENERIC_ADCS_SET_MODE_CC with GNC_MODE 'PASSIVE'")
wait(1)
# add more regular commands here 
# add condition for orbit loop 
wait_check_expression("tlm('NOVATEL_OEM615 NOVATEL_OEM615_DATA_TLM ECEF_Z') > 5826000", 100000)
# 
wait_check_expression("tlm('NOVATEL_OEM615 NOVATEL_OEM615_DATA_TLM ECEF_Z') < 5825000", 100000)
# orbit counter increase
orbit = orbit +1
end


















