require 'openc3/conversions/conversion'

module OpenC3
  class BetaAngle < Conversion
    def initialize(*args)
      super()
      @items = args
      @converted_type = :FLOAT
      @converted_bit_size = 64
      @converted_array_size = 2
    end

    def call(value, packet, buffer)
      p   = [packet.read(@items[0]), packet.read(@items[1]), packet.read(@items[2])]
      v   = [packet.read(@items[3]), packet.read(@items[4]), packet.read(@items[5])]
      svb = [packet.read(@items[6]), packet.read(@items[7]), packet.read(@items[8])]
      qbn = [packet.read(@items[9]), packet.read(@items[10]), packet.read(@items[11]), packet.read(@items[12])]

      pxv = Utilities::cross(p,v)
      orbnorm = Utilities::sxv(1/Utilities::norm(pxv), pxv)
      cbn = Utilities::Q2C(qbn)
      svn = Utilities::MTxV(cbn,svb)

      b = []
      b[0] = 90.0 - 180.0*Math.acos(Utilities::dot(orbnorm,svn))/Math::PI # beta angle
      b[1] = 90.0 + 180.0*Math.acos(Utilities::dot(orbnorm,svn))/Math::PI # beta angle supplement

      return b
    end

  end
end