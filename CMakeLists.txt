cmake_minimum_required(VERSION 3.28)

project(sender_and_receiver C)

include_directories(src)

add_executable(sender
    src/shared.c
    src/sender.c)

add_executable(receiver
    src/shared.c
    src/receiver.c)