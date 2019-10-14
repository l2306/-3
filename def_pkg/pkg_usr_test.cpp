
#ifndef __pkg_usr1__
#define __pkg_usr1__

#include <iostream>
#include "pkg__def.h"
#include "pkg_ctl.h"

using namespace std;

class pkg_usr1: public pkg_usr
{
public:
	pkg_usr1(Package_Center* p)
		:pkg_usr(p) {};
	int init();
	template <typename T>
	int reg_pkg_handler();
	template <typename T>
	int handle_pkg(T* pkg, void* other);

	int handle(pkg_xx11_t* pkg, void* other);
	int handle(pkg_xx22_t* pkg, void* other);


	//Package_Center* pkg_center;  
};

int pkg_usr1::init(){	
	reg_pkg_handler<pkg_xx11_t>();
	//pkg_center->reg_pkg_handler<pkg_xx22_t>();
};

template <typename T>
int pkg_usr1::handle_pkg(T* pkg, void* other)
{
	return handle(pkg, other);
}

template <typename T>
int pkg_usr1::reg_pkg_handler()
{
	typedef std::function<void (void*, void*)> fun_hdl;
	auto fun = (fun_hdl) bind<T>(&pkg_usr1::handle_pkg<T>, this, _1, _2);
	pkg_center->reg_pkg_handler((int)T::PKG_IDX, fun);
	return 0;
}

int pkg_usr1::handle(pkg_xx11_t* pkg, void* other)
{
	cout << pkg->PKG_IDX << endl;
	return 0;
}

int pkg_usr1::handle(pkg_xx22_t* pkg, void* other)
{
	return 0;
}

int main() 
{
	Package_Center* p       = new Package_Center();
	pkg_usr1*        usr     =new pkg_usr1(p);
	pkg_xx11_t      t11;

	p->rcv_pkg(11,&t11);
	//usr->handle_pkg(&t11, NULL);
	return 0;
}

#endif
