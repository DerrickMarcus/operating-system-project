import os
import shutil
import subprocess
from pathlib import Path

script_dir = Path(__file__).resolve().parent
project_root = script_dir.parent

os.chdir(project_root)

build_dir = project_root / "build"
if build_dir.exists():
    shutil.rmtree(build_dir)

build_dir.mkdir(parents=True, exist_ok=True)

os.chdir(build_dir)

subprocess.run(["cmake", ".."])
subprocess.run(["make", "-j" + str(os.cpu_count())])

bin_dir = project_root / "bin"
os.chdir(bin_dir)

if os.name == "nt":  # Windows
    subprocess.run(["bank_service.exe"])
else:  # Unix-like
    subprocess.run(["./bank_service"])
