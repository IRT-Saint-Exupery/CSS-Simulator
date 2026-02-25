require 'cosmos'
require 'cosmos/script'

class Cfdp_Automated_Test < Cosmos::Test
  # Verify cFS TO output is enabled
  # def test_to_enabled
  #   start("tests/verify_to_enabled.rb")
  # end

  # Test Class 1 uplink capabilities
  def test_uplink_class1
    start("tests/verify_uplink_class1.rb")
  end

  # Test Class 2 uplink capabilities
  def test_uplink_class2
    start("tests/verify_uplink_class2.rb")
  end

  # Test Class 1 downlink capabilities
  def test_downlink_class1
    # Test Class 2 downlink capabilities
    start("tests/verify_downlink_class1.rb")
  end

  # Test Class 2 downlink capabilities
  def test_downlink_class2
    start("tests/verify_downlink_class2.rb")
  end
end

class Cfdp_Test < Cosmos::TestSuite
  def initialize
    super()
    add_test('Cfdp_Automated_Test')
  end
end
