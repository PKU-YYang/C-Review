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


/*
 
 Provision 3: 尽量多使用const
 
 1, 区分const的目标就是看它在*号左侧还是右侧
 char * const p = "halo", p不可以改变
 
 const char * p = "halo"
 char const * p = "halo" 这两个等价，*p指向的内容不可以改变

 2, 在STL里面:
 std::vector<int> vec;
 
 const std::vector<int>::iterator iter = vec.begin()
 相当于 T* cosnt
 
 std::vector<int>::const_iterator iter=vec.begin()
 相当于const T*
 
 3, 成员函数
 
 操作符重载的时候为了不让返回值被误当做一个左值
 const rational operator* (const rational& lhs, const rational& rhs)
 这样就避免了 a*b == c 被误打成a*b=c

 const char & operator[] (std::size_t position) const
 成员函数末尾加const表示这个成员函数的调用对象可以是cosnt对象，const对象不可以调用普通的成员函数。逻辑在于，如果一个对象是const，那么它里面的数据成员是不可以被修改的，而尾部带const的成员函数规定了在该函数范围内，不可以有带数据成员的赋值语句的出现，称为bitwise cosntant。为了防止bitwise constant过于强硬，可以在声明对象成员的时候加一个Mutable，这样constant成员函数内部也可以修改它，这称为logical constant.
 
 虽然函数内部不可以修改对象的成员，但仍然可以通过返回该对象成员的引用，从而在成员函数外部提供一个接口去修改，为了避免这种情况，需要把返回值也声明成const &,即便是左值但是不可以修改。
 
 如果const成员函数和non-const成员函数大部分的代码相同，那么可以只定义一个const成员函数，然后让non-const的成员函数做一步转换：
    char & operator[] (std::size_t position)
 == const_cast<char &> (static_cast<const T&>(*this)[position]);
    去除返回值的const性    告诉对象调用const的成员函数，而不是无限调用本身（non-const的成员函数）
 */


/*
 provision 4: 一定要初始化！
 
 1, c里面数组不保证初始化，c++里面的vector保证初始化。
 三位一体：拷贝构造，赋值构造，析构函数
 
 拷贝构造： 
 class Base
 {
 public:
 
 Base(); //构造函数
 Base(const Base &);  // 复制构造函数
 Base & operator = (const Base &);   // 赋值操作符
 ~Base();   // 析构函数
 
 private:
 .......
 };

 初始化列表初始化一个对象是最高效的。初始化和赋值的区别在于：初始化发生在构造函数之前，赋值发生在构造函数之内。c++里面内置类型的初始化和赋值的成本相同，但是对于用户自定义的类型，一定要用初始化列表。
 必须放在初始化列表：const, reference, 父类的初始化。
 初始化列表的顺序是按声明成员变量时的顺序，而不是列表中的顺序，父类也比派生类要先初始化。

 类的数据成员，如果是一个数组，不可以用初始化列表，只能在构造函数里一个个赋值，如果是const数组，声明时必须初始化，否则构造函数无法初始化它（neither 初始化列表 nor 构造函数体）。static const数组甚至不可以在声明时初始化，此时只能在类外初始化。

 
 2, 多文件之间共享变量的初始化。
 精髓： 用static。 static在函数里永远保持上一次运行函数结束后的结果（第一次调用的话就自己初始化）。static在类里面算是全部对象，类的成员，对于单例模式，不希望被多次初始化，所以构造函数放在private里，此时就要利用static的特性，开辟一个返回自己的指针和返回自己指针的方法的成员。

 单例模式（线程安全）：
 class SingletonStatic
 {
 private:
    static const SingletonStatic* m_instance;
    SingletonStatic(){}
 public:
    static const SingletonStatic* getInstance()
    {
        return m_instance;
    }
 };

 const SingletonStatic* SingletonStatic::m_instance = new SingletonStatic;
 调用时用：SingletonStatic::m_instance

 
 单例模式，线程不安全：
 class Singleton{};
 Singleton & use_sigleton(){
 
    static Singleton s;
    return s;
 }
 从此要用到singleton的时候都用use_singleton(),这样就保证了永远保存一份，但是这样线程不安全。解决方法是在单线程的时候先运行一遍use_singleton(),这样里面的s会自己初始化一份。
 
 */




#include <iostream>

using namespace std;

class A{

public:

    A(int c):b(c){}

    void print() const {
        cout<<b<<endl;
    }

    int b;

    //const int s[3];
};

class SingletonStatic
{
private:
    static const SingletonStatic* m_instance;
    SingletonStatic(){}
public:
    static const SingletonStatic* getInstance()
    {
        return m_instance;
    }
};

const SingletonStatic* SingletonStatic::m_instance = new SingletonStatic;


int& staticadd(){
    static int a=1;
    a++;
    return a;

}


int main(){

    int a=1;
    int b=2;

    cout<<"aha"<<endl;
    cout<< (a==b) <<endl;

    cout<< SingletonStatic::getInstance() <<endl;

    A a_1 = A(5);
    const A a_2 = A(10);
    a_1.print();
    a_2.print();

    staticadd();
    staticadd();
    cout<<staticadd()<<endl;
}


















