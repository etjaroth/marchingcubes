#pragma once
#include "Config.h"
#include "RenderBox.h"

class ProcessingStage
{
	Texture2D& tex;
	RenderBox box;

public:
	ProcessingStage(Texture2D& tex);
};

