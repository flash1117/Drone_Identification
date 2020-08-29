# Drone_Identification

## 개발 동기

> 드론은 사생활 침해, 건물 훼손, 보행자 부상등의 부작용을 낳을 수 있지만, 차량 번호판과 같이 운송수단을 식별할 수 있는 수단이 존재하지 않는다. 따라서 사건, 사고 예방에 도움이 되고자 개발에 착수하게 되었다.

## 관련 연구

드론의 식별에 관한 연구는 계속 진행중이지만, 크게 3가지로 나뉠 수 있다.

1. USIM을 이용한 방법
2. RFID 송/수신기를 이용한 방법
3. LED Pattern Image Processing을 이용한 방법

이 프로젝트에서는 3번 방법인 **LED Pattern Image Processing**방법을 사용하였다.

## LED Pattern Image Processing

> 색이 깜빡이는 패턴을 이용하여 식별 시스템을 구성하는 방법이다. 주기별로 처음과 끝에 서로 대칭인 반복신호를 통하여 주기를 판별한다.

## 채택이유

> 국내에는 CCTV, 블랙박스와 같은 카메라가 많은 곳에 설치되어 있어서 **LED Image Processing**방법을 사용하기 용이하며, 다른 식별 수단과는 다르게 수신기가 필요하지 않으므로 비용이 들지 않는다.

## 설계

![image](https://user-images.githubusercontent.com/33343785/91631923-440c6880-ea18-11ea-9b02-4a3d2dee1dbe.png)

차량 번호판은 위 사진과 같이 구성되며, 이를 접목하여 카메라의 유무, 상업적인 목적등의 용도에 따라 드론을 분류하여 LED 색상에 따라 각 드론이 데이터베이스를 구성하여 확인 할 수 있도록 하였다.

![image](https://user-images.githubusercontent.com/33343785/91631941-7b7b1500-ea18-11ea-9f6e-d0e0ea5ce7c2.png)

위 와 같이 특정 신호를 인식하였을 때 드론 식별 번호가 생성되게 되는데, 드론의 LED를 지속적으로 인식하기 위해서는 드론이라는 객체를 인식하고 추적할 필요성을 느꼈다.

![image](https://user-images.githubusercontent.com/33343785/91632002-c39a3780-ea18-11ea-88ce-2ae12e0a7acd.png)

1. 드론 추적으로 관심영역을 설정하여 객체를 끊임 없이 따라가는 것으로 시작한다.

2. 설정된 관심영역을 추적하면서 추출한 사진들로부터 정보를 얻기 위한 과정을 거친다.

3. 추출한 색상정보를 1과 0과 같은 숫자의 조합으로 드론 식별 번호를 생성한다.

4. 생성된 드론 식별 번호와 소유주로 이루어진 쿼리문을 통하여 데이터베이스에서 삽입/조회한다.

## TLD Algorithm

OpenCV에서 제공하는 MeanShift, CamShift는 드론을 추적하기엔 색상 기반 탐지라서 오차율이 너무 심했기에 사용할 수 없었다. 또한, 드론이 건물에 가려졌다가 다시 나타나는 등의 동작을 하여도 드론의 추적은 꾸준히 일어나야했다.

따라서 필요한 것은 드론이 멀어져서 크기나 형태 위치가 변해거나 잠깐 가려졌다 나타나도 꾸준히 추적할 수 있는 기능이 필요했으며 이를 위해 **TLD(Tracking-Learning-Detection)**알고리즘을 통해 추적기능을 구현하였다.

이름에서 알 수 있듯이 장기 추적 작업을 **단기추적, 학습, 탐지**라는 **세 가지 구성 요소**로 분할 하는데. 내부적으로 tracker 와 detector를 동시에 운용한다.
tracker는 동영상의 인접한 영상 프레임들 사이의 시간적, 공간적, 형태적 유사성을 추적 정보로 받아 들여 대상을 찾게 된다. 때문에 과거에 대상을 놓치면 연속성이 끊어져 다시 찾기 힘들다. 이를 보완하기 위해 detector를 사용하는데, **detector 는 미리 알고 있는 대상을 입력 영상에서 찾을 수 있는 방법으로 이미지 한 장만 주어져도 대상을 찾을 수 있다.**

TLD 알고리즘의 순서는 처음에 사용자가 추적할 ROI를 설정하면 tracker 와 detector 가 초기화 된다. 이 때 detector 의 경우 이미지 하나에 대해서 학습을 한다. 이후 입력 영상이 들어오면 **tracker 와 detector 가 동시에 대상을 찾는다.** 만약 **tracker 가 추적에 성공**했다면 tracker 가 찾은 이미지가 detector 의 학습 데이터로 활용되어서 **detector를 좀 더 견고하게 만들어 준다.** 또한 detector 가 찾은 영역 중 tracker와 결과가 일치 하지 않으면 잘못 인식 한 것으로 분류되어 그 또한 잘못 인식한 예로 학습을 하게 된다. 만약 tracker 가 추적에 실패했을 경우에는 detector가 성공 할 때 까지 대기하다가 detector가 성공하면 탐지한 위치로 tracker를 초기화 하고 추적을 이어 나간다.

![image](https://user-images.githubusercontent.com/33343785/91632169-09a3cb00-ea1a-11ea-8022-75fcf4d64252.png)

## Segmenation

설정된 ROI를 프레임 추출하여 색상을 뽑아내는 과정을 거치게 된다. 색을 추출 하는 과정에서 **RGB 색상모델**을 **HSV 색상모델**로 바꾸어서 추출 하게 되는데, RGB 색상모델은 조명에 매우 큰 영향을 받아서 빛과 그림자의 영향을 많이 받는 드론 촬영에는 적합하지 않기 때문이다.

![image](https://user-images.githubusercontent.com/33343785/91632187-2dffa780-ea1a-11ea-9ecf-dd1669c19c89.png)

## 조회

![image](https://user-images.githubusercontent.com/33343785/91632342-107f0d80-ea1b-11ea-8ae8-d9d7594119dc.png)

![image](https://user-images.githubusercontent.com/33343785/91632200-3eb01d80-ea1a-11ea-8b98-01b435a7751a.png)

## 개선해야 할 점

악의적인 사용자가 드론의 LED 패널을 수정할 경우 잘못된 사용자를 불러올 수가 있으므로 개선이 필요하다.
