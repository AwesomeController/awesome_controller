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