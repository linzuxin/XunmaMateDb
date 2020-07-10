# 实现步骤

1. 纯内存版本实现
2. io版本实现
3. socket版本实现
4. 多线程版本实现
5. 调优版本实现

---------------------------

## 纯内存版本实现

### 1. 简单模型的定义

简单模型的定义包含了配置文件模型，数据Data信息结构。具体如下：

+ 1. 配置文件模型

  模型作用主要是提供配置参数，根据字段调整系统运运行配置参数和状态结构，在initProfile进行初始化参数
  > 模型名称  ProfileModel  
  > 字段列表
  >| 字段名  | 字段备注    | 类型     |
  >| ----   | :----:     | ---     |
  >| bufSize| 缓存buf大小 | uint64_t |
  >| sysVersion | 系统版本号 1.0.0 每位*100加上| uint64_t |
  >| sysDateTime | 系统创建时间 | uint64_t |
  >| dataVersion | 数据最大的版本号 | uint64_t |
  >
  > 函数列表
  >| 函数名 | 函数备注 |  返回类型 |
  >| ---- | :----: | --- |
  >| initProfileModel | 初始化 | ProfileModel* |
  >| setDataVersion | 设置版本号 | void |
  >| getDataVersion | 获取字段信息 | uint64_t |
  > initProfileModel函数参数  
  > 无参数信息  
  > setDataVersion函数参数  
  >| 参数名 | 参数备注 |  参数类型 |
  >| ---- | :----: | --- |
  >| profile | 配置文件 | ProfileModel * |
  >| version | 版本号 | uint64_t |
  > getDataVersion函数参数  
  >| 参数名 | 参数备注 |  参数类型 |
  >| ---- | :----: | --- |
  >| profile | 配置文件 | ProfileModel * |
  
+ 2. 数据Data信息结构

  >模型名称  DataModel
  >| 字段名 | 字段备注 | 类型|
  >| ---- | :----: | --- |
  >| version | 版本号 | uint64_t |
  >| field  | 字段信息 | uint64_t[64] |

+ 3. 系统创建结构

  >模型名称  SystemModel

### 2. 管理结构定义

管理结构包括了索引表,数组的管理.索引表可采用红黑树，B+树，跳跃表，平衡树等等数据结构.数组可以用链表，数组数据结构.这边采用红黑树和数组来实现。具体如下：

+ 1. 红黑树
  >模型名称 KeyMap
  >| 字段名 | 字段备注 | 类型|
  >| ---- | :----: | --- |
  >| key | 关键key | KeyNode |
  >| value  | 值 | ValueNode |
  > 函数列表
  >| 函数名 | 函数备注 |  返回类型 |
  >| ---- | :----: | --- |
  >| putRoot | 添加key-value | KeyNode |
  >| getRoot | 获取key-value | ValueNode |
  > putRoot函数参数
  >| 参数名 | 参数备注 | 参数类型 |
  >| ---- | :----: | --- | --- |
  >| root | 根节点 |  KeyMap**  |
  >| key  |  关键键 | KeyNode |
  >| value |  值键 | ValueNode |
  > getRoot函数参数
  >| 参数名 | 参数备注 | 参数类型 |
  >| ---- | :----: | --- | --- |
  >| root | 根节点 |  KeyMap**  |
  >| key  |  关键键 | KeyNode |
  >先声明需要使用，其他信息后续补充

+ 2. 数组

  >模型名称 ValueArray

### 3. 主流程实现

主流程入口主要包含了初始化流程，析构流程，写数据流程，读数据流程4大流程，具体流程如下：

+ 1. 初始化流程
  > 创建
  > > XXX  
  > > 3sdf  
  >
  > XXX  
  > xxx

+ 2. 析构流程
  > 析构
  >

+ 3. 写数据流程
  > 流程信息  
  > 判断写函数参数是否合法校验，不合法返回失败  
  > >判断个数不能为0
  >
  > 调用SysInfo模型中的setSysVersion赋值版本号函数  
  > 循环参数里面包
  > > 调用IoMMap模型中插入函数

  备注信息

  >备注信息
  > > 声明DeltaItem对象，并赋值为包里面对象  
  > > DeltaItem对象key赋值
  > > 判断是否存在key
  > > > 存在，则先求出插入位置，并插入  
  > > > 不存在，插入

+ 4. 读数据流程
  > 析构
  >

### 4. 红黑树流程

+ 1. 创建
  > 创建
  > > XXX  
  > > 3sdf  
  >
  > XXX  
  > xxx

+ 2. 析构
  > 析构
  >

+ 3. XX
  > 析构
  >

### 5. 数组流程

+ 1. 创建
  > 创建
  > > XXX  
  > > 3sdf  
  >
  > XXX  
  > xxx

+ 2. 析构
  > 析构
  >

+ 3. XX
  > 析构
  >

### 6. 生命周期管理

### 7. 测试用例
