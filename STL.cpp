

/* C11新特性
 
 1 auto,decltype类型推导关键字，不同的是auto是在申明时推导，而decltype只是对值，表达式等的类型推导。
 
 对于decltype有如下规则

 如果表达式e是一个变量，那么就是这个变量的类型。
 如果表达式e是一个函数，那么就是这个函数返回值的类型。
 如果不符合1和2，如果e是左值，类型为T，那么decltype(e)是T&；如果是右值，则是T。

 int a;
 double b;
 typedef decltype (a * b) type_ab; // 表达式类型，类型是double
 typedef decltype ((a)) type_a; // 左值，类型是int&

 for(auto i = 0; i < vec.size(); i ++) 
 {
        std::cout<< vec[i];
 }
 
 result_of: 推断函数对象的返回值类型
 struct foo {
 double operator()(char, int);
 };

 std::result_of<foo(char, int)>::type d1 = 10.0;
 decltype(foo()('c', 1)) d2 = 10.0;


 2 lambda 表达式
 [外部变量的传入方法](函数形参)->返回值 {函数体}
 传入方法：&表示传引用， =表示传值，若函数体没有return，可以省去->返回值
 
 int count=0;
 [&count](const char& l, const char& r) -> bool {
    count ++;
    return r > l;
 }
 
 int sum=0;
 [&](const char& v){
 sum += v - '0';
 })
 
 
 1+2法结合：
 template< typename LHS, typename RHS>
 auto AddingFunc(const LHS &lhs, const RHS &rhs) -> decltype(lhs+rhs) {return lhs + rhs;}
 
 _MSC_VER is defined when compiling with the Microsoft compiler so that it "evaluates to the major and minor number components of the compiler's version number"
 
 3 智能指针
 智能指针实现了在C++下的自动内存管理，一个地址不能同时被两个或两个以上智能指针维护

 class foo: public std::enable_shared_from_this<foo> {
 public:
 
 typedef std::shared_ptr<foo> type_ptr;


 type_ptr what_are_you_want_to_do() {
     return shared_from_this();
 }
 };
 
 4 bind函数
 可用于绑定函数、成员函数、函数对象、成员变量
 
 int f(int a, int b)
 {
 return a + b;
 }


 std::bind(f, 5, std::placeholders::_1)(x); //f(5, x)
 std::bind(f, std::placeholders::_2, std::placeholders::_1)(x, y); //f(y,x)
 
 
 绑定成员函数时，第二个参数必须是调用该成员函数时的实例,最好是指针或者引用，省去拷贝构造。对于成员函数，取地址时必须用&,因为它不在表达式里
 class X
 {
 bool f(int a);
 };

 X x;
 std::shared_ptr<X> p(new X);
 int i = 5;

 std::bind(&X::f, ref(x), std::placeholders::_1)(i);		// 相当于执行了 x.f(i)
 std::bind(&X::f, &x, std::placeholders::_1)(i);			// 相当于执行了(&x)->f(i)
 std::bind(&X::f, x, std::placeholders::_1)(i);			// 复制x，并执行(复制的x).f(i)
 std::bind(&X::f, p, std::placeholders::_1)(i);			// 复制智能指针p，并执行(复制的p)->f(i)

 
 5 std::function 作用就在于把函数或函数对象转换成function对象，并用于保存和后期调用。函数对象的好处在于，可以用类成员来定义一个关于函数的参数，比方说knn里面的k,是一种Hyperparameter.
 
 struct int_div {
 float operator()(int x, int y) const { return x / y; };
 };

 std::function<float (int x, int y)> f = int_div();

 作用于类的成员函数时要第一个参数应该是this指针
 struct X {
 int m;
 };

 X x;
 x.m = 10;

 std::function<int (X*) > f = &X::m;
 printf("Function: class member %d\n", f(&x));
 
 6 tuple:多重的Pair,最多可以10个
 std::tuple<int, std::string, double> tup0,tup1;
 tup0<tup1 ,需要所有元素都支持<操作
 
 std::get<0>(tup1) = 1; 对单个元素操作
 std::get<2>(tup1) = 3.0;
 
 7 array

 std::array<int> arr = {0, 1, 2, 3, 4, 5, 6};

 for(std::array<int>::size_type i = 0; i < arr.size(); i ++)
 std::cout<< arr[i]<< " = " << arr.at(i)<< std::endl;
 
 
 8 hash 自己定义的对象也自己定义哈希值
 #include <boost/functional/hash.hpp>
 std::hash<std::string> str_hash;
 std::hash<int> int_hash;
 std::cout<< str_hash("OWenT")<< std::endl;
 std::cout<< int_hash(3)<< std::endl; // 输出不是3哦

 9 std::ref + std::bing   + std::function

 std::ref<T> 返回std::reference_wrapper<T>，转换对象为引用
 std::cref<T> 返回常量引用对象std::reference_wrapper<const T>，转换对象为引用

 void assign(int& l, int r) {
    l = r;
 }

 std::function<void()> func = std::bind(&assign, std::ref(a), b);
 func();

 
 10 随机数分布
 std::uniform_int_distribution<int> distribution(0, 99); // [0, 99)的离散均匀分布
 std::mt19937 engine; // 随机数引擎
 int random = distribution (engine);  // 产生随机数
 
 11 C++11 增加了一个新的非常数引用（reference）类型，称为右值引用（R-value reference）
    在返回对象时，省去了临时对象的拷贝构造和析构，在没有编译器临时对象直接更名初始化的情况下节省了效率。
    
    函数返回class &&, return std::move()

12 临时对象的返回值优化问题：
 临时对象是指函数返回对象时把return的那个对象拷贝给的那个临时对象，和匿名对象要区别对待，匿名对象是指直接用类名创建的对象。
 
 临时对象产生的时候：
 
 1） 为了使函数调用成功而进行隐式类型转换的时候。传递某对象给一个函数，而其类型与函数的形参类型不同时，如果可以通过隐式转化的话可以使函数调用成功，那么此时会通过构造函数生成一个临时对象，当函数返回时临时对象即自动销毁。
 特别的，如果这个形参是一个引用，那么这个引用必须是const & ,否则报错。如果形参是一个普通对象，那么没有必要const。逻辑是，如果形参是一个普通对象，那么进入函数时会拷贝一份局部的给形参，对它的改变影响不到外面。但如果是传引用那就危险了，因为操作者误以为对引用的修改会改变函数外的实体，但是做了隐式类型转换以后就引用到一个临时对象身上了，对于它的改变将无法传递到函数外，所以干脆c++就不让改变这个引用对象，即如果要在引用型的形参上做一步类型转换，那么这个引用必须是const。
 
 int countChar (const string& str, char ch);
 char buffer[];
 char c;
 countChar (buffer, c);

 编译器会产生一个 string的临时变量，以buffer为参数进行构造，那么countChar中的str参数会绑定到此临时变量上，直到函数返回时销毁。
 
 
 2）当函数返回对象的时候。

 当函数返回一个对象时，编译器会生成一个临时对象返回。函数体内会消耗 一个构造函数(构造要返回的对象)， 一个拷贝构造函数（拷贝该对象给临时对象，并把地址存放到外面），一个析构函数（析构掉要返回的对象，此时只用临时对象）。
 
 MyCla TestFun2() {
 MyCla x(3);
 return x;
 }
 
 Myclass a = TestFun2(); // 如果是maclass a; a=Testfunc2() 那么编译器优化无效。


 编译器优化的本质是对拷贝构造函数的调用进行优化，在某些情况下，甚至可以消除或者减少对拷贝构造函数的调用。在g++编译这个demo程序的时候，添加参数-fno-elide-constructors禁用返回值优化

 return value optimization (RVO)，针对匿名对象： return class(), 用一个构造函数代替掉以上3步。直接将匿名对象更名，省去了使用临时对象那一个环节。

 NRVO 具名返回值优化：省略掉临时对象的过程较匿名对象复杂，对已if-else的返回会失效。要么把a传入函数，用x拷贝构造a(这样的话省去外面一步拷贝函数返回值以后的拷贝构造，尽管x到临时对象的拷贝构造还在),要么让x即便绕过临时对象也不会被析构，让x存在到函数外面，然后赋值给a。
 

13 顺序容器
 容器：顺序容器，根据位置，关联容器，根据key
 
 容器的大部分功能还要通过算法（排序，查找），泛型算法：不同的容器，容器内不同的元素
 
 顺序容器： vector（连续地址存储，可随机访问,中间插入开销大）
          list（链表存储，访问需遍历所有元素，插入开销小）
          deque（可随机访问，可在两端高速插入删除，中间不行）
 适配器(新的操作接口): stack, queue, priority_queue

 *初始化：
 vector< type > svec; 空容器，默认构造函数
 vector< type > svec2(svec); 拷贝构造，both容器类型and元素类型要符合
 vector< type > svec3(b,e); b,e是迭代器指针，不要求类型相同，可以直接大批量复制元素
 vector< type > svec4(n,t); n个值为t的元素创建，只适用于顺序容器
 vector< type > svec4(n); 可以没有t，那就指定元素大小，但是元素必须有默认构造函数

 *元素：
 容器内不能存放： 1 不能赋值  2 不能复制 的元素类型。 引用 / ios 就不可以

 *迭代器：
 特点：左开右闭，end指向最后一个元素的下一个元素，迭代器的运行错误是运算时错误
 sizeof(数组名)是整个数组占的长度
 vector, deque的迭代器支持特殊的操作： 1 迭代器加减  2 按元素顺序比较大小
 vector<string>::iterator mid = svec.begin() + svec.size()/2
 svec.rbegin() 逆向迭代器

 *大小：
 容器的大小 size_t, size_type是一个东西，都是unsigned int
 const list<int>::size_type list_szie = 64;

 *用容器定义的类型：
 size_type
 iterator
 const_iterator：针对const容器，不是容器里的元素
 reverse_iterator
 const_reverse_iterator
 difference_type:迭代器差值
 value_type：元素类型
 reference： 元素的左值类型引用
 const_reference： 元素的常量左值引用

 *添加删除成员： container.push_back() / s.pop_back()
 添加删除在头部： s.push_front() 仅对list, deque有效,对于vector要svec.insert(svec.begin(), "to_front") / s.pop_front()

 任意插入： s.insert(p,t) 在迭代器p以前插入t,考虑到end()迭代器返回的是最后一个元素之后的情况，insert函数返回指向新元素的迭代器，所以可以放在while里面
    while(cin >> word){iter = svec.insert(iter,word)}
         s.inset(p,n,t) 在迭代器p以前插入n个t
 任意删除：c.erase(p) 删除p，然后p指向下一个元素并返回， c.clear() 全部删除
 
 寻找并且删除
 list<string>::iterator iter = find(s. begin(), s.end(), value)
 if (iter != s.end()) s.erase(iter)


 容器在插入时都是深度拷贝原先的元素。
 !!!在插入元素后，原先的迭代器会混乱，尤其是end(), 此时就不要存储v.end()而是直接调用v.end()
 
 *比较：
 容器的比较就是容器内对应元素的比较
 
 *容器大小：
 c.size() 类型为size_type
 c.resize() 扩大，缩小，可以默认赋值或者指定新添加的值，但是修改大小以后要注意指针

 *寻址：
 c[n], c.back(), c.front()
 *但是要用引用
 list<int>::reference last = c.back() 等于 *--c.end() 注意end要减一
 
 *赋值：
 c1 = c2 删除c1并且赋值c2到c1
 c1.swap(c2) 交换，迭代器指向的元素不会改变，原来指向c1[2]的现在指向c2[2]
 c.assign(b,e) / c.assign(n,t)  b,e可以指向不一样的元素，=号必须元素一样，凡是用迭代器赋值或者初始化一个容器的，都允许使用不一样的值

 *内存管理：vector容器有预留的存储空间
 svec.capacity() 容器在必须分配新空间前可以存储的元素
 svec.reserve() 把capacity弄大
 
 14 string
 
  和vector相同，区别是不可以pop_back, pop_front, front(),back(), capacity(),reserve()
 
  初始化 string s;
        string s("a"); 存a的副本， 还可以指定数目 string s(5,'a')
        string s(char*) char*必须以\0结尾,但是string里面没有\0，还可以指定数目 string s(char *, 2) 不需要\0结尾
       string s1(s,0,10) 复制s1中前面10个元素
 
  string 特有的操作：
  s.substr()
  s.append("erd") 插在最后
  s.replace(10,3,"fourth") 替换,从下标10开始，删除3个元素，如何补入“fourth”
  string::size_type pos1 = s.find("a") 第一次出现a的地方，从0开始， rfind()最后一次出现，find_first_of（“abcd”）出现abcd任一元素，find_first_not_of找到第一次任意一个不属于。 以上找不到都返回string::npos
 
 15 adaptor 适配器
 
 封装在容器之上的具有某种特性的容器。
 stack, 要求尾部插入，3种顺序容器都可以，默认deque
 queue, 要求Push_front, vector不行, 默认deque
 priority_queue， 要求随机访问，list不行， 默认vector
 
 deque<int> deq
 stack<int> stk(deq)
 
 vector<int> vec
 stack< int, vector<int> > stack2(vec) 改变默认的容器
 
 stack: s.empty s.size() s.pop() s.top() s.push() 不可以直接调用适配器内的容器的操作函数
 
 queue/priority_queue: q.empty() q.size() q.pop() q.front() q.back() q.push()
 优先队列多一个q.top()返回一个优先级最高的元素，并不删除，优先级由<定义
 
 
 16 mutli-threading
 
 std::thread t1(function pointer)
 
 如果是pointer函数对象，那么函数名外面要括号，否则就会被误认为是传入一个参数，返回一个线程

 class fctot(){
 public: void operator()(string& s){cout<<s<<endl;}
 }
 std::thread( (fctot()),s )   fctot()外面的括号不可以省略,参数也是放在fctot外面，给thread的形参一定是值传递，即便形参是string&,所以函数内对s的改变不会到函数外。
     如果一定要用传引用，那么要用 std::ref(s), std::move(s)

 t1.join()  main thread等它完成
 t1.detach() main thread不等它完成
 
 join和detach不可以同时都选
 if(t1.joinable){t1.join()}
 
 线程不可以直接赋值，必须用move
 std::thread t2=std::move(t1)
 t2.join()
 
 thread独特的id
 std::this_thread::get_id()
 t1.get_id()
 
 std::thread::hardware_concurrency() 得出最大的可以并行的thread个数，再大就低效了
 
 如果父进程异常退出，那么子进程也会被destory,这时就要用try-catch
 try{
    father thread
 }catch(...){
    t1.join();
    throw;
 }

 */


