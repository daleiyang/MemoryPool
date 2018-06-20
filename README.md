### Memory Pool 内存池 C++版 
   * 这分代码从 SGI STL 2.91.57 源码中简化而来。参考了 “STL 源码剖析“ 这本书第02章 “空间适配器”的讲解，重点在于stl_alloc.h 里面的第二级配置器的设计、实现方法、以及如何精密高效的维护内存池。侯捷书中的解释、流程图能够帮助理解核心逻辑（但也有不少废话）。
   * 这份代码经过了长时间的压力测试，在线上产品的使用中也十分很稳定，已经成为我手头内存管理的基础工具了。

### 使用方法
   * 初始化一个全局静态的pool对象：MemoryPool* pool = new MemoryPool();
   * 申请8个字节的空间： char* ch = (char*)pool->Allocate(8);
   * 归还8字节内存空间：pool->DeAllocate(ch, 8);
   * 申请一个对象大小的空间，然后用placement new的方法初始化对象：char * t = (char*)pool->Allocate(sizeof(ClassA)); ClassA * obj = new(t) ClassA;
   * 归还一个对象占用的空间：pool->DeAllocate(obj, sizeof(ClassA));