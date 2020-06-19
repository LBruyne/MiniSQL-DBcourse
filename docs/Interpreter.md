## Interpreter

*功能介绍：*

##### create table

例子：

```
create table student (
		sno char(8),
		sname char(16) unique,
		sage int,
		sgender char(1),
		primary key (sno)
);
```

注意：

1. 括号内不能有空格
2. primary key结尾不能有逗号
3. 所有符号皆为英文格式

调用接口：

1. 调用Catalog，进行表格的创建
2. 其他



##### create index

例子：

```
create index stunameidx on student (sname);
```

注意：

1. 括号内不能有空格
2. 每个字段都由空格隔开，上述句子共有六个字段
3. 所有符号皆为英文格式

调用接口：

1. 调用Catalog，进行索引的创建
2. 其他



##### drop table

例子：

```
drop table student;
```

调用接口：

1. 调用Catalog，进行表格的删除
2. 其他



##### drop index

例子：

```
drop index stunameidx;
```

调用接口：

1. 调用Catalog，进行索引的删除
2. 其他



##### select * from

例子：

```
select * from student;
select * from student where sno = '88888888';
select * from student where sage > 20 and sgender = 'F';
```

注意：

1. 每个字段都要以空格隔开，包括等于号
2. 单引号内不能有空格
3. 所有符号为英文
4. 存储在table_name和vector<Condition>cons 里面了

调用接口：

1. 还没配置，不知道要用谁的接口



##### insert into

例子：

```
insert into student values ('12345678','y',22,'M');
```

注意：

1. 每个字段都要以空格隔开
2. 括号、单引号内不允许有空格
3. 所有符号皆为英文
4. 存储在vector<string>Value里面，所有值都存储为string格式，如例子存储为 12345678 y 22 M 皆为string类型

调用接口：

1. 还没配置，不知道要用谁的接口



##### delete from

例子：

```
delete from student;
delete from student where sno = '88888888';
```

注意：

1. 每个字段都要以空格隔开
2. 单引号内不允许有空格
3. 所有符号皆为英文
4. 最多只允许有一个condition，存储在了Condition con 里

调用接口：

1. 例子一调用Catalog中的dropTable，删除表格
2. 例子二未知



##### quit;

##### execfile filename;

------

##### Query（）

本函数用于处理输入的指令

##### Choice（）

本函数用于分析输入的指令并指向对应的板块

------

所以在main函数 中，提供Interpreter i 然后调用i.Query()和i.Choice()即可