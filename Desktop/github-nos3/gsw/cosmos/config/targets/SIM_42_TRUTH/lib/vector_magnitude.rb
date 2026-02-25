require 'openc3/conversions/conversion'
require 'utilities'

module OpenC3
  class VectorMagnitude < Conversion
    def initialize(*args)
      super()
      @items = args
      @converted_type = :FLOAT
      @converted_bit_size = 64
    end

    def call(value, packet, buffer)
      u = []
      u[0] = packet.read(@items[0])
      u[1] = packet.read(@items[1])
      u[2] = packet.read(@items[2])
      return Utilities::norm(u)
    end

  end
end