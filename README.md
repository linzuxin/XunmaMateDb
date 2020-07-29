第二届华为云数据库挑战赛--初赛--C++
描述
1. 赛题背景

	GaussDB是华为云自主研发的最新一代云原生数据库，采用计算与存储分离、日志即数据的架构设计。计算层无状态，不保存任何数据；存储层多租户共享，负责存储全部的数据。

	元数据描述了数据的生命周期（版本信息）和存储属性（存储位置、物理大小等），是一切数据的入口。存储层要管理海量的数据需要一个高性能的元数据管理器。因此本次大赛的题目是实现一个高性能的元数据管理器。

2. 初赛赛题描述

2.1 题目内容

	我们的赛题基于“日志即数据”的思想来设计，所有的数据都可以通过回放日志得到。赛题着重考察对元数据的管理，因此简化了日志的格式，在回放时只需要将delta对应的字段“按version从小到大的顺序”累加到data对应的filed上即可得到对应版本的数据。

	初赛需要实现一个简单的元数据管理器，用来管理日志的版本和存储信息，支持WriteDeltaPacket、ReadDataByVersion接口，并通过GetStoreInft接口获取单例对象句柄（Java参赛选手无需关注GetStoreInft接口）。用户通过WriteDeltaPacket写入日志，并且可以通过ReadDataByVersion读取回放到指定版本的数据。

	我们将元数据的管理放在一个简单的存储场景中，用来管理store中的元数据。

	注意，在数据库实际运行过程中，WriteDeltaPacket写入的数据的version可能会乱序，因此在应用delta到data时需要按照version从小到大顺序应用。

下面对用到的一些名词做下解释：

Data：data即用户最终需要读取的数据，data具有key和version，以及filed数据字段。Key 是该数据的唯一标识，具有相同key的数据可以有不同的版本。将某个version及其之前的所有delta apply到该data，那么我们就说该data的版本就是这个version。所有的apply是单向的，只能累加，不能减少。没有写入任何DeltaPacket的data初始filed字段全部是0。（类似数据库的page）

Delta：delta记录的是Data的 filed字段变化信息。（类似数据库的redo log）

DeltaItem: 指的是某个key的data具体的一个变化条目。

DeltaPacket：用户每次写入的一个打包后的delta数据包，每个写入的包有个序列号，代表该包的version。该包内的所有delta具有相同的version。

Version: version指用户写入DeltaPacket时的序列号，使用该序列号表示该packet的版本，packet内的所有DeltaItem具有相同的版本。同一个DeltaPacket里面的DeltaItem可以有不同key，也可以有相同的key。同一个packet里面具有相同key的DeltaItem在apply时会被一起apply，因为他们只有一个相同的版本。

Apply：将deltaItem的delta字段“按顺序”累加到Data的field字段上。可参考replay这个类。

关于用户读取的数据的版本：

假设用户从1到1000总共写了1000个数据包，这时全局最大的version就是1000。

假如key为1的data具有 4、 6、 18、32和40 五个版本，即该key的所对应的data的变化信息（delta）是通过version为4、 6、 18、32和40的数据包写下来的。如果用户需要读取version为6的数据，那我们应该apply version 4 和6（刚刚不超过），用户需要读取version为20的数据，那我们应该apply version 4、6和18，这时数据的版本是18，用户需要读取 version为40及以上，1000及其以下的数据，我们只需要apply 4、6、18、32和40，这时数据的版本是40（说明该key在40以后再也没有被修改过，因此用version 1000去读取该key，版本为40的数据是满足要求的，因此返回的数据的版本应为40）。

假如用户需要读取4以下的版本，我们应该返回错误，因为该key对应的数据最小的版本是4。

2.2 语言限定

C++ & JAVA

注：C++和JAVA一起排名

C++答题注意事项： 1.选手在c++答题时，将.cpp和.h文件放在student目录（不能再有其他目录， c++原文件以.cpp为后缀）2. 实现common.h中定义的5抽象方法或者函数。3.将student目录打包成student.zip上传

Java答题注意事项：1. 所有代码都放在com.huawei.classroom.student包下。2. 可以添加新的类，但是不要修改我们已提供的类的包名、类名。3. 主要是实现类在DataStoreRaceImpl.java，选手要完成4个函数的实现。

2.3 作弊说明

不得使用第三方存储引擎库（如RocksDB、LevelDB、LMDB等），在其之上封装接口用来参赛

禁止使用tmpfs以及share memory等内存。包括但不限于： /dev/shm、/dev/pts、/sys /proc

如果发现有作弊行为，包括但不限于： （1）通过hack评测程序，绕过了必须的评测逻辑 （2）通过猜测数据格式进行针对性的压缩 （3）窃取评测程序代码 ... ... 则成绩无效，且取消参赛资格。

------------------------------------------------

