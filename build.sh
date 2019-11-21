#!/bin/bash

cmake -G "Unix Makefiles" -B cmake-build-release
cmake --build cmake-build-release --target hex
