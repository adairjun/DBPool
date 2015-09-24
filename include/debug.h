#ifndef _DEBUG_H
#define _DEBUG_H

#define BUG_FILE() \
{ \
    std::cerr << " FILE(" << __FILE__ << "), LINE(" << __LINE__ << ")" << std::endl; \
}

#endif /* DEBUG_H */
