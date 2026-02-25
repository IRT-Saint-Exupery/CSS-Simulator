-- © Copyright European Space Agency, 2023
-- This file is subject to the terms and conditions defined in file 'LICENCE.md',
-- which is part of this source code package. No part of the package,
-- including this file, may be copied, modified, propagated, or distributed except
-- according to the terms contained in the file ‘LICENCE.md’.

encapsulation_packet_protocol = Proto("encapsulation_packet_protocol", "Encapsulation Packet Protocol")

local f = encapsulation_packet_protocol.fields
local enums = {}

-- Block size
EPP_MIN_SIZE = 1


-- Header
f.packet_version_number = ProtoField.uint8("encapsulation_packet_protocol.packet_version_number", "Version", base.DEC,
    nil, 0xe0)
f.encapsulation_protocol_id = ProtoField.uint8("encapsulation_packet_protocol.encapsulation_protocol_id",
    "Encapsulation Protocol ID", base.DEC, nil, 0x1C)
f.length_of_length = ProtoField.uint8("encapsulation_packet_protocol.length_of_length", "Length of Length", base.DEC, nil,
    0x03)
f.user_defined_field = ProtoField.uint8("encapsulation_packet_protocol.user_defined_field", "User Defined Field",
    base.DEC, nil, 0xf0)
f.encapsulation_protocol_id_extension = ProtoField.uint8(
"encapsulation_packet_protocol.encapsulation_protocol_id_extension", "Encapsulation Protocl ID Extension", base.DEC, nil,
    0x0f)
f.ccsds_defined_field = ProtoField.uint16("encapsulation_packet_protocol.ccsds_defined_field", "CCSDS Defined Field",
    base.DEC, nil, 0xffff)
f.packet_length1 = ProtoField.uint8("encapsulation_packet_protocol.packet_length", "Packet Length", base.DEC, nil, 0xff)
f.packet_length2 = ProtoField.uint16("encapsulation_packet_protocol.packet_length", "Packet Length", base.DEC, nil,
    0xffff)
f.packet_length3 = ProtoField.uint32("encapsulation_packet_protocol.packet_length", "Packet Length", base.DEC, nil,
    0xffffffff)

---------------
-- Dissector --
---------------

function encapsulation_packet_protocol.dissector(buffer, pinfo, tree)
    local length_buffer = buffer:len()
    if length_buffer < 1 then return end
    pinfo.cols.protocol = encapsulation_packet_protocol.name
    pinfo.cols.info = "Data"

    -- add a tab SPP
    local tab = ByteArray.tvb(buffer:bytes(), "EPP")

    local epp_tree = tree:add(encapsulation_packet_protocol, buffer(), "Encapsulation Packer Protocol")

    local offset = 0

    local header_buffer = buffer:range(offset, EPP_MIN_SIZE):tvb()
    local header_tree = epp_tree:add(encapsulation_packet_protocol, header_buffer(), "Encapsulation Packet Header")

    header_tree:add(f.packet_version_number, buffer(offset, 1))
    header_tree:add(f.encapsulation_protocol_id, buffer(offset, 1))
    local epi = buffer(offset, 1):bitfield(3, 3)
    header_tree:add(f.length_of_length, buffer(offset, 1))
    local length_of_length = buffer(offset, 1):bitfield(6, 2)
    offset = offset + 1
    if length_of_length == 1 then
        header_tree:add(f.packet_length1, buffer(offset, 1))
        offset = offset + 1
    elseif length_of_length == 2 then
        header_tree:add(f.user_defined_field, buffer(offset, 1))
        header_tree:add(f.encapsulation_protocol_id_extension, buffer(offset, 1))
        offset = offset + 1
        header_tree:add(f.packet_length2, buffer(offset, 2))
        offset = offset + 2
    elseif length_of_length == 3 then
        header_tree:add(f.user_defined_field, buffer(offset, 1))
        header_tree:add(f.encapsulation_protocol_id_extension, buffer(offset, 1))
        offset = offset + 1
        header_tree:add(f.ccsds_defined_field, buffer(offset, 2))
        offset = offset + 2
        header_tree:add(f.packet_length3, buffer(offset, 4))
        offset = offset + 4
    end

    local data_buffer = buffer:range(offset, buffer:len() - offset):tvb()
    -- Only Idle Data
    if epi == 0 then
        local oid_data_tree = epp_tree:add(encapsulation_packet_protocol, data_buffer(), "IDLE Data")
        Dissector.get("data"):call(data_buffer, pinfo, oid_data_tree)
    else
        local user_data_tree = epp_tree:add(encapsulation_packet_protocol, data_buffer(), "User Data")
        Dissector.get("data"):call(data_buffer, pinfo, user_data_tree)
    end
end

local udp_port = DissectorTable.get("udp.port")
local tcp_port = DissectorTable.get("tcp.port")
udp_port:add(7000, encapsulation_packet_protocol)
tcp_port:add(7000, encapsulation_packet_protocol)
