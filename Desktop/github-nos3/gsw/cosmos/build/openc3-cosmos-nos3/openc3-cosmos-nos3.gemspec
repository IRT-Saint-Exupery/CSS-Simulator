# encoding: ascii-8bit

# Create the overall gemspec
spec = Gem::Specification.new do |s|
  s.name = 'openc3-cosmos-nos3'
  s.summary = 'OpenC3 openc3-cosmos-nos3 plugin'
  s.description = <<-EOF
    openc3-cosmos-nos3 plugin for deployment to OpenC3
  EOF
  s.license = 'MIT'
  s.authors = ['Anonymous']
  s.email = ['name@domain.com']
  s.homepage = 'https://github.com/OpenC3/openc3'
  s.platform = Gem::Platform::RUBY

  if ENV['VERSION']
    s.version = ENV['VERSION'].dup
  else
    time = Time.now.strftime("%Y%m%d%H%M%S")
    s.version = '0.0.0' + ".#{time}"
  end
  s.files = Dir.glob("{targets,lib,tools,microservices}/**/*") + %w(Rakefile README.md LICENSE.txt plugin.txt)
end
