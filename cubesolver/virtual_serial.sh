socat -d -d pty,link=/tmp/vserial1,raw,echo=0 pty,link=/tmp/vserial2,raw,echo=0 &

socat -d -d pty,link=/tmp/vserial3,raw,echo=0 pty,link=/tmp/vserial4,raw,echo=0 &

python3 control.py &

#python3 face_color.py &
