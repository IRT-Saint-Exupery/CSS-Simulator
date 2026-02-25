-- © Copyright European Space Agency, 2023
-- This file is subject to the terms and conditions defined in file 'LICENSE.md',
-- which is part of this source code package. No part of the package,
-- including this file, may be copied, modified, propagated, or distributed except
-- according to the terms contained in the file ‘LICENSE.md’.

local tm_frame_protocol = Proto("tm_frame", "TM FRAME")

--------------------------
-- Register preferences --
--------------------------
tm_frame_protocol.prefs.encrypted = Pref.bool("Encrypted", false, "Whether the data is encrypted or not, do we continue dissecting or not")
tm_frame_protocol.prefs.crc = Pref.bool("CRC", false, "Cyclic Redundancy Check")

-- Other preferences
tm_frame_protocol.prefs.check_crc = Pref.bool("Check Frame CRC", false, "Whether we check TM Frame CRC or not")

local PREFERENCES = 0
local DEFAULT_TM = 1
local NO_SDLS = 2
local sdls_config_table = {
    {1, "Preferences", PREFERENCES},
    {2, "Default TM", DEFAULT_TM},
    {3, "No SDLS", NO_SDLS}
}
tm_frame_protocol.prefs.sdls_config = Pref.enum("SDLS configuration", nil, "SDLS configuration, overwrites preferences", sdls_config_table)
local sdls_configs = {
    -- Based on Preferences
    nil,
    -- DEFAULT TM
    {
        spi_length = 2,
        iv_length = 12,
        sn_length = 0,
        pad_length = 0,
        mac_length = 16,
    },
	-- No SDLS
	{
        spi_length = 0,
        iv_length = 0,
        sn_length = 0,
        pad_length = 0,
        mac_length = 0,
   },
}

local prefs = tm_frame_protocol.prefs

-------------------
-- Define fields --
-------------------
local f = tm_frame_protocol.fields

-- Block Size
local HEADER_LENGTH = 6
local OCF_LENGTH = 4
local FECF_LENGTH = 2
local FRAME_STATUS_LENGTH = 2
local HEADER_SPP_LENGTH = 6

local ONLY_IDLE_DATA = 2046
local CONTINUOUS_PACKET = 2047
local VC_PACKET_SERVICE = 0
local VC_ACCCESS_SERVICE = 1

-- Header Fields
f.transfer_frame_version_number = ProtoField.uint16("tm_frame_protocol.transfer_frame_version_number",
    "Transfer Frame Version Number", base.DEC, nil, 0xc000)
f.spacecraft_id = ProtoField.uint16("tm_frame_protocol.spacecraft_id", "Spacecraft ID", base.DEC, nil, 0x3ff0)
f.vc_id = ProtoField.uint16("tm_frame_protocol.vc_id", "Virtual Channel ID", base.DEC, nil, 0x000e)
f.ocf_flag = ProtoField.uint16("tm_frame_protocol.ocf_flag", "OCF Flag", base.DEC, nil, 0x0001)
f.master_channel_frame_count = ProtoField.uint8("tm_frame_protocol.master_channel_frame_count",
    "Master Channel Frame Count", base.DEC, nil, 0xff)
f.virtual_channel_frame_count = ProtoField.uint8("tm_frame_protocol.virtual_channel_frame_count",
    "Virtual Channel Frame Count", base.DEC, nil, 0xff)
f.transfer_frame_data_field_status = ProtoField.uint16("tm_frame_protocol.transfer_frame_data_field_status",
    "Transfer Frame Data Field Status", base.DEC, nil, 0xffff)

-- Transfer Frame Data Field Status Fields
f.transfer_frame_secondary_header_flag = ProtoField.uint16("tm_frame_protocol.transfer_frame_secondary_header_flag",
    "Transfer Frame Secondary Header Flag", base.DEC, nil, 0x8000)
f.synch_flag = ProtoField.uint16("tm_frame_protocol.synch_flag", "Synch Flag", base.DEC, nil, 0x4000)
f.packet_order_flag = ProtoField.uint16("tm_frame_protocol.packet_order_flag", "Packet Order Flag", base.DEC, nil, 0x2000)
f.segment_length_id = ProtoField.uint16("tm_frame_protocol.segment_length_id", "Segment Length ID", base.DEC, nil, 0x1800)
f.first_header_pointer = ProtoField.uint16("tm_frame_protocol.first_header_pointer", "First Header Pointer", base.DEC,
    nil, 0x07ff)

