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

task :proggram do
  opt = "-d #{SRM_BIN} -e #{ENCODING}"
  gram = "_proggram.xml"
  dest = "proggram/proggram"
  #
  items = []
  File.open("gram.txt", 'r:UTF-8').each do |s|
    s.chomp!
    a, b = s.split(/ /)
    if a != nil
      items << [a, b]
    end
  end

  items2 = {}
  items.each do |s|
    array = s[0].split(//)
    1.upto(array.size) do |size|
      yomi = ''
      0.upto(size - 1) do |i|
        yomi += array[i]
      end
      if items2[yomi] 
        items2[yomi] += ',' + s[1]
      else
        items2[yomi] = s[1]
      end
    end
  end
  items3 = []
  count = 1
  items2.keys.sort.each do |i|
    items3 << [ i, "o#{count}", items2[i] ]
    count += 1
  end
  items = items3
  #
  FileUtils.mkdir_p("proggram")
  #
  File.open("proggram/proggram.sym2word", 'w:UTF-8') do |file|
    items.each do |s|
      file.puts "#{s[1]} #{s[0]} #{s[2]}"
    end
  end
  #
  File.open(gram, "w") do |f|
    f.puts <<-EOF
<?xml version="1.0" encoding="UTF-8"?>
<grammar root="#all">
<rule id="all">
<one-of>
EOF
    items.each do |s|
      f.puts "<item> <token sym=\"#{s[0]}\">#{s[1]}</token> </item>"
    end
    f.puts <<-EOF
</one-of>
</rule>
</grammar>
EOF
  end
  sh "java -cp #{GDM_JAR} galatea.io.julius.GrammarMaker #{opt} #{Dir.pwd}/#{gram} #{Dir.pwd}/#{dest}"
end

