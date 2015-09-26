#ifndef _DEBUG_H
#define _DEBUG_H

#include <iostream>
// 这个BUG_FILE()宏是用来打印错误信息,使用编译器内置宏__FILE__和__LINE__来定位错误位置
#define BUG_FILE() \
{ \
    std::cerr << " FILE(" << __FILE__ << "), LINE(" << __LINE__ << ")" << std::endl; \
}

#endif /* DEBUG_H */
