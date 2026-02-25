require 'openc3/conversions/conversion'

#
# Example usage:
#
#  APPEND_ITEM    RNG_AND_CNT         16 UINT            "Range"
#  ITEM           RNG                 0 0 DERIVED        "Config Register State"
#    READ_CONVERSION bit_field_conversion.rb 'RNG_AND_CNT' 15
#    STATE LO 0
#    STATE HI 1
#  ITEM           RNG_SELECT          0 0 DERIVED        "Range Selection"
#    READ_CONVERSION bit_field_mask.rb 'RNG_AND_CNT' 14 13
#    STATE "Autorange"                  0x00
#    STATE "Manual Low"                 0x02
#    STATE "Manual Hi"                  0x03
#

module OpenC3
  class BitFieldConversion < Conversion
    def initialize(packet_field, bit_idx)
      super()
      @packet_field = packet_field
      @bit_idx = bit_idx.to_i
    end
    def call(value, packet, buffer)
      mask = 1 << @bit_idx
      return (packet.read(@packet_field) & mask) >> @bit_idx
    end
  end
end