#include <iostream>
#include <regex>
#include <thread>
#include <algorithm>
#include <tuple>
#include <functional>

using namespace std;

class foo {
public:
    int m;
    foo(){ puts("directly"); }
    foo(const foo&){ puts("copy"); }
    foo(int i){ puts("int"); m = i; }
    ~foo(){puts("xigou");}

    const foo & operator=(const foo&r){puts("this is =");
        return *this;}
};

foo foo_func1(int flag)
{
    if(flag == 1)
        return foo();
    if(flag == 2)
        return foo(1);

    // 此处返回值优化就是个渣渣
    foo retv;
    return retv; //如果用return foo就可以用构造函数代替掉拷贝构造函数
}

foo&& foo_func2(int flag)
{
    if(flag == 1)
        return foo();
    if(flag == 2)
        return foo(1);

    // 返回值优化就这么跪了
    foo retv;
    return std::move(retv);
}


// 模板类的定义不能在main函数里面
template<typename LHS,typename RHS>
auto AddingFunc(const LHS &lhs, const RHS &rhs) -> decltype(lhs+rhs){return lhs + rhs;}



void clear(const string &  str){
    cout<<str.substr(0,2)<<endl;
}


void function_1(){

    std::cout<< "beaty is good" <<std::endl;

}


//vector<int&> yinyong;  严重不可以

