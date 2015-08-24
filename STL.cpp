

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
 智能指针实现了在C++下的自动内存管理，一个地址不能同时被两个或两个以上智能指针维护。智能指针本质上是用栈上的东西控制堆上的内存。
 
 #include<memory>

 shared_ptr<Class> p(new Class()) // 这里面分别开辟了两个指针，不安全

 shared_ptr<Class> p3(new class[3],[](class *p){delete[] p;}); //如果p3指向一个数组，那么一定要初始化释放的方法，否则就释放第一个class对象

 shared_ptr<Class> q= make_shared<Class>(arg) //safe and fast

 q=p; 转换指向时q原来指向的东西就被删除了 p=nullptr, p.reset()
 class * d = p.get() 去除shared pointer性,变回普通的指针

 p.use_count() 统计个数

 class foo: public std::enable_shared_from_this<foo> {
 public:
 typedef std::shared_ptr<foo> type_ptr;
 type_ptr what_are_you_want_to_do() {
     return shared_from_this();
 }
 };
 
 * weak_ptr: 两个shared pointer如果互相指对方，那么就无法自动析构，会memory leak. weak pointer的本质就是指向智能指针指向的对象，但是计数不计算进去。本质上就和普通的指针一样，但是不能被显式的delete,也不能用->指向函数，得用weak_ptr.locl()->func(),并且之前得主动检查weak_ptr.expired(). weak_ptr.use_count()可以返回shared的次数。
 
 * unique_ptr: 独有的，不可以share,也会在结束时自动析构,exception-safe pointer,再创建unique_ptr以后如果有错，那也会自己析构。
   unique_ptr<Dog> pd(new Dog("hehe"));
   pd.release(); // 放弃拥有权，返回一个普通指针（shared_ptr的get函数不放弃拥有权）
 
  优点：当unique_ptr可以直接析构掉一个数组，不像shared_ptr要点名指向
  unique_ptr<Dog[]> dogs(new Dog[3]);
  shared_ptr<Dog> pdd(new Dog[3], [](Dog *p){delete [] p;});
 
  1 ptr指针指向其他地方会导致原来指向的东西被析构
  2 std::move会导致被move的对象变成空白
 
 

 
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
 
 11 C++11 增加了右值引用（R-value reference)
 
  作用： moving semantics + perfect forwarding
    
   * 左值和右值
    左值和右值：左值和内存打交道，非左值就是右值。变量是左值，表达式或函数表达值是右值。左值可以在表达式中转换成右值。函数如果返回一个引用，那么它也可以是左值。
    int &r = 40; 是错误的，因为40是一个右值，无法赋给左值引用
    int&& r= 40; r是右值引用，而40是右值

    * moving semantics:

    在返回对象时，或者传入形参时，如果它是右值，会拷贝构造出一个临时对象，果能直接引用这个右值（T &&），那样省去了临时对象的拷贝构造和析构，在没有编译器临时对象直接更名初始化的情况下节省了效率。
    
    std::move用法： 将一个对象转换成右值,然后释放掉被move的元素. 一个对象std::move赋值给另一个对象时，如果接收的是普通的class,那么会调用move constrcutor（因为已经是一个右值了）, 如果没有，那么就调用拷贝构造函数，区别是拷贝构造函数可以实现一个深拷贝，move constructor可以实现一个浅拷贝。如果接收对象是class &&, 同时外面在初始化一对象，而不是赋值构造,那么直接返回该右值引用，什么都不会调用。比如，函数返回class &&, A a(); return std::move(a)就可以节约 a到临时对象的过程。但注意，move constructor一旦有了就会覆盖copy构造函数。
    
    总结：传形参用const class &, 如果形参不是引用，那么实参用std::move + move constructor。如果形参是 class &, 实参是 class(),那是不可以的，因为class()是右值，必须用class&& 右值引用。
         函数返回局部构建的对象并且外面在初始化新对象时，可以用std::move返回，外面用class &&接。
    
     * perfect forwarding:
   
     template<typename T>
     void relay(T arg){
            foo(arg); 这个arg的传入就叫forwarding
      }
     
     目标：
     relay(a) 调用拷贝构造函数
     relay(class()) 调用move构造函数
    
     解：
    template<typename T>
    void relay(T&& arg){ 当T是typename时，&&就是universal ref,左值右值逗可以用
        foo ( std::forward<T>(arg) )
 }
 
 
    std::forward<T>(arg): 把arg换成T&&, std::move(arg) 把arg变成rvalue
 
 
 


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
 
 *适配器的一个特点：pop，pop_back()都不会返回值，因为如果返回的值在拷贝的时候出了问题，那么原先的结构将无法还原。所以干脆就不返回值。

 16 关联容器 map
 本质结构：元素根据key的顺序排列,按照<号. multiset multimap 允许多个键的存在.和string一样，关联容器不允许有push_front, pop_front, push_back, pop_back, front, back
 
 * pair类型：
 #include<utility>
 make_pair(v1,v2)
 pair<T1,T2> p1(v1,v2)
 p.firstm p.second
 
 * 初始化： c<T> c; c<T> c1(c2); c<T> c(b,e) 没有直接初始化一个具体元素的
 #include<map>
 map<string, int> word_count
 map<Key,Value>::key_type 键的类型，const类型
 map<Key, Value>::mapped_type 值的类型
 map<Key, Value>::value_type 是pair<Key, Value>类型
 
 * 寻址： 慎用，如果键不存在，会初始化一个然后赋值0
 下标表示法 word_count["anna"]=1 , anna若没有，则创建新的元素，并初始化值为0，这点和vector不同，如果键存在，那么覆盖它的值
 指针法：返回一个value_type类型，是一个pair
 
 * 插入:以一个pair为操作元素
 m.insert(map<v1,v2>::value_type("anna",1)) 以value_type来
 m.insert(make_pair("anna",1))
 
 !!注意：insert如果建已经存在，那么值不会覆盖，和下标法相比，Insert法少一步初始化
 insert的返回值：
 pair<map<string,int>::iterator, bool> ret = m.insert(make_pair(m,1)) 若已存在，bool返回false, 迭代器始终指向键为m的map元素，可以用.second访问他的值
 
 * 寻找：
 m.count(k)  对于set就是0/1,>0表示存在
 m.find(k)  map<K,V>::iterator it=m.find("anna"), it!=m.end()
 
 * 删除：
 m.erase(key) 返回size_type,表示删除的个数
 m.erase(p) 返回void
 
 * 遍历输出：输出时是按键值的升序排列的

 
 17 关联容器set:只有键，没有值
 不支持下标操作，value_type就是key_type,没有mapped_type
 
 set<int> iset(b,e) be可以是其他容器的迭代器，这样一进去，重复的元素就没了
 
 set<string> set1
 set1.insert("the") // 同样会返回一个迭代器+一个bool
 set1.find("the") 没有就指向set1.end()
 set1.count(1)
 
 不可以通过iterator来直接修改set的元素，因为是键值是const的

 * multiset multimap 都不支持下标运算，因为一个键对应多个值.
  在内存里，同一个键的值连续存放，所以要输出键的值要用两轮，1是 .count() 
 2是find第一个元素的迭代器，然后用count次数输出
 

 18 算法
 
 本质：通过遍历由两个迭代器标记的一段元素来实现其功能。算法不直接修改容器的大小，添加删除必须靠容器本身的操作。算法会规定输入的迭代器必须支持什么操作，只要迭代器提供这个层次的操作，该算法就可以起作用。
 
 * 查找 find
 vector<int>::const_iterator result = find(vec.begin(),vec.end(),value)  从beign开始寻找这个value,找到就返回那个迭代器,找不到就返回第二个参数。
 int * result = find(a,a+6,value)

 include<algorithm>
 include<numeric>
 
 * 累加accumulate
 int sum = accumulate(vec.begin(), vec.end(), 1)
 累加到第三个实参的类型，第三个类型是初值
 字符+数字
 string sum=accumulate(v.begin(), v.end(), string(""))

 * find_first_of(iter1.begin(), iter1.end(),
                 iter2.begin(), iter2.end()),返回1里面出现2中任意一个元素的地址。
 
 * fill： 写入算法。
   fill(begin(),end(),value) 写入的是value的副本
   fill_n(begin(),10,0) 不可以在空容器上直接写，严重的错误！！！
 
 * back_inserter: 迭代器适配器，用一个对象作为实参，生成一个适应实参行为的新对象，例如，输入一个vector，返回一个push_back
   #include<iterator>
   vector<int> vec;
   fill_n(back_iterator(vec),10,0); 相当于push_back，这样就不用担心没有空间的问题，因为用的是push_back

 * copy拷贝，带_copy的算法都是不改变原先容器内的元素的
   copy(ilist.begin(),ilist.end(),back_inserter(ivec))
   = vector<int> invec(ilist.begin(), ilist.end())
 
 * replace(ilist.begin(),ilist.end(),0,42) 替换掉原序列里面的0,换成42
   replace_copy(ilist.begin(),ilist.end(),
                back_inserter(vec),0,42) 替换好以后存到vec里面
 
 
 * 去除重复单词：
   sort(words.begin(),words.end()) words本身排序
   auto it = unique(words.begin(), words.end()) 删除相邻重复元素，返回指向重复元素的指针
   words.erase(it, words.end()) 删除还是要靠容器本身的操作，算法不可以完成
 
   stable_sort: 保留相等元素的原始开始位置
 
 * 谓词：按自己定义的检测函数.名字后面带_if的都是支持谓词的
 
    [&](const string &s1, const string &s2)->bool{return s1.size()<s2.size()}
    stable_sort(words.begin(),words.end(), lamba)
    
    bool GT6(const string &s){return s.size()>=6}
    vector<string>::size_type q = count_if(words.begin(),words.end(),GT6) 返回使GT6条件成立的元素个数
 
 * 迭代器综述： #include<iterator>
   
 1 插入迭代器, 于容器对象绑定，实现插入。
   back_inserter, 要求实现push_back,
   front_inserter, 要求容器必须有Push_front, 比如vector就不行。
   inserter,多一个参数，需要指明容器的位置，在那个位置以前插入 inserter(vecotr,it)
 
   front_inserter(vec) vs inserter(vec,vec.begin()): 前者是整个一块插入到当前vec的前端，插入的对象是一个整体，后者每次插完一个元素以后，首尾元素就会更新,因为vec,begin()更新了，所以是一个一个插，不当做一个整体。

 2 Iostream迭代器，类模板，必须绑定定义了输入操作符的类型
   istream_iterator:读取输入流
   ostream_iterator：读取输出流
   初始化：
   istream_iterator<int> cin_it(cin);  从cin读入ints，定义了>>的用户类也可以，还可以直接用cin_it->调用对象成员
   istream_iterator<int> endof; 不给实参就是超出末端迭代器
   vector<int> vec(cin_it, endof) 把读入的int放到vector里面
 
   ofstream outfile;
   ostream_iterator<Sale_item> output(outfile,",") 写到outfile上，用分隔符，
   ofstream_iterator out_iter(cout, "\n") 直接输出到屏幕上
   unique_copy(vec.begin(),vec.end(),out_iter) 调用算法并且直接输出到屏幕

 3 反向迭代器:
   reverse_iterator
   rbegin() rend() 分别指向最后一个元素和第一个元素之前一个元素，++ 表示往前
   sort(vec.rbegin(),vec.rend()) 逆序输出，最大的在前面，流迭代器不可以创建反向迭代器
   .base()会让反向迭代器退一位，然后正序输出，此时要用.end()记录结尾。
 
 4 const迭代器
   对于Begin()和end(),不允许一端加const而另一端不加const。
 
 map/set/list: 提供双向迭代器
 string/vector/deque, int *p = a[4]: 提供随机访问迭代器
 sort是需要提供随机访问迭代器的，所以list就不可以作为输入，list的输入要用list.sort()调用自己的一个方法，特别注意list的大多数方法会直接修改本身，这个和泛型算法不一样。

 19 模板与泛型编程
  函数体相同，形参类型不同 -> 模板函数,编译时才会用实参来初始化形参
  函数模板+inline:
  template<typename T> inline T min(const T&, const T&)
  template<class T> class queue{ T& front();} queue在定义类体本身时不需要再用queue<T>,直接用queue,但是如果嵌套了一个模板成员，并且该成员也用了T，那么这个类成员的<T>就不可以省略。
 
 类的模板成员函数 vs 模板函数：
  类的模板成员函数在被调用的时候实例化，那个时候所有的type由类指定，实例化以后可以对输入的参数进行类型转换。而普通的模板函数就不可以，单单实例化的过程不可以有类型转换。
 

  typename和class本质上是一个东西
  T有4大作用：1 指定返回类型 2 函数形参（尽量用const &） 3 函数体内变量声明(尽量用<不用>号) 4 强制类型转换

  typedef如果出现在template外面，那么typename会屏蔽它，如果出现在里面会报错。
  
  类型名 vs 数据成员
  如果模板定义的里面出现一个类型名，那么就要区分它是一个类型而不是一个数据成员的名字，typename Pram::size_type p, 就说明size_type是一个类型，而不是一个数据成员叫size_type

  非类型形参：定义函数时还需要的参数，实例化时必须是常量身份出现
  template<class T, size_t N> void f (T (&parm)[N]){} 这里的N不可以少！ 如果数组是用引用方式传进一个函数，那么必须指定它的大小，不指定大小不可以直接传数组名（const int * 接不了 int a[10]的a），这里这个方法是可以任意改变传入数组的大小的一个方法。
 
 *实例化
  类的实例化：每次产生一个独立的类类型。class<int> class<double>相互独立。对于类中嵌套定义模板成员函数的，只有在调用该成员函数时才会实例化。
  函数的实例化：模板实参推断，根据不同的实参推断调用哪个版本. 函数实例化不会进行类型转换。 但有两个特例： 1 如果模板里形参是const &,或者一个对象本身，那么实参是否为const可以忽略 2 如果传入的是数组名或者是函数，那么会转换成为指针处理，如果形参是引用，那会直接传数组的引用，此时会检查两个形参的大小是否相等。
  
    template <typename T> int compare(const T1&, const T2&)
    
    1）compare(1,2) //运用这个实参自动匹配的情况必须保证从最右边开始匹配，也就是说2对应t2,1对应t1, 否则必须compare<int,int>(1,2)显示调用
    2）int (*p1)(const int &, const int &) = compare
     作为其他函数的形参从而实例化，compare初始化p1, p1同时实例化compare, 因为typename被确立
    3）void func(int(*)(const string&, const string&))
    func(compare) 实例化，因为typename又确定下来了，是string
 
 * template的声明如果放在.h中，那么也一定要include有具体代码的.cpp文件编译器需要同时看到两个文件。cpp中成员函数的定义要加类名<typename>:
    template <class T> void queue<T>::pop(){}

 * 模板友元的声明：
   template <class T> class Bar{
        template <class T> friend class foo;
        template <class T> friend void func(const T&)
 }
   模板类foo和模板函数func的任意一个实力都可以访问bar的私有元素

   template <class T> class foo;
   template <class T> void func(const T&);
   template <class T> class Bar{
     friend class foo<T>;
     friend void func<T>(const T&)
 }
   foo的typename是T的实例，func的typename是T的实例才可以做bar的友元。注意两个前向声明不可以少。
 
 * static: 每个模板的实例们共享一个static,static不在模板层面共享

 20 mutli-threading
 
 std::thread t1(function pointer)
 
 * 如果是pointer函数对象，那么函数名外面要括号，否则就会被误认为是传入一个参数，返回一个线程
 class fctot{
 public: void operator()(string& s){cout<<s<<endl;}
 };
 std::thread( (fctot()),s )   fctot()外面的括号不可以省略,参数也是放在fctot外面，给thread的形参一定是值传递，即便形参是string&,所以函数内对s的改变不会到函数外。
     如果一定要用传引用，那么要用 std::ref(s), std::move(s)

 * t1.join()  main thread等它完成
 t1.detach() main thread不等它完成
 join和detach不可以同时都选
 if(t1.joinable){t1.join()}
 
 * 线程不可以直接赋值，必须用move
 std::thread t2=std::move(t1)
 t2.join()
 
 * thread独特的id
 std::this_thread::get_id()
 t1.get_id()
 
 * std::thread::hardware_concurrency() 得出最大的可以并行的thread个数，再大就低效了
 
 * 如果父进程异常退出，那么子进程也会被destory,这时就要用try-catch
 try{
    father thread
 }catch(...){
    t1.join();
    throw;
 }

 
 * race condition & mutex
 
 多个线程争夺一个资源时cout，会互相竞争，此时要用mutex.mutex可以让一个线程在使用的时候另外一个线程等
 
 #inculde<mutex>
 std::mutex mu;
 void shared_print(string msg, int id){
       std::lock_guard<std::mutex> guard(mu); 如果cout抛出一个异常那么会自动释放这个thread
       cout<<msg<<i<<endl;
}

 但是cout是一个全局函数，并不是线程安全的，所以继续改进的办法是，构造一个class，把要用的函数声明为这个类的方法，这样每次调用这个函数时就要用不同的对象，继而保护了这个全局函数不被其他的影响。但这个被保护的函数不能再作为其他函数的输入或者输出
 
 class logdile{
    std:: mutex m_mutex;
    ofstream f;
 
 public:
    logfile(){f.open("log.txt")}
    void shared_print (string id, int value){
        !!!std::lock_guard<mutex> locker(m_mutex);!!!
        f<<print log<<endl;
 }};
 
 logfile log;
 log.shared_print(msg, i); 这里的就是安全的

 * unique_lock vs lock_guard: more flexibility
 
 std::unique_lock<mutex> locker(m_mutex,std::defer_lock);
 ...
 locker.lock()
 lokcer.unlock()
 ...
 lock是不可以赋值的，只能std::move(locker)
 
 * 如果每次仅仅想让一个线程用一个资源，比方说读取资源,
 std::once_flag  _flag
 std::call_once(_flag,[&](){f.open(".txt");}) // lambda表达式

 * 重要技能1：thread相互提醒: condifiton value
 thread sleep
 std::this_thread::sleep_for(chrono::seconds(1));
 std::this_thread::sleep_until(tp);

 std::condition_variable cond;
 如果线程2里在等线程1，那么在线程1完成结果并unlock以后可以发送一个cond.notify_one(),然后线程2里面可以等cond.wait(locker).线程2会unlock locker然后sleep, 如果接到通知，那么立马Lock locker，然后执行下文。
 如果很多线程在等，那么线程1应该用cond.notify_all()

 * 重要技能2：thread间通讯：future, promise
   //开辟了一个子thread结果返回给父thread
   stf::future<int> fu = std::async(std::launch::async,func,实参);
   std::async效果和std::thread一样，都是开辟一个新的线程
   x=fu.get();

   //父thread传数给子thread，父thread用Promise传给子thread的future
   std::promise<int> p;
   std::future<int> f = p.get_future();
   //std::shared_future<int> sf = f.share(); sf是可以提供给多个线程的
   f随后以std::ref(f)传入子thread fu，这里要特别注意传入的不是promise，而是future
   p.set_value(4)
   x= fu.get()
 
 * packaged_task: 可以打包一个函数给thread
 
  std::deque<std::packaged_task<int> > task_q;
  
  void thread_1(){
    std::packaged_task<int(int)> t;
      {
        std::lock_guard<std::mutex> locker(mu);
        cond.wait(locker,[](){return !task_q.empty();})
        等待父thread把task压入栈，并且栈非空，执行
        t = std::move(task_q.front());
      }
    t();
 }
  
 MAIN:
  std::thread t1(thread_t1); // run task
  std::packaged_task<int(int)> t(fact); fact返回一个int，输入一个int，如果想传入参数要用std::bind,这点和thread不一样,返回值要用int x = t.get_future().get();
  std::future<int> fu = t.get_future();
  
  {
    std::lock_guard<std::mutex> locker(mu);
    task_q.push_back(t);
  }
  
  cond.notify_one();
  fu.get()

 * 总结：
  promise: get_future()
  packaged_task: get_future()
  async(): returns a future

 * Deadlock
 如果一个函数里lock了多个mutex，可能会死锁,解决方法：
 std::lock(_mu1,_mu2)
 std::lock_guard<mutex> locker(m_mutex,std::adpot_lock);
 或者,所有的mutex都按一种顺序lock
 
 * 函数对象，函数指针的调用,callable
 class A(){
 public:
    int operator()(int N){return 0;}
    void f(int x , char c){}
 };
 A a;
 std::thread t1(std::ref(a),6) // a(6) in a thread
 std::thread t2(std::move(A()),6)
 std::thread t4([](int x){return x*x;},6) // lambda
 std::thread t6(&A::f,a,8,'W') // first copy a, then a.f(8,w), use &a, then no copy
 std::thread t7(&A::f,std::move(a),8,'W') // 用右值引用，也不会copy，但是a就消失了
 std::bind(), std::async(), std::call_once()都可以一样调用
 */


