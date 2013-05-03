BIN_DIR = "./bin"
SRC_DIR = "./src"
TEST_DIR = "./test"

MAIN_BIN = "#{BIN_DIR}/mysh"
TEST_BIN = "#{BIN_DIR}/test_mysh.so"
MAIN_SRC = FileList["#{SRC_DIR}/*.c"]
MAIN_HDR = FileList["#{SRC_DIR}/**/*.h"]
TEST_SRC = FileList["#{TEST_DIR}/*.c"]

task "default" => MAIN_BIN

directory BIN_DIR

file MAIN_BIN => [BIN_DIR, *(MAIN_HDR + MAIN_SRC)] do
  sh "gcc -Wall -Wextra -g -o #{MAIN_BIN} #{MAIN_SRC} -I#{SRC_DIR}"
end

file TEST_BIN => [BIN_DIR, *(MAIN_HDR + MAIN_SRC + TEST_SRC)] do
  sh "gcc -Wall -Wextra -g -shared -o #{TEST_BIN} " +
     "#{MAIN_SRC} #{TEST_SRC} -I#{SRC_DIR} " +
     `pkg-config --cflags cutter`
end

task "run" => MAIN_BIN do
  sh "#{MAIN_BIN}"
end

task "test" => TEST_BIN do
  sh "cutter #{BIN_DIR}"
end

task "cl" do
  main_count = countline MAIN_SRC
  test_count = countline TEST_SRC
  puts "Main: #{main_count}"
  puts "Test: #{test_count}"
  puts "Total: #{main_count + test_count}"
end

def countline src
  src.inject(0) do |count, name|
    count + IO.readlines(name).size
  end
end
