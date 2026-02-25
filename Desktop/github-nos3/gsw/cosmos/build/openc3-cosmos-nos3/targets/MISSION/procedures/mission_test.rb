require 'cosmos'
require 'cosmos/script'

class COM < Cosmos::Test
    def setup
        # Unused constructor
    end

    def test_debug
        start("com/debug.rb")
    end

    def test_radio
        start("com/radio.rb")
    end

    def teardown
        # Unused constructor
    end
end

class LPT < Cosmos::Test
    # Limited Performance Test

    def setup
        # Unused constructor
    end

    def test_cfdp_up
        start("tests/cfdp_up.rb")
    end

    def test_cfdp_down
        start("tests/cfdp_down.rb")
    end

    def teardown
        # Unused constructor
    end
end

class Mission_Test < Cosmos::TestSuite
    def initialize
        super()
        add_test('COM')
        add_test('LPT')
    end

    def setup
        cmd("CFS LC_SET_LC_STATE with NEWLCSTATE LC_STATE_ENABLED")
    end
    
    def teardown
        cmd("CFS LC_SET_LC_STATE with NEWLCSTATE LC_STATE_DISABLED")
    end
end
