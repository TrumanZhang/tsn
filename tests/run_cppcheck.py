import sys
import os
import subprocess

for env_var in ["NESTING", "OMNETPP", "INET"]:
    if env_var not in os.environ:
        print("Environment variale ${} not defined!".format(env_var), file=sys.stderr)
        sys.exit(1)

# Read environment variables
omnetpp = os.environ["OMNETPP"]
nesting = os.environ["NESTING"]
inet = os.environ["INET"]

# Path to cppcheck report that shall be generated
cppcheck_report = "cppcheck_report.json" if len(sys.argv) <= 1 else sys.argv[1]

exec_cppcheck_cmd = [
    'cppcheck',
    '--std=c++11',
    '--suppress=*:*_m.cc',
    '--enable=all',
    '-I {}/include'.format(omnetpp),
    '-I {}/src'.format(inet),
    '-I {}/src'.format(nesting),
    '{}/src'.format(nesting),
    '--template=\'{"type": "issue","description": "{message}","check_name": "{id}","location": {"path": "{file}","lines": {"begin": "{line}"}}, "severity": "{severity}", "fingerprint": ""}\''
]

for option in exec_cppcheck_cmd:
    print(option, end=' ')
subprocess.call(exec_cppcheck_cmd)