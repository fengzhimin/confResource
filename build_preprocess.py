import os
import sys

def extract_json(compile_commands_json_path):
	fout = open('./build_preprocess.sh', 'w')
	fout.write('#!/bin/bash\n')
	fout.write('#this is automatically produced by "build_preprocess.py compile_commands.json", DO NOT change!\n\n\n ')
	JSON = open(compile_commands_json_path)
	Lines = JSON.readlines()
	outcommand = ''
	languageType = 'gcc'
	for line in Lines:
		line = line[:-1]
		line = line.replace(',', '')
		subflds = line.split(':')
		command = subflds[0].strip()
		command = command.replace('\"','')
		# handle mysql C and C++
		command = command.replace('.cc.o','.E.cc')
		command = command.replace('.c.o','.E.c')
		command = command.replace('.cpp.o','.E.cpp')
		# handle redis C
		if languageType == 'gcc':
			command = command.replace('.o', '.E.c')
		else:
			command = command.replace('.o', '.E.cc')
		if command == 'c++':
			outcommand = 'g++ '
			languageType = 'g++'
		elif command == 'cc':
			outcommand = 'gcc '
			languageType = 'gcc'
		elif command == '-c':
			outcommand = outcommand + '-E -P '
		elif command  == 'file':
			file = subflds[1]
			file = file.replace('\"', '')
			file = file.replace(' ', '')
			lastfile = ''
			if file != lastfile:
				fout.write('cd ' + directory + '\n')
				if outcommand.find('.E.') == -1:
					outcommand = outcommand + '-o ' + file.replace('.', '.E.')
				fout.write(outcommand + '\n')
				lastfile = file
		elif command == 'directory':
			directory = subflds[1]
			directory = directory.replace('\"', '')
			directory = directory.replace(' ', '')
		elif command != ']':
			#if command.find('=') == -1:
			outcommand = outcommand + command + ' '
	fout.write('\n')
	fout.close()
	os.system('sh ./build_preprocess.sh')

if __name__=="__main__":
	print "start"
	if len(sys.argv) == 2:
		if os.path.exists(sys.argv[1]):
			extract_json(sys.argv[1])
		else:
			print sys.argv[1] + "not exist!"
	else:
		print "please input compile commands json path"
