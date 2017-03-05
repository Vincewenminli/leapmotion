#include "stdafx.h"
#include "leapdata.h"
#include "LeapMotionExpDlg.h"
#include "LeapMotionExp.h"

float sensitivity = 2;
float px, py;
int isConnected;
int isDetected;
int CenterX = GetSystemMetrics(SM_CXSCREEN) / 2, CenterY = GetSystemMetrics(SM_CYSCREEN) / 2;


void SampleListener::onInit(const Controller& controller)
{
	isConnected = 0;
	isDetected = 0;
}

void SampleListener::onConnect(const Controller& controller)
{
	controller.enableGesture(Gesture::TYPE_CIRCLE);
	controller.enableGesture(Gesture::TYPE_KEY_TAP);
	controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
	controller.enableGesture(Gesture::TYPE_SWIPE);
	isConnected = 1;
	isDetected = 0;
}

void SampleListener::onDisconnect(const Controller& controller)
{
	isConnected = 0;
	isDetected = 0;
}

void SampleListener::onFrame(const Controller& controller)
{
	isConnected = 1;
	const Frame frame = controller.frame();
	if (!frame.hands().isEmpty())
	{
		HandList hands = frame.hands();
		const Hand hand = hands[0];
		//��ǰ�������߶ȵ�̽������
		Vector palmPosition = hand.palmPosition();
		float palmHeight = palmPosition.y;
		float detectionWidth = (float)(palmHeight*tan(75.0 / 180.0 * PI) * 2);
		float detectionHeight = 600;
		//������
		detectionWidth = detectionWidth / sensitivity;
		detectionHeight = detectionHeight / sensitivity;
		//ָ��λ��
		const FingerList fingers = hand.fingers();
		Finger finger = fingers[0];
		Vector position = finger.tipPosition();
		//����ӳ��
		double screenWidth = GetSystemMetrics(SM_CXSCREEN);
		double screenHeight = GetSystemMetrics(SM_CYSCREEN);
		//ӳ������
		px = position.x / detectionWidth * screenWidth + screenWidth / 2;
		py = screenHeight - position.y / detectionHeight * screenHeight;
		//״̬��ʶ 
		isDetected = 1;
	}
	else
	{
		isDetected = 0;
		px = CenterX, py = CenterY;
	}
}