desc 'Testing testing 123'
task :test do
  puts `mkdir -p libraries/wiiuse_arduino/build`

  Dir.chdir 'libraries/wiiuse_arduino/build'

  system 'ccmake .. -DBUILD_EXAMPLE_SDL=NO'
  puts `make wiiuse`
end

task :default => :test