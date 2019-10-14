 
#ifndef __common__
#define __common__
#pragma pack(1)

#define	__DEBUG__  
#ifdef	__DEBUG__  
	#define DEBUG(format,...) \
	printf("File: "__FILE__", Line: %05d: %s "format, __LINE__, __FUNCTION__, ##__VA_ARGS__)  
#else  
	#define DEBUG(format,...)  
#endif 

#define cnd_v	condition_variable	//

typedef struct TLV{
	int        tag;
	int        len;
	char       val[0];	//变长数组  c99柔性数组 
}tlv_t;					//TLV 编解码格式

#endif
