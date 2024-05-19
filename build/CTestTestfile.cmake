# CMake generated Testfile for 
# Source directory: /home/borisz/BME/nagyhf/nagyhf
# Build directory: /home/borisz/BME/nagyhf/nagyhf/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(runTests "/home/borisz/BME/nagyhf/nagyhf/build/runTests")
set_tests_properties(runTests PROPERTIES  _BACKTRACE_TRIPLES "/home/borisz/BME/nagyhf/nagyhf/CMakeLists.txt;40;add_test;/home/borisz/BME/nagyhf/nagyhf/CMakeLists.txt;0;")
subdirs("external/googletest")
