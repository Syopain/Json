## 轻量级Json库

### 依赖：

- C++ 11
-	跨平台/编译器
-	使用现代C++、STL，不依赖第三方库

### 特性:

- 符合标准的JSON解析器和生成器
- 仅支持UTF-8 JSON文本
-	仅支持以 `double` 存储 JSON `number` 类型
-	JSON `object` 类型不支持重复键

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
#### 实现解析器
解析器封装在`Parser`类中，`Parser`类有两个成员变量，分别是解析后所得的Json对象引用`val_`和一个指向json字符串当前解析位置的`char`指针`cur_`。解析在`Parser`的构造函数中进行：
```
Parser::Parser(Value &val, const std::string &content)
	: val_(val), cur_(content.c_str())
{
	val_.set_type(json::Null);
	parse_whitespace();	//跳过空字符
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
### 待补充。。。
