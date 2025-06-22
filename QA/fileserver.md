FileSession.h:
* 一个session有一个weak_ptr，指向一个connection，当这个connection销毁时，这session会自动销毁吗?

不会，除非循环引用，