f.transfer_frame_secondary_header_version_number = ProtoField.uint8(
    "tm_frame_protocol.transfer_frame_secondary_header_version_number", "Version Number", base.DEC, nil, 0xc0)
f.transfer_frame_secondary_header_length = ProtoField.uint8("tm_frame_protocol.transfer_frame_secondary_header_length",
    "Length", base.DEC, nil, 0x2F)

-- FECF field
f.checksum = ProtoField.uint16("tm_frame_protocol.frame_error_control_field", "Checksum", base.HEX)

-- CLCW fields
f.control_world_type = ProtoField.uint8("tm_frame_protocol.control_world_type", "Control World Type", base.DEC, nil, 0x80)
f.clcw_version_number = ProtoField.uint8("tm_frame_protocol.clcw_version_number", "clcw Version Number", base.DEC, nil,
    0x60)
f.status_field = ProtoField.uint8("tm_frame_protocol.status_field", "Status Field", base.DEC, nil, 0x1c)
f.cop_in_effect = ProtoField.uint8("tm_frame_protocol.cop_in_effect", "COP In Effect", base.DEC, nil, 0x03)
f.vc_id_clcw = ProtoField.uint8("tm_frame_protocol.vc_id_clcw", "VC ID", base.DEC, nil, 0xfc)
f.reserved_space = ProtoField.uint8("tm_frame_protocol.reserved_space", "Reserved Space", base.DEC, nil, 0x03)
f.flags = ProtoField.uint8("tm_frame_protocol.flags", "Flags", base.DEC, nil, 0x3e)
f.farm_b_counter = ProtoField.uint8("tm_frame_protocol.farm_b_counter", "Farm-B Counter", base.DEC, nil, 0x06)
f.reserved_space2 = ProtoField.uint8("tm_frame_protocol.reserved_space2", "Reserved Space2", base.DEC, nil, 0x01)
f.report_value = ProtoField.uint8("tm_frame_protocol.report_value", "Report Value", base.DEC, nil, 0xff)

-- Flags Fields in Communications Link Control Word
f.no_rf_available = ProtoField.uint8("tm_frame_protocol.no_rf_available", "No RF Available", base.DEC, nil, 0x80)
f.no_bit_lock = ProtoField.uint8("tm_frame_protocol.no_bit_lock", "No Bit Lock", base.DEC, nil, 0x40)
f.lock_out = ProtoField.uint8("tm_frame_protocol.lock_out", "Lock Out", base.DEC, nil, 0x20)
f.wait = ProtoField.uint8("tm_frame_protocol.wait", "Wait", base.DEC, nil, 0x10)
f.retransmit = ProtoField.uint8("tm_frame_protocol.retransmit", "retransmit", base.DEC, nil, 0x08)

-- Frame Security Report Fields
f.fsr_version_number = ProtoField.uint8("tm_frame_protocol.fsr_version_number", "FSR Version Number", base.DEC, nil, 0x70)
f.alarm_field = ProtoField.uint8("tm_frame_protocol.alarm_field", "Alarm Field", base.DEC, nil, 0x08)
f.bad_sn_flag = ProtoField.uint8("tm_frame_protocol.bad_sn_flag", "Bad SN Flag", base.DEC, nil, 0x04)
f.bad_mac_flag = ProtoField.uint8("tm_frame_protocol.bad_mac_flag", "Bad MAC Flag", base.DEC, nil, 0x02)
f.bad_sa_flag = ProtoField.uint8("tm_frame_protocol.bad_sa_flag", "Bad SA Flag", base.DEC, nil, 0x01)
f.last_spi_used = ProtoField.uint16("tm_frame_protocol.last_spi_used", "Last SPI Used", base.DEC)
f.sn_value = ProtoField.uint8("tm_frame_protocol.sn_value", "SN Value", base.DEC)

------------
-- Helper --
------------

require("crc16")

-- Reassembly buffers for SP (one per VC except Idle)
local assemblyBuff = {}
local sp_assembly_buffs = {}
local assembled_sp_tvb
local fec_buffer_length = 0

-- Cache data dissector to avoid many Dissector.get() calls
local data_dis = Dissector.get("data")

-- Empty reassembly buffer
local function clear_asm_buffer(vc_id)
	assemblyBuff[vc_id]:set_size(0)
end

