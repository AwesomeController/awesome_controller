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

require 'rake/testtask'

task :default => :test

desc "Run the test suite"
task :test => :build_wiiuse do
  Rake::TestTask.new do |t|
    t.test_files = FileList['tests/**/*_test.rb']
    t.verbose = true
  end
end

desc 'Testing testing 123'
task :build_wiiuse do
  puts `mkdir -p libraries/wiiuse_arduino/build`

  Dir.chdir 'libraries/wiiuse_arduino/build'

  unless File.exists? 'Makefile'
    system 'ccmake .. -DBUILD_EXAMPLE_SDL=NO'
  end
  puts `make wiiuse`

  Dir.chdir '../../..'
end
