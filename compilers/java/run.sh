gcc judge.c ./cjson/cJSON.c -o judge.exe 
# ./judge.exe  code.c 0 2 1000 100000000 in expect res.json;
# ./judge.exe code.c 1 2 1000 100000000 in spec.py res.json;
export special=0
export timelimit=1000
export memorylimit=262144
export casenum=1
./judge.exe