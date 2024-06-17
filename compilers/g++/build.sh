docker build -t judger:cpp .
# docker run -it -e special=0 \
#             -e timelimit=1000 \
#             -e memorylimit=100000000 \
#             -e casenum=2 \
#             -v /home/urlyy/桌面/judge_engine/judge/data:/app/data:shared \
#             judger:g++
