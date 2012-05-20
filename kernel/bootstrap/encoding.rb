# -*- encoding: us-ascii -*-

# This stupidity is necessary to counter the stupidity of checking
# 1.9 syntax support by checking if Encoding is defined. Who said
# two stupids don't make a smart.
Encoding = Rubinius::EncodingClass::Encoding

class Encoding
  attr_reader :name
  attr_reader :dummy

  alias_method :to_s, :name
  alias_method :dummy?, :dummy

  def self.compatible?(a, b)
    Rubinius.primitive :encoding_compatible_p
    raise PrimitiveFailure, "Encoding.compatible? primitive failed"
  end

  def replicate(name)
    Rubinius.primitive :encoding_replicate
    raise PrimitiveFailure, "Encoding#replicate primitive failed"
  end

  def ascii_compatible?
    Rubinius.primitive :encoding_ascii_compatible_p
    raise PrimitiveFailure, "Encoding#ascii_compatible? primitive failed"
  end
end
