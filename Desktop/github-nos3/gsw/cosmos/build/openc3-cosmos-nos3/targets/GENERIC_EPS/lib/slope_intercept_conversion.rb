require 'openc3/conversions/conversion'

module OpenC3
  class SlopeInterceptConversion < Conversion
    def initialize(packet_field, slope, intercept=0)
      super()
      @converted_type = :FLOAT
      @converted_bit_size = 32
      @packet_field = packet_field
      @slope = slope.to_f
      @intercept = intercept.to_f
    end
    def call(value, packet, buffer)
      # Slope
      value = @slope * packet.read(@packet_field)
      # Intercept
      value = value + @intercept
      return value
    end
  end
end
