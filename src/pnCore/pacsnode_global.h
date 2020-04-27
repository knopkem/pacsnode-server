#ifndef pnGlobal_h__
#define pnGlobal_h__

namespace pacsnode {

#define _PN_STRINGIZE( x ) #x
#define PN_STRINGIZE(x) _PN_STRINGIZE(x)

#define PN_LINK_TEXT __FILE__ "(" PN_STRINGIZE(__LINE__) ") : "

}

#endif // pnGlobal_h__

