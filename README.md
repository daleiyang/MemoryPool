### Memory Pool C++�� 
   * ��ִ���� SGI STL 2.91.57 Դ���м򻯶������ο��� ��STL Դ�������� �Ȿ���02�� ���ռ����������Ľ��⡣�ص�����stl_alloc.h ����ĵڶ�������������ơ�ʵ�ַ������Լ���ξ��ܸ�Ч��ά���ڴ�ء�������еĽ��͡�����ͼ�ܹ������������߼�����Ҳ�в��ٷϻ�����
   * ��ݴ��뾭���˳�ʱ���ѹ�����ԣ������ϲ�Ʒ��ʹ����Ҳʮ�ֺ��ȶ����Ѿ���Ϊ����ͷ�ڴ����Ļ��������ˡ�

### ʹ�÷���
   * ��ʼ��һ��ȫ�־�̬��pool����MemoryPool* pool = new MemoryPool();
   * ����8���ֽڵĿռ䣺 char* ch = (char*)pool->Allocate(8);
   * �黹8�ֽ��ڴ�ռ䣺pool->DeAllocate(ch, 8);
   * ����һ�������С�Ŀռ䣬Ȼ����placement new�ķ�����ʼ������char * t = (char*)pool->Allocate(sizeof(ClassA)); ClassA * obj = new(t) ClassA;
   * �黹һ������ռ�õĿռ䣺pool->DeAllocate(obj, sizeof(ClassA));