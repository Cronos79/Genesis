#pragma once
#include "GEngineBase.h"
#include "GEngineWindow.h"
#include "GEngineContext.h"

int main(int argc, char** argv);

class GEngineApp
{
public:
	GEngineApp(int32_t width, int32_t height, std::string title);
	~GEngineApp();

	int Run();

private:
	GEngineContext context;
};

// To be defined in CLIENT
GEngineApp* CreateGEngineApp();

