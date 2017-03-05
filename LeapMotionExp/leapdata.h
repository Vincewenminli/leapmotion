#pragma once

#include <iostream>
#include <cstring>
#include "Leap.h"

using namespace Leap;

class SampleListener : public Listener
{
public:
	virtual void onInit(const Controller&);
	virtual void onConnect(const Controller&);
	virtual void onDisconnect(const Controller&);
	virtual void onFrame(const Controller&);
private:
};