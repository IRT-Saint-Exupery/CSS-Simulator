require 'openc3/conversions/conversion'

module OpenC3
  class ZNadirAngle < Conversion
    def initialize(*args)
      super()
      @items = args
      @converted_type = :FLOAT
      @converted_bit_size = 64
    end

    def call(value, packet, buffer)
      p   = [packet.read(@items[0]), packet.read(@items[1]), packet.read(@items[2])]
      qbn = [packet.read(@items[3]), packet.read(@items[4]), packet.read(@items[5]), packet.read(@items[6])]

      cbn = Utilities::Q2C(qbn)

      nb = Utilities::MxV(cbn, Utilities::sxv(-1.0/Utilities::norm(p),p)) # Nadir (-position) unit vector in body frame

      return 180.0*Math.acos(nb[2])/Math::PI # Dot product with +z, then acos
    end

  end
end