
#ifndef __PKG_CTL__
#define __PKG_CTL__


#include <map>
#include <functional>
#include "pkg__def.h"

using namespace std;
using namespace std::placeholders;


class Package_Center{
    typedef std::function<void(void*, void*)> 	fun_hdl;
    
public:
	int reg_pkg_handler(int msg_idx, fun_hdl fun){
        mulMap_RegFun.emplace(msg_idx, fun);
	}
	template <typename pkgT>
	int reg_pkg_handler()
	{
		auto fun = bind<pkgT>(&pkg_usr::handle<pkgT>, this, _1, _2);          
		reg_pkg_handler(pkgT::PKG_IDX, fun);
		return 0;
	}
	template <typename pkgT>
	int rcv_pkg(int pkg_idx, pkgT* pkg)
	{
        auto range = mulMap_RegFun.equal_range(pkg_idx);
        for (auto it = range.first; it != range.second; ++it)
		{
            if (it == mulMap_RegFun.end())
                return -1;
            auto ptrFun = it->second;
            ptrFun((void*)pkg,(void*)"");
		}
	}

    std::multimap<int, fun_hdl> mulMap_RegFun;
};

template <typename T>
class fun_snd_pkg{
public:
	static int snd(const T& pkg, Package_Center* center){
//		snd_pkg(pkg, sizeof(T), T::PKG_IDX, center);
        return 1;
	}
};

/*
template<class Function, class... Args>
inline auto FuncWrap(Function&& f, Args&& ... args) 
	-> decltype(f(std::forward<Args>(args)...))
{
	//typedef decltype(f(std::forward<Args>(args)...)) ReturnType; 
	return f(std::forward<Args>(args)...);
};
*/
#endif
