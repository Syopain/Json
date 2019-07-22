## 轻量级Json库

### 依赖：
- C++ 11
- 跨平台/编译器
- 使用现代C++、STL，不依赖第三方库

### 特性:
- 符合标准的JSON解析器和生成器
- 仅支持UTF-8 JSON文本
- 仅支持以 `double` 存储 JSON `number` 类型
- JSON `object` 类型不支持重复键

### 实现：

#### 头文件和API设计
为降低文件间的编译依赖关系，把接口从从实现中分离，`Json`类只提供接口，`Value`类负责实现该接口，`Json`类通过一个`std::unique_ptr`实现对`Value`的访问。
</br>
解析一个JSON字符串的一般用法是：
```
    Json json;
    std::string str("......");
    json.parse(str);
    json.get_type();    //获取其类型
```
JSON的数据类型声明为一个枚举类型:
```
enum type : int{ Null, True, False, Number, String, Array, Object};
```
#### 单元测试
如果解析出现异常，会返回一个错误信息给可选的`string`引用参数`status`。
```
    json.parse(content, status);
```
因为函数以及inline函数无法正确定位到__LINE__的具体行数，而是跳转到函数定义处的行数，所以这里使用宏代替函数。反斜线表示该行未结束，串接下一行。因为花括号后通常不接分号，加分号表示两条语句（代码块和一条空语句），使用do while语句接分号可以完美代替花括号代码块。
```
#define EXPECT_EQ_BASE(expect, actual) \
    do {\
        ++test_count;\
        if (expect == actual)\
            ++test_pass;\
        else {\
            std::cerr << __FILE__ << ":" << __LINE__ << ": expect: " << expect << " actual: " << actual << std::endl;\
            main_ret = 1;\
        }\
    } while(0)
```
使用宏`EXPECT_EQ_BASE(expect, actual)`判断两个参数(支持所有实现了`operator==`的类型)的值是否相等, 如果`expect != actual`(预期值不等于实际值)遍会输出错误信息，可根据错误信息定位错误到具体行。 对于不支持`operator==`的类型需要实现特例化版本(如C风格字符串)：
```
#define EXPECT_EQ_STRING(expect, actual) EXPECT_EQ_BASE(0, memcmp(expect, actual.c_str(), actual.size()));
```
#### JSON对象的各类型表示
一个值不可能同时为多种类型，所以可以使用`union`(C++17可使用std::variant代替union)来节省内存。由于`bool`类型足够简单，可以把`bool`的两个值`true`和`false`视为两个单独的类型来处理，所以无需在`union`中声明`bool`类型。C++的早期版本规定，在`union`中不能含有定义了构造函数或拷贝控制成员的类类型成员（如本例中的`string`和`vector`）。C++11标准取消了这一限制。不过，如果`union`的成员类型定义了自己的构造函数或拷贝控制成员，则该`union`的用法要比只含有内置类型的`union`更复杂。
```
namespace json {
    enum type : int{
        Null,
        True,
        False,
        Number,
        String,
        Array,
        Object
    };
}
class Value {
    json::type type_ = json::Null;
    union {
        double num_;
        std::string str_;
        std::vector<Value> arr_;
        std::vector<std::pair<std::string, Value>> obj_;
    };
};
```
##### 使用类管理union成员
> 对于union来说，要想构造或销毁类类型的成员必须执行非常复杂的操作，因此我们通常把含有类类型成员的union内嵌在另一个类当中。这个类可以管理并控制与union的类类型成员有关的状态转换。
> 为了追踪union中到底存储了什么类型的值，我们通常会定义一个独立的对象，该对象称为union的判别式（discriminant）。我们可以使用判别式来辨认union存储的值。

我们使用`Value`类来管理union成员，我们将判别式`type_`也作为Value的成员，我们将判别式定义为JSON数据类型的枚举类型来追踪其`union`成员的状态。在我们的类中定义了一些函数，这些函数可以将`union`的某种类型的值赋给`union`成员以及销毁`union`的值：
```
class Value {
    void init(const Value &rhs);
    void free();
    void set_type(type t);
    void set_number(double d);
    void set_string(const std::string& str);
    void set_array(const std::vector<Value> &arr);
    void set_object(const std::vector<std::pair<std::string, Value>> &obj);

    json::type type_ = json::Null;
    union {
        double num_;
        std::string str_;
        std::vector<Value> arr_;
        std::vector<std::pair<std::string, Value>> obj_;
    };
};
```
##### 管理判别式并销毁类类型成员
因为我们的`union`含有定义了析构函数的成员，所以必须为`union`也定义一个析构函数以销毁类类型成员。和普通的类类型成员不一样，作为`union`组成部分的类成员无法自动销毁。因为析构函数不清楚`union`存储的值是什么类型，所以它无法确定应该销毁哪个成员。我们的析构函数首先检查被销毁的对象中是否存储着类类型，如果是，则类的析构函数显式地调用相应的析构函数释放该类型使用的内存;反之，如果`union`存储的值是内置类型，则类的析构函数什么也不做。
```
    void Value::free()      //析构函数调用该函数
    {
        using std::string;
        switch (type_) {
            case json::String: str_.~string();      //显式调用相应析构函数
                break;
            case json::Array:  arr_.~vector<Value>();
                break;
            case json::Object: obj_.~vector<std::pair<std::string, Value>>();
                break;
        }
    }
```
#### 实现解析器
解析器封装在`Parser`类中，`Parser`类有两个成员变量，分别是解析后所得的Json对象引用`val_`和一个指向json字符串当前解析位置的`char`指针`cur_`。解析在`Parser`的构造函数中进行：
```
    Parser::Parser(Value &val, const std::string &content)
        : val_(val), cur_(content.c_str())
    {
        val_.set_type(json::Null);
        parse_whitespace(); //跳过空字符
        parse_value();
        parse_whitespace();
        if(*cur_ != '\0'){
            val_.set_type(json::Null);
            throw(Exception("parse root not singular"));
        }
    }
```
若解析失败，`Parser`会把`val`设为`null`类型，所以先把它设为`null`，让`parse_value()`写入解析出来的值。由于JSON语法的类型简单，只需要检测首个字符便可知道它是哪个类型的值，然后调用相关类型的解析：
```
    void Parser::parse_value()
    {
        switch (*cur_) {
            case 'n' : parse_literal("null", json::Null);  return;
            case 't' : parse_literal("true", json::True);  return;
            case 'f' : parse_literal("false", json::False); return;
            case '\"': parse_string(); return;
            case '[' : parse_array();  return;
            case '{' : parse_object(); return;
            default  : parse_number(); return;
            case '\0': throw(Exception("parse expect value"));
        }
    }
```
##### 解析数字
从 JSON 数字的语法：
```
number = [ "-" ] int [ frac ] [ exp ]
int = "0" / digit1-9 *digit
frac = "." 1*digit
exp = ("e" / "E") ["-" / "+"] 1*digit
```
我们可能直观地会认为它应该表示为一个浮点数（floating point number），为了简单起见，我们仅使用`double`表示JSON number 类型，我们只需要进行简单的格式校验，然后使用标准库的`strtod()`将字符串转换为`double`类型。

### 待补充。。。
