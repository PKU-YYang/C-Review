/*
 Scott Meyers: << Effective C++ >> 读书笔记
 
 第0章，预备知识

 4 programming paradigms: procedural-based, object-based, object-oriented, generics
 
 size_t 是一个typdef,是标准库中计算元素个数的容器，unsigned类型
 
 函数的声明 = signature, 告诉编译器参数和返回类型， 定义是拨画内存的地方，初始化是给对象赋初值的地方。类的默认构造函数有两种：1 没有任何形参 2 每个形参都有默认的参数。
 
 class A{
    A()
    A(int a=0)
    explicit A(int b)
    A(const A& a) // copy constructor
    A& operator=(const A& a)  //copy assignment
 };
 
 1)explicit的作用就在于，禁止隐式类型转换。例如，一个函数的input是A，那么如果对该函数输入一个整数，那么他将会被隐士转换为A的实例，加上explicit就禁止了这样的转换。
 2)拷贝构造函数 vs 赋值函数： 看左值是否是已经存在的对象，A a=b 相当于 A a(b) 调用拷贝构造函数， a = b, 调用赋值构造函数。函数形参的传入和返回值的输出都是调用的拷贝构造函数

 
 function object: 一个重载了opeartor () 的类。本质在于可以通过更改类的成员更改整个函数的功能，相当于给函数一个更大的选择范围。
 
 interface: 在c++中指代函数的signature和class的可访问元素
 
 TR1: technical report 1, 是个命名空间，一些最新的规范在里面。以boost的工作为基础。
 

*/




/*

Provision 1: view c++ as a federation of languages
 
 C++ 是一个联邦语言，4大派系都有自己的守则，为了高效有时候需要切换准则，取决于用的c++里面的哪一部分，例如，C里面传值要比传引用高效
 C++ = C + Object Oriented C++ + Template C++ (template metaprogramming TMP) + STL(容器，迭代器，算法，函数对象)
 
 */


/*

Provision 2: prefer const, enums, inlines to #defines
 
 宏的缺点：1 简单替换，不进symbol table
         2 每次调用都存储一份拷贝
         3 没有封装性，没有scope的概念
 
 * 对于单纯常量，用const, enums代替#defines
 
 #define as 1.653
 const double as=1.653

 用#define 定义的变量会在预处理阶段直接被替换掉，不会进入symbol table,这会导致在debug的时候报错出错。
 建议用const 定义的变量替换 define,这样在程序中该变量实际就存了一份而不是用到就保存一份，另外define无法提供封装性，例如类专属的变量，但是const可以。
 
 enum hack: 用于在编译器不让const在声明时就附初值的例子(数组大小有时必须要这个值大小)。
 enum的特点是不允许被Pointer/reference指向
 class A{
    
    enum {NT = 5};
    int scores[NT]
 }

 * 对于函数宏，用Inline代替
 
 #define call_with_max(a,b) f((a) > (b) : (a) : (b)) 需要对所有变量加上括号,并且因为是简单替换，如果a本身是表达式的话，传入的a会被执行两次。
 

 template<typename T>
 inline void callwithMax(const T&a, const T&b){ f(a>b ? a:b)} 以上两个问题都不需要考虑

*/