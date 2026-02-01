# 간단한 BLDC 6-스텝 제어 펌웨어 프로젝트

### 1.현재 stm32g431 nucleo 보드와 x-nucleo-ihm08m1 모터 드라이버 확장 보드를 기반으로 펌웨어 작성
#### &emsp;&emsp;1) 펌웨어 구조는 최대한 mcu나 driver 의존부분을 추상화하고 제어 부분과 분리 시도
#### &emsp;&emsp;2) uniPolar 방식의 6-스텝 제어
#### &emsp;&emsp;3) Hall sensor 기반 rpm 측정 및 필터링
#### &emsp;&emsp;4) CLI를 통해 rpm제어 
#### &emsp;&emsp;5) 볼륨저항을 통해 rpm 제어
#### &emsp;&emsp;6) 현재 pwm, timer, gpio 는 어느정도 추상화
<br><br><br>
### 2.command line interface (CLI)와 로깅 전용 uart 지원
#### &emsp;&emsp;1) stm32g431 nucleo 기준으로 usb VCP와 uart3 tx(PB9) 사용
#### &emsp;&emsp;2) 로깅 전용 uart는 better serial plotter 포멧에 맞춤
<br><br><br>
### 3. 추후 진행 예정
#### &emsp;&emsp;1) uart, adc 추상화
#### &emsp;&emsp;2) center-aligned PWM 
#### &emsp;&emsp;3) PWM On center 지점에서 ADC trigger (전류측정 목적)
#### &emsp;&emsp;4) biPolar 방식 구현
<br><br><br>
### 4. CLI 사용법
#### &emsp;&emsp;1) 테라텀과 같은 시리얼 프로그램 실행
#### &emsp;&emsp;2) 엔터를 쳐서 BLDC@cli$ 확인
#### &emsp;&emsp;3) motor help 를 타이핑하면 제어에 필요한 커맨드 표시
#### &emsp;&emsp;4) 예시 rpm 500, CCW -> BLDC@cli$ motor rpm 1 500  
### 
<br><br><br>
#### &emsp;&emsp;&emsp;&emsp;RPM 측정 (Better Serial Plotter)
![image](https://github.com/user-attachments/assets/f76bd441-50f6-43b9-85ea-0b1a151145c4) 
<br><br><br>
#### &emsp;&emsp;&emsp;&emsp;CLI 사용
| ![image](https://github.com/user-attachments/assets/5bacc6b2-d861-4573-a8e9-4bff1a33b0da) 
