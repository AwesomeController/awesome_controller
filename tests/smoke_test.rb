require_relative 'test_helper'

require 'ffi'

module WiiUse
  extend FFI::Library
  ffi_lib "libraries/wiiuse_arduino/build/src/libwiiuse.dylib"

  attach_function :wiiuse_init, [:int], :pointer

  class Wiimote < FFI::Struct
    layout  :unid,  :int,
            :state, :int,
            :flags, :int
  end
end

describe :smoke do
  before do
    result = WiiUse.wiiuse_init 3
    @wiimote = WiiUse::Wiimote.new result
  end

  it "has a unid" do
    assert @wiimote[:unid]
  end

  it "has a state" do
    assert @wiimote[:state]
  end

  it "has flags" do
    assert @wiimote[:flags]
  end
end

