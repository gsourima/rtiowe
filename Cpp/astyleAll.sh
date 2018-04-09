#!/bin/sh

astyle --options=".astylerc" `pwd`/*.cpp
astyle --options=".astylerc" `pwd`/*.h