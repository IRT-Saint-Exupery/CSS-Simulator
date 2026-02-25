-- © Copyright European Space Agency, 2023
-- This file is subject to the terms and conditions defined in file 'LICENCE.md',
-- which is part of this source code package. No part of the package,
-- including this file, may be copied, modified, propagated, or distributed except
-- according to the terms contained in the file ‘LICENCE.md’.

local tc_cltu_protocol = Proto("TC_CLTU", "TC CLTU")

local f = tc_cltu_protocol.fields
local enums = {}

-- Physical Channel Name
PHYSICAL_CHANNEL_NAME = "TC"

-- Block Size
TC_HEADER_LENGTH = 5
SEGMENT_HEADER_LENGTH = 1
TRAILOR_LENGTH2 = 2
SPP_HEADER_LENGTH = 6

RANDOMIZE_SEQUENCE = ByteArray.new("ff399e5a68e906f56c892fa1315e08c052a8bbae4ec2c7ed66dc38d4f886503d")

---------------
-- Dissector --
---------------

function tc_cltu_protocol.dissector(buffer, pinfo, tree)
    local length = buffer:len()
    print(length)
    if length == 0 then return end
    pinfo.cols.protocol = tc_cltu_protocol.name

    -- add a tab CLTU
    local tab = ByteArray.tvb(buffer:bytes(), "TC CLTU")

    local tc_tree = tree:add(tc_cltu_protocol, buffer(), "TC SYNCHRONIZATION AND CHANNEL CODING")

    local offset = 0

    -- Search for Start Sequence
    local start_sequence = buffer(offset, 2):uint()
    if start_sequence ~= 60304 then
        error("Start Sequence not matching")
    end
    offset = offset + 2

    -- BCH decoding
    -- consider that the network is perfect
    local buffer_tc_frame = ByteArray.new("", true)
    local randomizer_sequence_index = 0
    local transfer_frame

    while (buffer:len() > offset) do
        local bch_codeword

        -- Search for Tail Sequence
        local tail_sequence = buffer(offset, 8):uint64()
        if UInt64(tail_sequence) == UInt64(0xC5C5C579, 0xC5C5C5C5) then
            break
        end


        transfer_frame = buffer(offset, 7)
        offset = offset + 7

        -- CHECK parity bytes
        -- SKIPPED
        local fec = buffer(offset, 1)
        offset = offset + 1

        buffer_tc_frame:append(transfer_frame:bytes())
    end

    if tc_cltu_protocol.prefs.randomization then
        -- DeRandomization
        local tc_frame_raw = buffer_tc_frame
        local buffer_transfer_frame = ByteArray.new()
        local byte_xored

        for i = 0, buffer_tc_frame:len() - 1 do
            local byte = buffer_tc_frame:get_index(i)

            if byte == 85 then
                byte_xored = 85
            else
                local byte_random = RANDOMIZE_SEQUENCE:get_index(i % 254)
                byte_xored = bit.bxor(byte, byte_random)
            end
            buffer_transfer_frame:append(ByteArray.new(string.format("%02X", byte_xored)))
        end

        local transfer_frame_tvb = ByteArray.tvb(buffer_transfer_frame, "TC FRAME")
        Dissector.get("tc_frame"):call(transfer_frame_tvb, pinfo, tree)
    else
        local tranfer_frame_tvb = ByteArray.tvb(buffer_tc_frame, "TC FRAME")
        Dissector.get("tc_frame"):call(tranfer_frame_tvb, pinfo, tree)
    end
end

local udp_port = DissectorTable.get("udp.port")
local tcp_port = DissectorTable.get("tcp.port")
udp_port:add(30002, tc_cltu_protocol)
tcp_port:add(30020, tc_cltu_protocol)

-- Register preferences
tc_cltu_protocol.prefs.randomization = Pref.bool("Randomization", false, "Whether we enable randomization or not")
