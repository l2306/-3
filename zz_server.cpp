
#include "zz_server.h"

z_Server *z_Server::instance = NULL;

static void hdlCtrlC(int signum)
{
	debug_output("ctrl-c signal .....\n");
	z_Server::delself();
}

z_Server::z_Server()
{
	instance = this;
}

z_Server::~z_Server()
{
}


bool z_Server::init()
{
	debug_output("%s\n" , __PRETTY_FUNCTION__);
	
	struct sigaction act;
	act.sa_handler = hdlCtrlC;   
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGINT , &act , NULL);		//定义ctrl-c信号
}


