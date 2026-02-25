-- © Copyright European Space Agency, 2023
-- This file is subject to the terms and conditions defined in file 'LICENSE.md',
-- which is part of this source code package. No part of the package,
-- including this file, may be copied, modified, propagated, or distributed except
-- according to the terms contained in the file ‘LICENSE.md’.

local sdls_header = Proto("sdls_header", "Space Data Link Security header")

-- Preferences
sdls_header.prefs.spi_length = Pref.uint("SDLS SPI Length", 2, "Security Parameter Index size")
sdls_header.prefs.iv_length = Pref.uint("SDLS IV Length", 0, "Initialization Vector length")
sdls_header.prefs.sn_length = Pref.uint("SDLS SN Length", 0, "Sequence Number length")
sdls_header.prefs.pad_length = Pref.uint("SDLS Pad Length", 0, "Pad length")

local prefs = sdls_header.prefs
local config = prefs

-- Fields
local f = sdls_header.fields
f.spi = ProtoField.uint16("sdls.spi", "Security Parameter Index")
f.IV = ProtoField.bytes("sdls.iv", "Initialization Vector")
f.SN = ProtoField.bytes("sdls.sn", "Sequence Number")
f.pad = ProtoField.bytes("sdls.pad", "Pad")

function sdls_header.dissector(buffer, pinfo, tree)
    local subtree = tree:add(sdls_header, buffer(), "Space Data Link Security header")
    local offset = 0

    if SDLS_header_get_length(config) > 64 then
        subtree:add_expert_info(PI_MALFORMED, PI_ERROR, "Security header can be at most 64 bytes")
        return
    end
    subtree:add(f.spi, buffer(offset, config.spi_length))
    local spi = buffer(offset, config.spi_length):uint()
    offset = offset + config.spi_length

    subtree:add(f.IV, buffer(offset, config.iv_length))
    offset = offset + config.iv_length

    subtree:add(f.SN, buffer(offset, config.sn_length))
    offset = offset + config.sn_length

    subtree:add(f.pad, buffer(offset, config.pad_length))
    offset = offset + config.pad_length

    if spi == 0 or spi == 65535 then
        -- Reserved, used for SDLS Extended Procedures
    end
end

function sdls_header.prefs_changed()
    SDLS_header_set_config(nil)
end

function SDLS_header_get_length(config)
    if config ~= nil then
        return config.spi_length + config.iv_length + config.sn_length + config.pad_length
    end
    return prefs.spi_length + prefs.iv_length + prefs.sn_length + prefs.pad_length
end

function SDLS_header_set_config(c)
    if c ~= nil then
        config = c
    else
        config = prefs
    end
end
