#include "zzz_test.h"

MainPro *MainPro::instance = NULL;

int main()
{
Zz_logger::instance()<<"sss";
	MainPro *mp = MainPro::getInstance();
	mp->init();
	sleep(-1);
}

//
