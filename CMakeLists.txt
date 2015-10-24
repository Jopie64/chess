cmake_minimum_required (VERSION 2.6)
project (Chess)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (MSVC)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
elseif (CMAKE_COMPILER_IS_GNUCXX )
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif(MSVC)

add_definitions(-D_AFXDLL -DWINVER=0x600 -D_WIN32_WINNT=0x600 -DUNICODE -D_UNICODE)

set(CHESS_SOURCES_CPP
	"chessboard.cpp"
	"main.cpp"
	"tests.cpp"
	)

set(CHESS_SOURCES_H
	"chessboard.cpp"
	)

source_group("include" FILES ${CHESS_SOURCES_H})
source_group("src"     FILES ${CHESS_SOURCES_CPP})

add_executable(Chess
        ${CHESS_SOURCES_CPP}
        ${CHESS_SOURCES_H})

