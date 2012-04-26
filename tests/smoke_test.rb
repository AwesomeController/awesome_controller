# Copyright 2011-2012 Awesome Controller
#
# This file is part of Awesome Controller.
#
# Awesome Controller is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Awesome Controller is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Awesome Controller.  If not, see <http://www.gnu.org/licenses/>.

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

