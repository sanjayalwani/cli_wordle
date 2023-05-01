default:
	g++ main.cc -lsqlite3 -o main -O3

setup:
	python3 scripts/loadWords.py
