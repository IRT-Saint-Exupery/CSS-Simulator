require 'openc3/conversions/conversion'

module OpenC3
  class XzRamAngle < Conversion
    def initialize(*args)
      super()
      @items = args
      @converted_type = :FLOAT
      @converted_bit_size = 64
    end

    def call(value, packet, buffer)
      v   = [packet.read(@items[0]), packet.read(@items[1]), packet.read(@items[2])]
      qbn = [packet.read(@items[3]), packet.read(@items[4]), packet.read(@items[5]), packet.read(@items[6])]

      cbn = Utilities::Q2C(qbn)

      vb = Utilities::MxV(cbn, Utilities::sxv(1.0/Utilities::norm(v),v)) # Velocity unit vector in body frame

      ang = 180.0*Math.atan2(vb[1], vb[0])/Math::PI # atan of y/x
      if ang < 0.0
        ang = -ang # keep in 0 to 180 degree range to better compare with x-ram angle
      end

      return ang
    end

  end
end