#include <iostream>
#include <regex>
#include <thread>
#include <algorithm>
#include <numeric>
#include <tuple>
#include <functional>
#include <memory>
#include <map>
#include <utility>
#include <stack>
#include <boost/lexical_cast.hpp>



using namespace std;

class foo {
public:
    int m;
    foo(){ puts("directly"); }
    foo(const foo&){ puts("copy"); }
    foo(int i){ puts("int"); m = i; }
    foo(foo&& ){puts("i am move");} // move构造函数给std::move
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


void use_foo(foo){cout<<"i am using foo_value"<<endl;}
void use_foo_lvalue(foo&){cout<<"i am using foo_lvalue"<<endl;}
void use_foo_rvalue(foo&&){cout<<"i am using foo_rvalue"<<endl;} // 形参是&&的，可以传入右值。

template<typename T>
void temp_use_foo(T&& t){
    use_foo(std::forward<T>(t));
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

// 函数形参是对一个数组的引用,必须指明大小，这里的1不可以少

void shuzuf(int (&a)[1]){
    cout<<a[0]<<endl;
}



//vector<int&> yinyong;  严重不可以

int STL(){

    // thread
    std::thread t1(function_1); // t1 starts running
    t1.join(); // main thread waits for t1 to finish
    //t1.detach(); // t1 will run freely on its own --deamon process, main thread 不等它
    cout<<std::thread::hardware_concurrency()<<endl;

    // 隐士类型转换必须为const
    char buffer[]="hai";
    clear(buffer);  // 如果函数的形参不是const，那就不能进行隐士转换,因为隐士转换完会给一个临时对象，该临时对象在函数结束时被释放，如果改变了这个临时对象也是对外面的没有影响。
    cout<<buffer<<endl;

    // 右值引用，形参传入
    foo f1;
    cout<<"start moving"<<endl;
    use_foo(f1); // 拷贝构造给形参
    use_foo(std::move(f1)); //调用move构造函数，没有就调用拷贝构造
    use_foo(foo()); // copy构造给形参，被编译器优化

    use_foo_lvalue(f1); // 函数的输入是一个左值引用
    //use_foo_lvalue(foo()); //foo()是一个右值，无法直接给foo &, 因为是左值引用，尽管可以给const foo&
    use_foo_rvalue(foo()); // 函数的输入是一个右值引用
    //use_foo_rvalue(f1); 错误，f1是一个左值，只可以传给左值引用
    cout<<"TEMP"<<endl;
    temp_use_foo(f1); //copy
    temp_use_foo(foo()); // move constuctor


    // 右值引用, 函数返回

    foo rr1 = foo_func1(0); // copy+析构
    cout<<"rvalue_start"<<endl;
    foo&& rr2 = foo_func2(0); // 无，右值引用节约一次拷贝一次析构
    cout<<"rvalue_end"<<endl;
    foo ttf;
    ttf = foo_func1(0); // 如果是非初始化状态，那么拷贝构造函数不可省略，因为调用的是赋值函数。


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



    // swap以后的iterator
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

    //vector也可以new
    vector<int> *p = new vector<int>(1);
    p->push_back(100);
    p->push_back(100);
    cout<<p->back()<<endl;
    delete p;

    struct A1{
        int m;
        A1(){}
        A1(int n):m(n){cout<<"build"<<m<<endl;}
        ~A1(){cout<<"xigou"<<m<<endl;}
    };


    //测试智能指针析构顺序，智能指针在stack上创建,无论如何创建,析构时按栈的顺序
    // 智能指针的本质是，用栈生命周期上构建的指针去控制堆上的对象
    shared_ptr<A1> ps = make_shared<A1>(151);
    shared_ptr<A1> qs = make_shared<A1>(2);

    ps = make_shared<A1>(100); // 会析构掉之前指向的151
    shared_ptr<A1> pss(new A1(5));
    shared_ptr<A1> qss(new A1(6)); // 分别调用两次内存分配，new一次，只能指针一次
    shared_ptr<A1> pshuzu(new A1[3],[](A1 *p){delete [] p;}); //指向数组时，析构函数的定义不可少。
    cout<<"shuzu:"<<pshuzu.use_count()<<endl; //1

    A1 *pn = new A1(3);
    A1 *qn = new A1(4);
    delete pn;
    delete qn;

    // 检测map的inset是否会覆盖
    map<string , int> pm;
    pm["anna"]=123;
    pm.insert(make_pair("anna", 10));
    pm["anna"]=1233;
    cout<<pm["anna"]<<endl;


    // 测试虚析构函数是否会调用基类的析构函数,结果是会
    struct father{
        virtual ~father(){cout<<"father"<<endl;}
    };

    struct son: public father{
        ~son(){cout<<"son"<<endl;}
    };

    father *pf = new son();
    delete pf;


    // 测试pop是否返回值,结果是不返回,因为不是exception safe的
    stack<int, vector<int> > pstack;
    pstack.push(112);
    pstack.pop();


    // 测试线程的函数传入方法，结果未知
    class fctot{
    public: void operator()(string& s){cout<<s<<endl;}
    };
    //std::thread( (fctot()),"hehe" );

    // 测试find_first_of
    cout<<"string:find_first_of"<<endl;
    string string1("cdac");
    string string2("cd");
    size_t cnt=0;
    auto itt = string1.begin(); // itt要在外面初始化
    for(;find_first_of(itt, string1.end(), string2.begin(), string2.end())!=string1.end();++itt){
        ++cnt;
        cout<<*itt<<endl;
    }

    cout<<"find:"<<cnt<<endl;

    // find_first_of的本质是从输入的1的迭代器开始++到1的末尾，这期间若任何一次出现过2里面的任何一个元素，有就直接返回出现的首地址。否则返回1的末尾。
    // 在这个例子里面，当从a开始迭代时，会碰到末尾的c，也算找到，所以一共出现4此。


    // shuzuf这个函数形参是对数组的一个引用， 必须指明大小 int(&a)[1]
    int at[]={1};
    shuzuf(at);

    /* referecen 上的reference, pointer, 数组都是禁止的
    int ar= 1;
    int ae= 2;
    int *&ak =ar;
    int &[] = {ar,ae}
     
     */
    return 0;


}