-- Append bytes to an assembly buffer
local function add_to_asm_buff(vc_id, data_buffer)
	-- Create reassembly buffer for this VC if not yet done
	if assemblyBuff[vc_id] == nil then
		assemblyBuff[vc_id] = data_buffer:bytes()
	else
		assemblyBuff[vc_id]:append(data_buffer:bytes())
	end
end

-- Get number of bytes in reassembly buffer
local function get_bytes_in_asm_buffer(vc_id)
	return assemblyBuff[vc_id]:len()
end

local function parse_user_data(tree, pinfo, buffer, vc_id)
    local length_buffer = buffer:len()
	-- Use of a while loop to avoid recursion
	local bytes_consumed = 0
	while bytes_consumed < length_buffer do
		local remaining_length = length_buffer - bytes_consumed
		-- Here, we are sure to have at least one byte and we can read the PVN
		-- Packet Version number (PVN) and adapt our processing accordingly
		-- https://sanaregistry.org/r/packet_version_number/
		local packet_version_number = buffer(bytes_consumed,1):bitfield(0,3)
		-- Normally, according to 132.0-B-3, section 4.1.4.4, the same TM transfer frame
		-- shall not mix SP with VCA_SDU, but our code is generic enough.
		if packet_version_number == 0 then
			-- Here we have a source packet for sure.
			local sp_length
			if remaining_length < HEADER_SPP_LENGTH then
				-- Not enough space to contain SP length. The SP is thus obviously
				-- fragmented over at least two frames including this one. Trick to make
				-- sure that remaining length will never be greater than the sp_length
				sp_length = HEADER_SPP_LENGTH
			else
				-- sp_length is more meaningfull than frame length and it is
				-- better to add 1 in the logic below (the user data length is indeed one more).
				local user_data_length = buffer(bytes_consumed+4,2):uint() + 1
				sp_length = HEADER_SPP_LENGTH + user_data_length
			end

			if remaining_length >= sp_length then
				-- One packet or more. Advance dissection by 1 SP
				local packet_buffer = buffer:range(bytes_consumed,sp_length):tvb()
				Dissector.get("space_packet_protocol"):call(packet_buffer,pinfo,tree)
				bytes_consumed = bytes_consumed + sp_length
			else
				local packet_buffer = buffer:range(bytes_consumed,remaining_length):tvb()
				data_dis:call(packet_buffer,pinfo,tree)
				if sp_assembly_buffs[pinfo.number] == nil then
					-- Start reconstructing the packet
					add_to_asm_buff(vc_id, packet_buffer:range())
				end
				return
			end
		elseif packet_version_number == 7 then
			local packet_buffer = buffer:range(bytes_consumed,remaining_length):tvb()
			Dissector.get("encapsulation_packet_protocol"):call(packet_buffer,pinfo,tree)
			return
		else
			-- Unknown packet
			local packet_buffer = buffer:range(bytes_consumed,remaining_length):tvb()
			data_dis:call(packet_buffer,pinfo,tree)
			return
		end
	end
end

---------------
-- Dissector --
---------------

function tm_frame_protocol.init()
	-- Create an assembly buffer for each VC but the idle one
	for i=0,6 do
		assemblyBuff[i] = ByteArray.new()
	end
	sp_assembly_buffs = {}
	if tm_frame_protocol.prefs.crc then
		fec_buffer_length = FECF_LENGTH
	end
end

