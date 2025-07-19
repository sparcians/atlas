import os, sys, re, subprocess

if len(sys.argv) > 1:
    os.chdir(os.path.abspath(sys.argv[1]))

result = subprocess.run(['git', 'status', '--porcelain', '|', 'grep "M "'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
if result.returncode == 0 and result.stdout:
    raise RuntimeError('Cannot have uncommitted changes in this directory: {}'.format(os.getcwd()))

ext = os.path.basename(os.getcwd())
rv_insts_cpp = 'Rv{}Insts.cpp'.format(ext)
pattern = r"ActionGroup\* Rv{}Insts::".format(ext)

headers = set()
headers.add('#include "core/ActionGroup.hpp"\n')
headers.add('#include "include/ActionTags.hpp"\n')
for filename in os.listdir(os.getcwd()):
    if filename.endswith(".cpp") and filename != rv_insts_cpp:
        with open(filename, 'r') as fin:
            for line in fin.readlines():
                if line.find('#include') == 0:
                    headers.add(line)

lines = list(headers)
lines = sorted(lines, key=len, reverse=True)
lines.append('')

with open(rv_insts_cpp, 'r') as fin:
    transfer = False
    for line in fin.readlines():
        if line.find('namespace pegasus') == 0:
            transfer = True
        elif line.find('// namespace pegasus') != -1:
            break

        if transfer:
            lines.append(line)

lines = [''.join(lines)]
lines.append('')

for filename in os.listdir(os.getcwd()):
    if filename.endswith(".cpp") and filename != rv_insts_cpp:
        lines.append('')
        with open(filename, 'r') as fin:
            transfer = False
            for line in fin.readlines():
                if re.search(pattern, line):
                    transfer = True
                elif line.find('// namespace pegasus') != -1:
                    break

                if transfer:
                    lines.append(line.rstrip('\n'))

lines.append('')
lines.append('} // namespace pegasus')

with open(rv_insts_cpp, 'w') as fout:
    fout.write('\n'.join(lines))

for filename in os.listdir(os.getcwd()):
    if filename.endswith('.cpp') and filename != rv_insts_cpp:
        os.remove(filename)