2020第二届华为云数据库挑战赛复赛赛题
1. 赛题背景

GaussDB是华为云自主研发的最新一代云原生数据库，采用计算与存储分离、日志即数据的架构设计。计算层无状态，不保存任何数据；存储层多租户共享，负责存储全部的数据。

元数据描述了数据的生命周期（版本信息）和存储属性（存储位置、物理大小等），是一切数据的入口。存储层要管理海量的数据需要一个高性能的元数据管理器。因此本次大赛的题目是实现一个高性能的元数据管理器。

2. 复赛赛题描述

2.1 题目内容

我们的赛题基于“日志即数据”的思想来设计，所有的数据都可以通过回放日志得到。赛题着重考察对元数据的管理，因此简化了日志的格式，在回放时只需要将delta对应的字段“按version从小到大的顺序”累加到data对应的filed上即可得到对应版本的数据。

初赛需要实现一个简单的元数据管理器，用来管理日志的版本和存储信息，支持WriteDeltaPacket、ReadDataByVersion接口，并通过GetStoreInft接口获取单例对象句柄（Java参赛选手无需关注GetStoreInft接口）。用户通过WriteDeltaPacket写入日志，并且可以通过ReadDataByVersion读取回放到指定版本的数据。

复赛中加入了崩溃恢复，即写入数据过程中进程可能会挂掉，重启后需要做崩溃恢复，崩溃前写入的数据不丢失。选手需要在Init接口中做崩溃恢复。

选手自己的数据保存在Init接口传入的dir目录中。当不需要做崩溃恢复时，测评程序自己会删除dir目录的数据。

我们将元数据的管理放在一个简单的存储场景中，用来管理store中的元数据。

注意：

（1）在数据库实际运行过程中，WriteDeltaPacket写入的数据的version可能会乱序，因此在应用delta到data时需要按照version从小到大顺序应用。

（2）数据库在事务提交时日志必须已经落盘，因此WriteDeltaPacket函数返回时数据必须已经写入文件。禁止异步延迟写入，先写入内存，通过捕获信号，收到进程退出信号时再写文件。一经发现这种行为，则成绩无效，且取消参赛资格。

下面对用到的一些名词做下解释：

Data：data即用户最终需要读取的数据，data具有key和version，以及filed数据字段。Key 是该数据的唯一标识，具有相同key的数据可以有不同的版本。将某个version及其之前的所有delta apply到该data，那么我们就说该data的版本就是这个version。所有的apply是单向的，只能累加，不能减少。没有写入任何DeltaPacket的data初始filed字段全部是0。（类似数据库的page）

Delta：delta记录的是Data的 filed字段变化信息。（类似数据库的redo log）

DeltaItem: 指的是某个key的data具体的一个变化条目。

DeltaPacket：用户每次写入的一个打包后的delta数据包，每个写入的包有个序列号，代表该包的version。该包内的所有delta具有相同的version。

Version: version指用户写入DeltaPacket时的序列号，使用该序列号表示该packet的版本，packet内的所有DeltaItem具有相同的版本。同一个DeltaPacket里面的DeltaItem可以有不同key，也可以有相同的key。同一个packet里面具有相同key的DeltaItem在apply时会被一起apply，因为他们只有一个相同的版本。

Apply：将deltaItem的delta字段“按顺序”累加到Data的field字段上。可参考replay这个类。

关于用户读取的数据的版本：

假设用户从1到1000总共写了1000个数据包，这时全局最大的version就是1000。

假如key为1的data具有 4、 6、 18、32和40 五个版本，即该key的所对应的data的变化信息（delta）是通过version为4、 6、 18、32和40的数据包写下来的。如果用户需要读取version为6的数据，那我们应该apply version 4 和6（刚刚不超过），用户需要读取version为20的数据，那我们应该apply version 4、6和18，这时数据的版本是18，用户需要读取 version为40及以上，1000及其以下的数据，我们只需要apply 4、6、18、32和40，这时数据的版本是40（说明该key在40以后再也没有被修改过，因此用version 1000去读取该key，版本为40的数据是满足要求的，因此返回的数据的版本应为40）。

假如用户需要读取4以下的版本，我们应该返回错误，因为该key对应的数据最小的版本是4。

2.2 语言限定

C++ & JAVA

注：C++和JAVA一起排名

1.     C++答题注意事项： 1. 请仔细阅读include目录代码了解基类StoreInft定义，参赛者通过编写代码(可参考src简单示例实现)，继承该基类并来实现自己的libmeta_mgr，通过cmake编译生成libmeta_mgr.so。请不要修改meta_mgr_cpp目录名称及其结构，完成赛题后，上传meta_mgr_cpp.zip即可。

