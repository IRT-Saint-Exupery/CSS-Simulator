require 'openc3/conversions/conversion'
require 'utilities'

module OpenC3
  class OrbitNormal < Conversion
    def initialize(*args)
      super()
      @items = args
      @converted_type = :FLOAT
      @converted_bit_size = 64
      @converted_array_size = 3
    end

    def call(value, packet, buffer)
      p = [packet.read(@items[0]), packet.read(@items[1]), packet.read(@items[2])]
      v = [packet.read(@items[3]), packet.read(@items[4]), packet.read(@items[5])]

      pxv = Utilities::cross(p,v)
      orbnorm = Utilities::sxv(1/Utilities::norm(pxv), pxv)

      return orbnorm
    end

  end
end