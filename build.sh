#!/bin/bash

cmake -G "Unix Makefiles" -H. -Bcmake-build-release -DCMAKE_BUILD_TYPE=Release
cmake --build cmake-build-release --target hex
