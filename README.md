# ![icon](img/icon.png) mini_tetris

***Announcement***
> 우리 팀은 보드를 가지고 있지 않아 더 이상의 업데이트가 불가능합니다. 테트로미노 90도 회전 시 가끔 테트로미노끼리 한 칸 겹치는 버그, 회전할 수 없는 상황임에도 회전이 되는 버그가 있는데 마감기한 내에 고치지 못했습니다. 이 점을 유의하고 코드를 참고하십시오.

## Developers
* [YoungHun Go](https://github.com/younghoongo)
* [Jubin Park](https://github.com/jubin-park)
  
## Demo Video

[![thumbnail](img/video_thumbnail.png)](https://youtu.be/--UvrgKFGB0)

## Interfaces

![control](img/game_control.png)

### **Main Screen**
![](img/dot_matrix.png)

메인화면을 담당하는 7 by 10 크기의 스크린

### **Score Board**
![](img/seven_segment.png)

4자리 점수판 7-Segment

### **Frame Rate LED**
![](img/led.png)

테트로미노 블록이 떨어지는 속도를 표시

### **LCD Message Display**
![](img/lcd_text_display.png)

2 by 16 크기의 아스키 문자를 출력하는 LCD 디스플레이
  
## Game Control

![](img/direction.png)

* **Cancel** 키: 꼭지점 부분에 위치한 취소 버튼 4개
* **Direction** 키: 블록을 좌우로 움직이거나, 아래로 즉시 내림
* **OK/Rotate Clockwisely** 키: 확인 버튼 또는 블록 시계방향 90 degree 회전시키는 키

## Scene

### 인트로 씬 (SCENE_INTRO)

![intro](img/scene_intro.gif)

* 테트리스 게임의 타이틀과 제작자 명단 출력
* OK 버튼을 눌러서 게임을 시작

### 게임 씬 (SCENE_GAME)

![](img/dot_matrix2.png)

* 방향키와 OK 버튼으로 블록을 조작
* 취소 버튼을 누르면 일시 정지 화면 전환

### 게임오버 씬 (SCENE_GAMEOVER)

![gameover](img/scene_gameover.gif)

* 게임오버 발생 시 문구 출력
* OK 버튼을 눌러 인트로 씬 전환


### 일시정지 씬 (SCENE_PAUSE)

![pause](img/scene_pause.gif)

* 플레이 도중 취소 버튼을 누르면 일시정지 모드 실행
* OK 버튼을 누르면 인트로 씬 전환
* 취소 버튼을 누르면 게임 이어서 진행

### 리소스 작업 (32-bit to 8-bit Converter)

* Piskel 프로그램을 사용하여 도트 씬 애니메이션을 제작
* C언어 Header 파일로 Export 가능
* 그러나 4바이트 ARGB 포맷으로 이루어짐
* Black/White 포맷의 1 Row - 8 Bit 포맷으로 변환하는 python3 코드 *gen_dot10x7_headers.py* 작성 
`./res/*.h -> ./dot10x7/*.h` 로 변환

## Command

`make`

* 모든 디바이스 드라이버 ko 파일 빌드

`make clean`

* 모든 빌드파일 지움

`make app`

* app_game 실행파일 빌드

`./update_mod.sh`

* mknod, rmmod, insmod 관련 업데이트 명령어

`python3 gen_dot10x7_headers.py`

* 헤더 컨버팅 스크립트

`./app_game`

* 게임 실행

`Ctrl + C`

* 게임 종료