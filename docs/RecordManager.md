<h3>RecordManager</h3>

---

胡洋凡

**RecordManager**

主要功能描述：通过给出接口的方式，实现：

* record级别的插入
* record级别的删除
* record级别的选择
* table级别的删除
* table级别的创建

###### **接口定义、类型定义**

```C++
API:
	DATA select (Table& T, vector<condition>& con, char op ='a');
	RecordResult insert(Table& T, map <string,string> &value);
	RecordResult delete ( Table& T, vector<condition> con);
	//以上为元组操作
	//以下为表操作
	using TableResult = RecordResult
    TableResult create ( Table & T);
	TableResult drop (Table & T);

//数据结构定义：
	struct RecordResult{
        bool status;//成功or失败
        string Reason;//失败原因，报错信息
    };
//类定义
class RecordManager{
  public:
	DATA select (Table& T, vector<condition>& con, char op ='a');
	RecordResult insert(Table& T, map <string,string> &value);
	RecordResult delete ( Table& T, vector<condition> con);
	//以上为元组操作
	using TableResult = RecordResult
    TableResult create ( Table & T);
	TableResult drop (Table & T);
  private:
  	//暂时没想到有什么需要存的东西。。
};
```

----

**接口分析：**

```C++
DATA select (Table& T, vector<condition>& con, char op ='a');
//此处的Table类型和condition类型参考global.h
//op默认为'a'，表示condition之间的关系是and，为'o'则表示为OR关系；默认为'a'；
//当condition长度为1或者0时，此参数无意义；
//若无任何条件（查询语句中没有where部分，则请置condition[0]="None";
```

本接口会调用buffermanager，将表逐块读入，实现select；

是否可以用index进行优化由本模块自行判断；<del>（大家都不容易</del>



```C++
RecordResult insert(Table& T, map <string,string> &value);
//请在parsing阶段将要插入的内容按照 属性-值 做成map形式，并且跳过声明为null的属性。
//e.g.: insert into student values(
//									"hyf",
//									null
//									20);
//则value应该为
//		value["name"]="hyf";
//		value["age"]="20";
//		跳过了第二个属性。
```

本接口会调用buffermanager对缓冲区进行修改；视缓冲区情况进行适当的readPage等操作；





```C++
RecordResult delete ( Table& T, vector<condition> con,char op='a');
//参数意义同insert相同 不赘述
```

本接口会调用buffermanager对缓冲区进行修改；视缓冲区情况进行适当的readPage等操作；



```C++
TableResult create ( Table & T);
//本接口会试图创建一张新的表，并把调用catalog等模块做好表的元数据存储。
//因此会自行创建文件、写文件。
//根据表的定义，可能会调用indexmanager进行索引的创建
```



```C++
TableResult drop ( Table & T);
//本接口会调用windows的API，将与此表有关的文件全部删除；
//若其他manager有清除文件/内存的API，此接口会优先调用；
//调用indexmanager、buffermanager等，并确保将内存中与此表相关的内容也全部清除；
//如果队友有mac系统的，这里可能还需要跨平台处理。。。
```

