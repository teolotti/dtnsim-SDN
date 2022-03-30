import subprocess
import os

def execute():
	python_bin = "venv/bin/python3.6"

	script_file = "run_cgrbruf.py"
	
	subprocess.Popen([python_bin, script_file])
	
	return 0

