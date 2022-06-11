cmake_minimum_required(VERSION 3.16)
project(wet2)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(wet2
        Company.cpp
        Company.h
        CompanyUnion.h
        EarnerRankTree.cpp
        EarnerRankTree.h
        Employee.cpp
        Employee.h
        EmployeeHash.cpp
        EmployeeHash.h
        EmployeeManager.cpp
        EmployeeManager.h
        library2.cpp
        library2.h
        testMain.cpp)
