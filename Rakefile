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

desc 'Copy all src files to ~/awesome_controller'
task :copy_src do
  require 'fileutils'

  dest_dir = '~/awesome_controller'

  puts `mkdir -p #{dest_dir}`

  [['awesome_controller'], ['libraries', 'wiiuse_arduino', 'src']].each do |file_path_pieces|
    Dir[File.join(file_path_pieces + ['*'])]
      .grep(/\.(ino|h|c|)$/)
      .map{ |f| `cp \`pwd\`/#{f} #{dest_dir}` }
  end
end