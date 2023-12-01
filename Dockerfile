from ubuntu

run apt-get update
run apt-get install -y g++
run apt-get install -y make
run apt-get install -y cmake

copy tuxx.hpp tuxx.hpp
copy examples examples
copy tests tests
copy CMakeLists.txt CMakeLists.txt
