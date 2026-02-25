require 'openc3/conversions/conversion'
require 'utilities'

module OpenC3
  class GeocentricLatitude < Conversion
    def initialize(*args)
      super()
      @items = args
      @converted_type = :FLOAT
      @converted_bit_size = 64
    end

    def call(value, packet, buffer)
      p = [packet.read(@items[0]), packet.read(@items[1]), packet.read(@items[2])]

      return Math.atan2(p[2], Math.sqrt(p[0]*p[0]+p[1]*p[1]))*180.0/Math::PI
    end

  end
end