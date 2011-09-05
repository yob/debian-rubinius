require 'benchmark'
require 'benchmark/ips'

Const = 1

Benchmark.ips do |x|
  x.report "const read", <<-CODE
    Const
  CODE
end
