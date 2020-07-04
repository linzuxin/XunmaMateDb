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