int main(){

    // thread
    std::thread t1(function_1); // t1 starts running
    t1.join(); // main thread waits for t1 to finish
    //t1.detach(); // t1 will run freely on its own --deamon process, main thread 不等它
    cout<<std::thread::hardware_concurrency()<<endl;


    char buffer[]="hai";
    clear(buffer);  // 如果函数的形参不是const，那就不能进行隐士转换,因为隐士转换完会给一个临时对象，该临时对象在函数结束时被释放，如果改变了这个临时对象也是对外面的没有影响。
    cout<<buffer<<endl;




    //decltype
    cout<<AddingFunc<string,string>("haha","hehe")<<endl;

    // std::function
    struct int_div {
        float operator()(int x, int y) const { return x / y; };
    };
    std::function<float (int x, int y)> f = int_div();
    cout<<f(3.0,1.0)<<endl;

    // reuslt_of
    struct foo2 {
        char operator()(char a, int b){return a;};
    };
    std::result_of<foo2(char, int)>::type d1 = 'a'; // 推断出foo2这个函数返回的是char
    cout<<d1<<endl;

    // 右值引用
    foo rr1 = foo_func1(0); // copy+析构
    foo&& rr2 = foo_func2(0); // 无，右值引用节约一次拷贝一次析构


    foo ttf;
    ttf = foo_func1(0); // 如果是非初始化状态，那么拷贝构造函数不可省略，因为调用的是赋值函数。


    vector<string> c(10,"hehe"),d(10,"haha");
    vector<string>::iterator tc=c.begin();
    cout<<*tc<<endl;
    c.swap(d);
    cout<<*tc<<endl; // swap以后指向的内容不变，只是换了一容器

    // size_type size_t
    string s("haha");
    size_t pos1 = s.find("b"); //找不到返回string::npos
    string::size_type pos2 = s.find("a");
    cout<<string::npos<<endl;


    try {
        throw 20;
    } catch (int e) { // 也可以用catch(...)捕捉任何错误
        cout<<e<<endl;
    }


    return 0;


}