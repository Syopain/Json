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
    std::string str("......")
    json.parse(str);
```
JSON的数据类型声明为一个枚举类型:
```
enum type : int{ Null, True, False, Number, String, Array, Object};
```
### 待补充。。。