2.   Java答题注意事项：1. 主要是实现类在DataStoreRaceImpl.java，所有代码都放在com.huawei.hwcloud.gaussdb.data.store.race包下。2. 可以添加新的类，但是不要修改我们已提供的类的包名、类名。3. pom.xml文件中，build信息内的finalName是我们约定的构建产物名，不能随意修改。其他配置，如dependencies可根据需要进行添加，但是不能删除已有的配置项。4. 完成赛题后，上传meta_mgr_java.zip。

3.    参赛者上传赛题后，可以在提交作品的页面的日志下载中，获取运行结果（result.json）及日志信息(user.log)。

5.     选手在打印日志时，在日志头部加上" [LIBMETA_MGR] "关键字，平台使用该关键字过滤用户日志并发送给用户。可以使用系统标准输出进行打印。 C++推荐使用

easylogging打印。

 

2.5 测试环境

CPU：Intel(R) Xeon(R) Gold 6151 CPU @ 3.00GHz  (16核)

OS： Linux 3.10.0-1062.12.1.el7.x86_64

 

文件系统：EXT4

 

core file size          (blocks, -c) unlimited

data seg size           (kbytes, -d) unlimited

scheduling priority             (-e) 0

file size               (blocks, -f) unlimited

pending signals                 (-i) 515006

max locked memory       (kbytes, -l) 64

max memory size         (kbytes, -m) unlimited

open files                      (-n) 1048576

pipe size            (512 bytes, -p) 8

POSIX message queues     (bytes, -q) 819200

real-time priority              (-r) 0

stack size              (kbytes, -s) 8192

cpu time               (seconds, -t) unlimited

max user processes              (-u) unlimited

virtual memory          (kbytes, -v) unlimited

file locks                      (-x) unlimited

磁盘：Huawei Technologies Co., Ltd. Device 3714 （NVME 320G）

 

JDK：

java version "1.8.0_251"

Java(TM) SE Runtime Environment (build 1.8.0_251-b08)

Java HotSpot(TM) 64-Bit Server VM (build 25.251-b08, mixed mode)

 

Java JVM参数:

-server

-Xms4096m 

-Xmx4096m 

-XX:MaxDirectMemorySize=256m 

-XX:NewRatio=1 

-XX:+UseConcMarkSweepGC 

-XX:+UseParNewGC 

-XX:-UseBiasedLocking 

 

Java测试工程以Maven 3.6.3组织的：

Apache Maven 3.6.3

Default locale: en_US, platform encoding: ANSI_X3.4-1968

OS name: "linux"

 

2.6 程序评测逻辑

评测程序分为2个阶段：

正确性评测

此阶段评测程序会并发写入随机数据（DataPacket），写入数据过程中进行任意次进程意外退出测试，引擎需要保证异常中止不影响已经写入的数据正确性。

异常中止后，重启引擎，验证已经写入数据正确性和完整性，并继续写入数据，重复此过程直至数据写入完毕。

只有通过此阶段测试才会进入下一阶段测试。

选手在实现StoreInf::Init（const char * dir）时，如果dir目录有数据，需要做crash recovery。Init完成后测评程序会验证之前写入数据的正确性。

当不需要crash recovery时，测评程序会删除dir目录的文件。

性能评测

随机写入：30个线程并发随机写入，数据总量在20GB以内

随机读取：30个线程并发读取各个版本的数据

混合读写：15个线程读，同时15个线程写。读取的数据保证版本是已经写下去的。

注：

1. 共2个维度测试性能，每一个维度测试结束后会保留DB数据，关闭实例，重启进程，清空PageCache，下一个维度开始后重新打开新的实例

2. 读取阶段会数据进行校验，没有通过的话则评测不通过

3.参赛引擎只需保证进程意外退出时数据持久化不丢失即可，不要求保证在系统crash时的数据持久化不丢失

4. 整个评测会有时间限制在1小时内，超时后评测结束且对应提交无结果

2.7 排名规则

在正确性验证通过的情况下，对性能评测阶段整体计时，如果该阶段的正确检测全部通过，则成绩有效，根据总用时从低到高进行排名（用时越短排名越靠前）

 

2.8 资源限制

内存占用不得超过：4G（C++), 5G(JAVA)。 说明：测评程序自身运行时大概占用不超过1GB内存，因此用户实际可使用的内存要再减少1GB。

磁盘占用不得超过：100GB

2.9 作弊说明

不得使用第三方存储引擎库（如RocksDB、LevelDB、LMDB等），在其之上封装接口用来参赛

禁止使用tmpfs以及share memory等内存。包括但不限于： /dev/shm、/dev/pts、/sys /proc

如果发现有作弊行为，包括但不限于：

（1）通过hack评测程序，绕过了必须的评测逻辑

（2）写数据时先写入内存，通过捕获信号，在收到进程退出信号时再写文件

（3）选手只能访问Init接口传入的目录，禁止窥探系统其它目录

（4）窃取评测程序代码 ... ... 则成绩无效，且取消参赛资格。