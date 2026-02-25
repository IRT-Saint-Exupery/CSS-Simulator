require 'openc3/conversions/conversion'
require 'utilities'

module OpenC3
  class Svn < Conversion
    def initialize(*args)
      super()
      @items = args
      @converted_type = :FLOAT
      @converted_bit_size = 64
      @converted_array_size = 3
    end

    def call(value, packet, buffer)
      svb = [packet.read(@items[0]), packet.read(@items[1]), packet.read(@items[2])]
      qbn = [packet.read(@items[3]), packet.read(@items[4]), packet.read(@items[5]), packet.read(@items[6])]
      cbn = Utilities::Q2C(qbn)
      svn = Utilities::MTxV(cbn,svb)
      return svn
    end

  end
end