require 'openc3/conversions/conversion'

module OpenC3
  class InSun < Conversion
    ER = 6378137.0

    def initialize(*args)
      super()
      @items = args
      @converted_type = :INT
      @converted_bit_size = 8
    end

    def call(value, packet, buffer)
      p   = [packet.read(@items[0]), packet.read(@items[1]), packet.read(@items[2])]
      svb = [packet.read(@items[3]), packet.read(@items[4]), packet.read(@items[5])]
      qbn = [packet.read(@items[6]), packet.read(@items[7]), packet.read(@items[8]), packet.read(@items[9])]

      cbn = Utilities::Q2C(qbn)
      svn = Utilities::MTxV(cbn,svb)
      rmag = Utilities::norm(p)
      r = Utilities::sxv(-1.0/rmag, p)

      theta = Math.acos(Utilities::dot(r, svn))

      in_sun = 50 # non-zero value for in sun... scale as you wish for use on graphs

      if ((theta < Math::PI/2) and (rmag * Math.sin(theta) < ER)) # simple calculation using sun vector and nominal earth radius... does not account for penumbra, etc.
        in_sun = 0 # zero value means not in sun
      end

      return in_sun
    end

  end
end