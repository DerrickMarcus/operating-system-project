#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import subprocess

script_dir = os.path.dirname(os.path.realpath(__file__))
project_root = os.path.dirname(script_dir)
os.chdir(project_root)

if os.path.exists("build"):
    subprocess.run(["rm", "-rf", "build"])

os.makedirs("build", exist_ok=True)
os.chdir("build")

subprocess.run(["cmake", ".."])
subprocess.run(["make", "-j" + str(os.cpu_count())])

os.chdir("../bin")
subprocess.run(["./bank_service"])
