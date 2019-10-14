
#ifndef __pkg_def__
#define __pkg_def__

#define DECL_PKG_IDX(pkg_idx)    			\
public:										\
	enum {PKG_IDX =pkg_idx};					\
	static int get_idx(){ return PKG_IDX; }

#define DECL_PKG_LEN(pkg_cls)    			\
public:										\
	static int get_len()						\
	{ 											\
		return sizeof(pkg_cls); 				\
	}											\



class Package_Center;

class pkg_usr{
public:
	pkg_usr(Package_Center* p)
		:pkg_center(p) {};
	virtual int init() = 0;

	template <typename T>
	int handle(T* pkg, void* other);

	template <typename T>
	int reg_pkg_handler();

	Package_Center* pkg_center;
};

enum enum_pkg_idx{
	ePKG_IDX_11 =11,
	ePKG_IDX_22 =22
};


class pkg_t{
public:
	pkg_t(int idx) :PKG_IDX(idx) {};
	int getLen() { return sizeof(pkg_t); }
	int PKG_IDX;
};

class pkg_xx11_t
{
	DECL_PKG_IDX(ePKG_IDX_11);
	DECL_PKG_LEN(pkg_xx11_t);
public: 
};

class pkg_xx22_t
{
	DECL_PKG_IDX(ePKG_IDX_22);
	DECL_PKG_LEN(pkg_xx22_t);
public:
	int num;
};





/*
class pkg_xx33_t:public pkg_t
{
public:
	pkg_xx22_t()
		:pkg_t(33) {};
	int PKG_IDX {22}; 
	
};
*/
//pkg_hdr();   

#endif
