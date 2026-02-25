require 'cosmos'
require 'cosmos/script'

class Aliveness_Test < Cosmos::Test
    def setup
        # Unused constructor
    end

    def test_udp
        start("tests/enable_udp.rb")
    end

    def teardown
        # Unused constructor
    end
end

class Cfs_Test < Cosmos::TestSuite
    def initialize
        super()
      add_test('Aliveness_Test')
    end
end