function tm_frame_protocol.dissector(buffer, pinfo, tree)
    local subtree_name = "TM Space Data Link Frame"
    if tm_frame_protocol.prefs.crc then
        subtree_name = subtree_name .. " + CRC"
    end
    pinfo.cols.protocol = tm_frame_protocol.name

    local subtree = tree:add(tm_frame_protocol, buffer(), subtree_name)

    local offset = 0

    local header_buffer = buffer:range(offset, HEADER_LENGTH):tvb()
    local header_tree = subtree:add(tm_frame_protocol, header_buffer(), "Transfer Frame Primary Header")
    header_tree:add(f.transfer_frame_version_number, buffer(offset, 2))
    local tfvn = buffer(offset, 2):bitfield(0, 2)
    header_tree:add(f.spacecraft_id, buffer(offset, 2))
    local sc_id = buffer(offset, 2):bitfield(2, 10)
    header_tree:add(f.vc_id, buffer(offset, 2))
    local vc_id = buffer(offset, 2):bitfield(12, 3)
    header_tree:add(f.ocf_flag, buffer(offset, 2))
    local ocf_flag = buffer(offset, 2):bitfield(15, 1)
    offset = offset + 2
    header_tree:add(f.master_channel_frame_count, buffer(offset, 1))
    offset = offset + 1
    header_tree:add(f.virtual_channel_frame_count, buffer(offset, 1))
    offset = offset + 1

    local frame_status_buffer = buffer:range(offset, FRAME_STATUS_LENGTH):tvb()
    local frame_status_tree = header_tree:add(tm_frame_protocol, frame_status_buffer(), "Frame Data Fields Status")

    frame_status_tree:add(f.transfer_frame_secondary_header_flag, buffer(offset, 2))
    local secondary_header_flag = buffer(offset, 1):bitfield(0, 1)
    frame_status_tree:add(f.synch_flag, buffer(offset, 2))
    local synch_flag = buffer(offset, 1):bitfield(1, 1)
    frame_status_tree:add(f.packet_order_flag, buffer(offset, 2))
    frame_status_tree:add(f.segment_length_id, buffer(offset, 2))
    frame_status_tree:add(f.first_header_pointer, buffer(offset, 2))
    local first_header_pointer = buffer(offset, 2):bitfield(5, 11)
    offset = offset + 2

    local second_header_data_buffer = nil

    -- Transfer Frame Secondary Header
    if secondary_header_flag == 1 then
        local second_header_data_length = buffer(offset, 1):bitfield(2, 6)
        local second_header_buffer = buffer:range(offset, 1 + second_header_data_length):tvb()
        local second_header_tree = subtree:add(tm_frame_protocol, second_header_buffer(),
            "Transfer Frame Secondary Header")

        second_header_tree:add(f.transfer_frame_secondary_header_version_number, buffer(offset, 1))
        second_header_tree:add(f.transfer_frame_secondary_header_length, buffer(offset, 1))
        local second_header_data_length = buffer(offset, 1):bitfield(2, 6)
        offset = offset + 1

        second_header_data_buffer = buffer:range(offset, second_header_data_length):tvb()
        local second_header_tree = second_header_tree:add(tm_frame_protocol, second_header_data_buffer(),
            "Transfer Frame Secondary Data")
        Dissector.get("data"):call(second_header_data_buffer, pinfo, second_header_tree)
        offset = offset + second_header_data_length
    end

    local ocf_buffer_length = 0
    if ocf_flag == 1 then
        ocf_buffer_length = OCF_LENGTH
    end

    -- SDLS, we set some stuff here because we need it early in the dissection process
    local sdls_configuration = sdls_configs[prefs.sdls_config + 1]
    local sdls_header_length = SDLS_header_get_length(sdls_configuration)
    local sdls_trailer_length = SDLS_trailer_get_length(sdls_configuration)
    SDLS_header_set_config(sdls_configuration)
    SDLS_trailer_set_config(sdls_configuration)

    -- SDLS header
    if sdls_header_length > 0 then
        Dissector.get("sdls_header"):call(buffer:range(offset, sdls_header_length):tvb(), pinfo, subtree)
        offset = offset + sdls_header_length
    end
    ----------
    -- Data --
    ----------
    local data_offset = ocf_buffer_length
    -- NOTE: old frame 03, was not consistent with the rest, it added FECF_LENGTH only, while the others only add it when CRC is enabled
    -- It also means it was not adding MAC_LENGTH, even though it has a MAC, so I think a bug was fixed here
    -- Otherwise this is an indicator of a change from the previous version, and it can be reversed
    if tm_frame_protocol.prefs.crc then
        data_offset = data_offset + FECF_LENGTH
    end
    data_offset = data_offset + sdls_trailer_length

    -- NOTE: only the old frame_01: moo = NONE and no CRC, adds first_header_pointer here, i don't know why, so i removed it for now
    local data_frame_buffer = buffer:range(offset, buffer:len() - offset - data_offset):tvb()
    if prefs.encrypted then
        --------------------
        -- Encrypted data --
        --------------------
        local encrypted_payload_tree = subtree:add(tm_frame_protocol, data_frame_buffer(), "Encrypted Payload")
        data_dis:call(data_frame_buffer, pinfo, encrypted_payload_tree)
        offset = buffer:len() - data_offset
    else
        ------------------------------
        -- Plaintext data, parse it --
        ------------------------------
        -- Frame Data Field
        -- Evaluate the type of channel: Packet or VCA_SDU
        if synch_flag == VC_PACKET_SERVICE then
            -- TO CHECK
            -- 4.1.2.7.6.3
            -- If the last Packet in the Transfer Frame Data Field of Transfer Frame N spills over into Frame 
            -- M of the same Virtual Channel (N<M), the First Header Pointer in Frame M ignores the 
            -- residue of the split Packet and indicates the start of the next Packet that starts in Frame M

            -- Type of segment 
            if first_header_pointer == CONTINUOUS_PACKET then
                -- Case of an intermediate segment of a SP
                local continuous_tree = subtree:add(tm_frame_protocol, data_frame_buffer, "User Data: Continuous Packet")
                data_dis:call(data_frame_buffer, pinfo,continuous_tree)
                -- Add all data to the reassembly buffer
                add_to_asm_buff(vc_id, data_frame_buffer)
            elseif first_header_pointer == ONLY_IDLE_DATA then
				-- Idle frame. Don't dissect further.
				pinfo.cols.info = "Only IDLE Data"
				local oid_tree = tm_tree:add(tm_frame_protocol, data_frame_buffer, "User Data: Only IDLE Data")
				data_dis:call(data_frame_buffer, pinfo, oid_tree)
				-- On a given VC ID, an OID SP shouldn't be in between consecutive fragments of the same source packet.
				-- Thus, reset reassembly
				clear_asm_buffer(vc_id)
            else
                -- At least one SP in this frame
                local bundle_spp_tree = subtree:add(tm_frame_protocol, data_frame_buffer, "User Data: Bundle Packets")
                if first_header_pointer ~= 0 then
                    -- A segmented SP is ending on this particular frame.
                    local asm_spp_tree = bundle_spp_tree:add(tm_frame_protocol, data_frame_buffer:range(0,first_header_pointer), "Space Packet ending segment")
                    local assembled_sp_tvb
                    data_dis:call(data_frame_buffer:range(0,first_header_pointer):tvb(), pinfo, asm_spp_tree)
                    -- If the assembly buffer for this VC Id is empty we are in a corner case of a capture starting with an
                    -- ending packet (or a dropped starting packet). Thus, only "glue" trailing fragment if we have something
                    -- to "glue" with.
                    if get_bytes_in_asm_buffer(vc_id) ~= 0 then
                        -- Only reassemble packet once because of the way Wireshark handles packets (the current packet
                        -- being processed once more when the user clicks on it)
                        local assembled_bytes
                        if sp_assembly_buffs[pinfo.number] == nil then
                            -- First add it at then end of the reassembly buffer
                            add_to_asm_buff(vc_id, data_frame_buffer:range(0,first_header_pointer))
                            -- Then, extract the reassembled SP and dissect it
                            assembled_bytes = ByteArray.new()
                            assembled_bytes:append(assemblyBuff[vc_id])
                            -- Associate the bytes to the packet in a global table
                            sp_assembly_buffs[pinfo.number] = assembled_bytes
                        else
                            assembled_bytes = sp_assembly_buffs[pinfo.number]
                        end
                        assembled_sp_tvb = ByteArray.tvb(assembled_bytes, "Assembled SP")
                        local full_sp_range = assembled_sp_tvb:range()
                        -- Use the parse_user_data to dissect what has been reassembled. This will lead
                        -- to a single loop in this function since a single, complete packet is passed.
                        parse_user_data(asm_spp_tree, pinfo, full_sp_range, vc_id)
					end
                end
                -- Whatever the case, reset reassembly buffer. This is to handle final reassembly
                -- but also the case of a missing final SP whilst the reassembly buffer was in use
                clear_asm_buffer(vc_id)
                local data_frame_buffer = buffer:range(offset + first_header_pointer, buffer:len() - offset - sdls_trailer_length - ocf_buffer_length - first_header_pointer - fec_buffer_length):tvb()
                parse_user_data(bundle_spp_tree, pinfo, data_frame_buffer, vc_id)
            end
        elseif synch_flag == VC_ACCCESS_SERVICE then
                local vc_access_tree = subtree:add(tm_frame_protocol, data_frame_buffer, "VC Access Service Data")
                -- TODO: look at data frame buffer here
                parse_user_data(vc_access_tree, pinfo, data_frame_buffer, vc_id)
        else
            error("Unkown Data Type")
        end

        offset = offset + data_frame_buffer:len()
    end

    -- SDLS trailer -> authentication tag
    if sdls_trailer_length > 0 then
        Dissector.get("sdls_trailer"):call(buffer:range(offset, sdls_trailer_length):tvb(), pinfo, subtree)
        offset = offset + sdls_trailer_length
    end

    -- Operational Control Field
    if ocf_flag == 1 then
        local ocf_buffer = buffer:range(offset, OCF_LENGTH)
        local control_word_type = buffer(offset, 1):bitfield(0, 1)

        if control_word_type == 0 then
            -- Type 1 OCF / CLCW
            local ocf_tree = subtree:add(tm_frame_protocol, ocf_buffer(), "OCF CLCW")
            ocf_tree:add(f.control_world_type, buffer(offset, 1))
            ocf_tree:add(f.clcw_version_number, buffer(offset, 1))
            ocf_tree:add(f.status_field, buffer(offset, 1))
            ocf_tree:add(f.cop_in_effect, buffer(offset, 1))
            offset = offset + 1
            ocf_tree:add(f.vc_id_clcw, buffer(offset, 1))
            ocf_tree:add(f.reserved_space, buffer(offset, 1))
            offset = offset + 1

            local flag_tree = ocf_tree:add(tm_frame_protocol, buffer(offset, 1), "Flags")
            flag_tree:add(f.no_rf_available, buffer(offset, 1))
            flag_tree:add(f.no_bit_lock, buffer(offset, 1))
            flag_tree:add(f.lock_out, buffer(offset, 1))
            flag_tree:add(f.wait, buffer(offset, 1))
            flag_tree:add(f.retransmit, buffer(offset, 1))

            ocf_tree:add(f.farm_b_counter, buffer(offset, 1))
            ocf_tree:add(f.reserved_space2, buffer(offset, 1))
            offset = offset + 1
            ocf_tree:add(f.report_value, buffer(offset, 1))
            offset = offset + 1
        elseif control_word_type == 1 then
            local bit1_version_number = buffer(offset, 1):bitfield(1, 1)
            if bit1_version_number == 0 then
                -- Project specific
                local project_specific_tree = subtree:add(tm_frame_protocol, ocf_buffer(),
                    "Operational Control Field Project Specific")
                data_dis:call(data_frame_buffer, pinfo, project_specific_tree)
            elseif bit1_version_number == 1 then
                -- Type 2 OCF
                -- SDLS FSR
                local fsr_tree = subtree:add(tm_frame_protocol, ocf_buffer(), "OCF: Frame Security Report")
                fsr_tree:add(f.control_world_type, buffer(offset, 1))
                fsr_tree:add(f.fsr_version_number, buffer(offset, 1))
                fsr_tree:add(f.alarm_field, buffer(offset, 1))
                fsr_tree:add(f.bad_sn_flag, buffer(offset, 1))
                fsr_tree:add(f.bad_mac_flag, buffer(offset, 1))
                fsr_tree:add(f.bad_sa_flag, buffer(offset, 1))
                offset = offset + 1
                fsr_tree:add(f.last_spi_used, buffer(offset, 2))
                offset = offset + 2
                fsr_tree:add(f.sn_value, buffer(offset, 1))
                offset = offset + 1
            end
        end
    end
 
	-- CRC / Frame error control
	if tm_frame_protocol.prefs.crc then
		local fecf_buffer = buffer:range(offset, FECF_LENGTH):tvb()
		offset = offset + FECF_LENGTH
		local frame_error_control_field_tree = subtree:add(tm_frame_protocol, fecf_buffer(),
			"Frame Error Control Field")
		local crcVerification = " [unverified]"
		if tm_frame_protocol.prefs.check_crc == true then
			local crc = crc16(buffer)
			crcVerification = " [OK]"
			if crc ~= 0 then
				crcVerification = " [KO]"
			end
			frame_error_control_field_tree:add(f.checksum, fecf_buffer()):append_text(crcVerification)
		end
	end
end

local udp_port = DissectorTable.get("udp.port")
local tcp_port = DissectorTable.get("tcp.port")
udp_port:add(70001, tm_frame_protocol)
tcp_port:add(70010, tm_frame_protocol)

function tm_frame_protocol.prefs_changed()
    if tm_frame_protocol.prefs.crc then
		fec_buffer_length = FECF_LENGTH
	else
		fec_buffer_length = 0
	end
end