#!/usr/bin/ruby -Ku
# rakefile.rb
# -*- coding: utf-8 -*-
require "fileutils"
GDM_JAR = "/usr/local/galatea-dialog/jar/gdm.jar"
SRM_BIN = "/usr/local/galatea-engine/SRM/bin"

ENCODING = "EUC-JP" # "UTF-8"
 
task :version do
  sh "java -cp #{GDM_JAR} galatea.io.julius.GrammarMaker"
end
 
task :default do
  opt = "-d #{SRM_BIN} -e #{ENCODING}"
  gram = "_gram.xml"
  dest = "gram/gram"
  #
  FileUtils.mkdir_p("gram")
 
  File.open(gram, "w") do |f|
    f.puts <<-EOF
<?xml version="1.0" encoding="UTF-8"?>
<grammar root="#all">
<rule id="all">
<one-of>
EOF
    File.open("gram.txt", 'r:UTF-8').each do |s|
      s.chomp!
      a, b = s.split(/ /)
      if a != nil
        f.puts "<item> <token sym=\"#{a}\">#{b}</token> </item>"
      end
    end
    f.puts <<-EOF
</one-of>
</rule>
</grammar>
EOF
  end
 
  sh "java -cp #{GDM_JAR} galatea.io.julius.GrammarMaker #{opt} #{Dir.pwd}/#{gram} #{Dir.pwd}/#{dest}"
end
