#!/bin/bash

# 저장 경로
SAVE_DIR=/home/kim001104/bin/7th_Assignment/images
# 날짜 기반 prefix (파일명 구분용)
NOW=$(date +"%Y%m%d_%H%M%S")

# 2초 간격으로 30장 촬영 (총 약 1분)
for i in $(seq -w 0 29)
do
    libcamera-still -n --width 1920 --height 1080 -o "$SAVE_DIR/${NOW}_$i.jpg"
    sleep 3
done

# sleep을 3초로 해서 15초 영상을 만들기 위해서는 22분 30초의 촬영 시간 필요(450초 * 3초 = 1350초 == 22분 30초)
# cron을 시간을 정해서 사용하기 위해서는 14시 10분부터 25분까지 실행(10-25 14 * * * /home/kim001104/bin/7th_Assignment/capture.sh)
# crontab -e를 통해서 capture.sh를 등록
# crontab -l을 통해서 등록된 cron 확인
# ffmpeg -r 30 -pattern_type glob -i "images/*.jpg" -vf scale=1920:1080 -c:v libx264 -preset ultrafast -pix_fmt yuv420p output.mp4통해서 동영상 렌더링
