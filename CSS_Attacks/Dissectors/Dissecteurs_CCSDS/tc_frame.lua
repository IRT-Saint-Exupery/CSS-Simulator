-- © Copyright European Space Agency, 2023
-- This file is subject to the terms and conditions defined in file 'LICENSE.md',
-- which is part of this source code package. No part of the package,
-- including this file, may be copied, modified, propagated, or distributed except
-- according to the terms contained in the file ‘LICENSE.md’.

local tc_frame_protocol = Proto("TC_FRAME", "TC FRAME")

--------------------------
-- Register preferences --
--------------------------
tc_frame_protocol.prefs.encrypted = Pref.bool("Encrypted", false, "Whether the data is encrypted or not, do we continue dissecting or not")
tc_frame_protocol.prefs.segment_header = Pref.bool("Segment header", false)
tc_frame_protocol.prefs.crc = Pref.bool("CRC", false, "Cyclic Redundancy Check")

-- Other preferences
tc_frame_protocol.prefs.check_crc = Pref.bool("Check Frame CRC", false, "Whether we check TC Frame CRC or not")

local PREFERENCES = 0
local DEFAULT_TC = 1
local NO_SDLS = 2
local sdls_config_table = {
    {1, "Preferences", PREFERENCES},
    {2, "Default TC", DEFAULT_TC},
    {3, "No SDLS", NO_SDLS}
}
tc_frame_protocol.prefs.sdls_config = Pref.enum("SDLS configuration", nil, "SDLS configuration, overwrites preferences", sdls_config_table)
local sdls_configs = {
    -- Based on Preferences
    nil,
    -- Default TC
    {
        spi_length = 2,
        iv_length = 0,
        sn_length = 4,
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

local prefs = tc_frame_protocol.prefs

-------------------
-- Define fields --
-------------------
local f = tc_frame_protocol.fields
local enums = {}

-- Block Size
local TC_HEADER_LENGTH = 5
local SEGMENT_HEADER_LENGTH = 1
local SPP_HEADER_LENGTH = 6
local FEFC_LENGTH = 2

enums.bypass_flag = {
    [0] = "Type-A",
    [1] = "Type-B",
}

enums.control_command_flag = {
    [0] = "Type-D",
    [1] = "Type-C",
}

enums.sequence_flags = {
    [0] = "Continuation segment",
    [1] = "First segment",
    [2] = "Last segment",
    [3] = "Unsegmented data",
}

-- Header Fields
f.transfer_frame_version_number = ProtoField.uint16("tc_frame_protocol.transfer_frame_version_number", "Version",
    base.DEC, nil, 0xc000)
f.bypass_flag = ProtoField.uint16("tc_frame_protocol.bypass_flag", "Bypass", base.DEC, enums.bypass_flag, 0x2000)
f.control_command_flag = ProtoField.uint16("tc_frame_protocol.control_command_flag", "Control Command", base.DEC,
    enums.control_command_flag, 0x1000)
f.rsvp_space = ProtoField.uint16("tc_frame_protocol.rsvp_space", "Rsvp Space", base.DEC, nil, 0x0c00)
f.spacecraft_id = ProtoField.uint16("tc_frame_protocol.spacecraft_id", "Spacecraft ID", base.DEC, nil, 0x03ff)
f.vc_id = ProtoField.uint16("tc_frame_protocol.vc_id", "VC ID", base.DEC, nil, 0xfc00)
f.frame_length = ProtoField.uint16("tc_frame_protocol.frame_length", "Frame Length", base.DEC, nil, 0x03ff)
f.frame_sequence_number = ProtoField.uint8("tc_frame_protocol.frame_sequence_number", "Frame Sequence Number", base.DEC,
    nil, 0xff)

-- Control Command Fields
f.control_command = ProtoField.string("tc_frame_protocol.control_command", "Command")
f.receiver_frame_sequence_number = ProtoField.uint16("tc_frame_protocol.receiver_frame_sequence_number",
    "Receiver Frame Sequence Number", base.DEC)

-- Sequence Fields
f.sequence_flags = ProtoField.uint8("tc_frame_protocol.sequence_flags", "Sequence Flags", base.DEC, enums.sequence_flags,
    0xc0)
f.map_id = ProtoField.uint8("tc_frame_protocol.map_id", "Map ID", base.DEC, nil, 0x3f)

-- Checksum TC Field
f.checksum = ProtoField.uint16("tc_frame_protocol.frame_error_control_field", "Checksum", base.HEX)

------------
-- Helper --
------------
local function parse_user_data(tree, pinfo, buffer)
    if buffer:len() == 0 then return end
    if buffer:len() < 6 then
        Dissector.get("data"):call(buffer, pinfo, tree)
        return
    end

    -- Determine the type of packet based on the Packer Version number (PVN)
    -- https://sanaregistry.org/r/packet_version_number/
    local packet_version_number = buffer(0, 1):bitfield(0, 3)

    if packet_version_number == 0 then
        local user_data_length = buffer(4, 2):uint()
        local frame_length = SPP_HEADER_LENGTH + user_data_length
        -- More than one packet
        if buffer:len() > frame_length then
            local buffer_leftover = buffer:range(frame_length, buffer:len() - (frame_length + 1)):tvb()
            local packet_buffer = buffer:range(0, frame_length + 1):tvb()
            Dissector.get("space_packet_protocol"):call(packet_buffer, pinfo, tree)
            parse_user_data(tree, pinfo, buffer_leftover)
        elseif buffer:len() < frame_length then
            -- TODO: reconstruct the packet
            Dissector.get("space_packet_protocol"):call(buffer, pinfo, tree)
        elseif buffer:len() == frame_length then
            Dissector.get("space_packet_protocol"):call(buffer, pinfo, tree)
        end
    elseif packet_version_number == 7 then
        Dissector.get("encapsulation_packet_protocol"):call(buffer, pinfo, tree)
    else
        -- Unknown packet
        Dissector.get("data"):call(buffer, pinfo, tree)
    end
end

---------------
-- Dissector --
---------------

function tc_frame_protocol.dissector(buffer, pinfo, tree)
    local subtree_name = "TC Space Data Link Frame"

    if prefs.crc then
        subtree_name = subtree_name .. " + CRC"
    end
    if prefs.segment_header then
        subtree_name = subtree_name .. " + Segment header"
    end

    pinfo.cols.protocol = tc_frame_protocol.name
    pinfo.private.channel_type = 1

    local subtree = tree:add(tc_frame_protocol, buffer(), subtree_name)

    local offset = 0

    local header_buffer = buffer:range(offset, TC_HEADER_LENGTH):tvb()
    local header_tree = subtree:add(tc_frame_protocol, header_buffer(), "Transfer Primary Frame Header ")

    header_tree:add(f.transfer_frame_version_number, buffer(offset, 2))
    local tfvn = buffer(offset, 2):bitfield(0, 2)
    header_tree:add(f.bypass_flag, buffer(offset, 2))
    header_tree:add(f.control_command_flag, buffer(offset, 2))
    header_tree:add(f.rsvp_space, buffer(offset, 2))
    header_tree:add(f.spacecraft_id, buffer(offset, 2))
    local sc_id = buffer(offset, 2):bitfield(6, 10)
    offset = offset + 2
    header_tree:add(f.vc_id, buffer(offset, 2))
    local vc_id = buffer(offset, 2):bitfield(0, 6)
    header_tree:add(f.frame_length, buffer(offset, 2))
    local frame_length = buffer(offset, 2):bitfield(6, 10)
    offset = offset + 2
    header_tree:add(f.frame_sequence_number, buffer(4, 1))
    offset = offset + 1

    local control_command_flag = buffer(0, 1):bitfield(3, 1)
    local control_command_flag_type = enums.control_command_flag[control_command_flag]

    -- Search for fill bits (0x55)
    -- The Synchronization and Channel Coding Sublayer could require the introduction
    -- of these fill data in the encoding process; they are not removed by the decoding
    -- process. Removal of fill is the responsibility of the sublayer above, which delimits
    -- the end of the Transfer Frame(s) and discards extraneous bits (e.g., fill).

    if (frame_length + 1) < buffer:len() then
        buffer = buffer:range(0, frame_length + 1):tvb()
    end

    -- Control Command Information (Type-C)
    if control_command_flag_type == "Type-C" then
        local control_command_buffer = buffer:range(TC_HEADER_LENGTH):tvb()
        local control_command_tree = subtree:add(tc_frame_protocol, control_command_buffer(), "Control Command")

        -- As there is no explicit header information on whether the command is unlock or set V(R), the dissector evaluate the size of the request
        -- UNLOCK: 1 octet
        -- SET V(R): 3 octets

        -- Unlock
        if control_command_buffer:len() == 1 then
            pinfo.cols.info = "Control Command: Unlock"
            control_command_tree:add(f.control_command, "Unlock")
            -- Set V(R)
        elseif control_command_buffer:len() == 3 then
            pinfo.cols.info = "Control Command: Set V(R)"
            control_command_tree:add(f.control_command, "set V(R)")
            offset = offset + 2
            control_command_tree:add(f.receiver_frame_sequence_number, buffer(offset, 1))
            offset = offset + 1
        else
            error("Failed to parse Control Command")
        end

        -- Frame Data Unit (Type-D)
    elseif control_command_flag_type == "Type-D" then
        pinfo.cols.info = "Data"
        ------------------------------
        -- Process preferences here --
        ------------------------------
        if prefs.segment_header then
            local segment_buffer = buffer:range(offset, SEGMENT_HEADER_LENGTH):tvb()
            local segment_tree = subtree:add(tc_frame_protocol, segment_buffer(), "Segment Header")
            segment_tree:add(f.sequence_flags, buffer(offset, 1))
            segment_tree:add(f.map_id, buffer(offset, 1))
            offset = offset + SEGMENT_HEADER_LENGTH
        end

        -- SDLS, we set some stuff here because we need it early in the dissection process
        local sdls_configuration = sdls_configs[prefs.sdls_config+1]
        local sdls_header_length = SDLS_header_get_length(sdls_configuration)
        local sdls_trailer_length = SDLS_trailer_get_length(sdls_configuration)
        SDLS_header_set_config(sdls_configuration)
        SDLS_trailer_set_config(sdls_configuration)

        -- SDLS header
        if sdls_header_length > 0 then
            Dissector.get("sdls_header"):call(buffer:range(offset, sdls_header_length):tvb(), pinfo, subtree)
            offset = offset + sdls_header_length
        end

        -- Data
        local data_offset = 0
        if prefs.crc then
            data_offset = data_offset + FEFC_LENGTH
        end
        data_offset = data_offset + sdls_trailer_length

        local data_frame_buffer = buffer:range(offset, buffer:len() - offset - data_offset):tvb()

        if prefs.encrypted then
            local encrypted_payload_tree = subtree:add(tc_frame_protocol, data_frame_buffer(), "Encrypted Payload")
            Dissector.get("data"):call(data_frame_buffer, pinfo, encrypted_payload_tree)
        else
            parse_user_data(subtree, pinfo, data_frame_buffer)
        end
        offset = buffer:len() - data_offset

        -- SDLS trailer -> authentication tag
        if sdls_trailer_length > 0 then
            Dissector.get("sdls_trailer"):call(buffer:range(offset, sdls_trailer_length):tvb(), pinfo, subtree)
            offset = offset + sdls_trailer_length
        end

        -- CRC / Frame error control
        if prefs.crc then
            local fecf_buffer = buffer:range(offset, FEFC_LENGTH):tvb()
            offset = offset + FEFC_LENGTH
            local frame_error_control_field_tree = subtree:add(tc_frame_protocol, fecf_buffer(),
                "Frame Error Control Field")
            local crcVerification = " [unverified]"
            if prefs.check_crc == true then
                local crc = crc16(buffer)
                crcVerification = " [OK]"
                if crc ~= 0 then
                    crcVerification = " [KO]"
                end
            end
            frame_error_control_field_tree:add(f.checksum, fecf_buffer()):append_text(crcVerification)
        end
    end
end

local udp_port = DissectorTable.get("udp.port")
local tcp_port = DissectorTable.get("tcp.port")
udp_port:add(60001, tc_frame_protocol)
tcp_port:add(60010, tc_frame_protocol)
