# 《操作系统》课程实验报告 处理机调度 银行家算法

> 姓名：陈彦旭
>
> 班级：无24

## 1 问题描述

银行家算法是避免死锁的一种重要方法，将操作系统视为银行家，操作系统管理的资源视为银行家管理的资金，进程向操作系统请求分配资源即企业向银行申请贷款的过程。

请根据银行家算法的思想，编写程序模拟实现动态资源分配，并能够有效避免死锁的发生。

## 设计思路

> OS: Ubuntu 24.04.2 LTS x86_64.
>
> Language: C++.

流程图：

我们设计一个银行家类 `Banker` ，实现对系统资源的管理和调度功能。主要成员变量：

1. `total`：资源总数。
2. `allocation`：分配矩阵，表示每个进程已经分配的资源数。
3. `max_demand`：最大需求矩阵，表示每个进程对资源的最大需求。
4. `available`：可用资源，表示当前系统中可用的资源数。
5. `need`：需求矩阵，表示每个进程还需要的资源数。
6. `num_processes`：进程个数。
7. `num_resources`：资源种类数。

主要的方法有：

1. `print_state`：输出当前系统状态，包括资源总数、分配矩阵等5个重要指标。
2. `is_request_valid`：判断请求是否合法，包括检查进程号是否存在、请求资源是否超过最大需求、请求资源是否超过可用资源。
3. `is_system_safe`：判断系统是否处于安全状态，使用安全性算法检查当前资源分配是否会导致死锁，并返回一个安全序列。如果系统处于不安全状态，则返回空序列。
4. `process_request`：处理一次进程的资源分配请求。

主进程中，首先初始化系统资源，从 JSON 文件中读取存储的信息，创建银行家 `Banker` 对象，得到资源总数 `total` 、分配矩阵 `allocation` 、最大需求矩阵 `max_demand` ，由此可以计算出资源种类数、进程个数、可用资源 `available` 和需求矩阵 `need` 。

对于系统运行过程中每一个进程的每一个请求，首先判断请求是否合法。若合法，则尝试进行资源分配，更新 `allocation, available, need` 矩阵，并判断系统是否处于安全状态。若处于安全状态，则允许分配；否则，拒绝此次请求，回复刚刚分配的资源，系统回滚到请求前的状态。

## 文件结构说明

```text
.
├── bin
│   └── banker_algorithm
├── build_and_run.sh
├── CMakeLists.txt
├── data
│   ├── cpp.log
│   ├── example_1.json
│   ├── example_2.json
│   ├── example_3.json
│   └── output.log
├── include
│   ├── banker.hpp
│   ├── json.hpp
│   └── utils.hpp
├── README.md
├── report
│   └── report.md
├── run.sh
└── src
    ├── banker.cpp
    ├── main.cpp
    ├── python
    │   ├── banker.py
    │   └── main.py
    └── utils.cpp
```

## 样例测试

设计三个不同的测试样例，包括资源分配成功和失败的情况，其中都一个样例的输出结果如下：

```log
Loaded data from ../data/example_1.json successfully.
------------------------------------------------------------
Currnet state of the system resources:
Total: [10  8  7  9]
Allocated:
[[1 0 1 2]
 [2 1 0 1]
 [1 2 2 1]
 [0 1 1 2]]
Max demand:
[[5 4 3 5]
 [3 2 2 3]
 [7 5 4 6]
 [4 3 3 4]]
Need:
[[4 4 2 3]
 [1 1 2 2]
 [6 3 2 5]
 [4 2 2 2]]
Available: [6 4 3 3]
------------------------------------------------------------
Process 1 is requesting resources: [1 0 1 1]
Request is safe. Safe sequence:[1 3 0 2]
Request accepted. Allocation successful.
------------------------------------------------------------
Currnet state of the system resources:
Total: [10  8  7  9]
Allocated:
[[1 0 1 2]
 [3 1 1 2]
 [1 2 2 1]
 [0 1 1 2]]
Max demand:
[[5 4 3 5]
 [3 2 2 3]
 [7 5 4 6]
 [4 3 3 4]]
Need:
[[4 4 2 3]
 [0 1 1 1]
 [6 3 2 5]
 [4 2 2 2]]
Available: [5 4 2 2]
------------------------------------------------------------
Process 0 is requesting resources: [5 0 0 0]
Process 0 is requesting more than its maximum demand:
Request: [5 0 0 0]
Need:    [4 4 2 3]
Invalid request. Allocation rejected.
------------------------------------------------------------
Currnet state of the system resources:
Total: [10  8  7  9]
Allocated:
[[1 0 1 2]
 [3 1 1 2]
 [1 2 2 1]
 [0 1 1 2]]
Max demand:
[[5 4 3 5]
 [3 2 2 3]
 [7 5 4 6]
 [4 3 3 4]]
Need:
[[4 4 2 3]
 [0 1 1 1]
 [6 3 2 5]
 [4 2 2 2]]
Available: [5 4 2 2]
------------------------------------------------------------
Process 2 is requesting resources: [1 1 1 3]
Process 2 is requesting more than available resources:
Request:   [1 1 1 3]
Available: [5 4 2 2]
Invalid request. Allocation rejected.
------------------------------------------------------------
Currnet state of the system resources:
Total: [10  8  7  9]
Allocated:
[[1 0 1 2]
 [3 1 1 2]
 [1 2 2 1]
 [0 1 1 2]]
Max demand:
[[5 4 3 5]
 [3 2 2 3]
 [7 5 4 6]
 [4 3 3 4]]
Need:
[[4 4 2 3]
 [0 1 1 1]
 [6 3 2 5]
 [4 2 2 2]]
Available: [5 4 2 2]
------------------------------------------------------------
Process 3 is requesting resources: [1 0 1 0]
Request is safe. Safe sequence:[1 3 0 2]
Request accepted. Allocation successful.
------------------------------------------------------------
Currnet state of the system resources:
Total: [10  8  7  9]
Allocated:
[[1 0 1 2]
 [3 1 1 2]
 [1 2 2 1]
 [1 1 2 2]]
Max demand:
[[5 4 3 5]
 [3 2 2 3]
 [7 5 4 6]
 [4 3 3 4]]
Need:
[[4 4 2 3]
 [0 1 1 1]
 [6 3 2 5]
 [3 2 1 2]]
Available: [4 4 1 2]
------------------------------------------------------------
Process 0 is requesting resources: [1 0 1 0]
Request would lead to an unsafe state.
Request denied. Back to the previous state.
------------------------------------------------------------
Currnet state of the system resources:
Total: [10  8  7  9]
Allocated:
[[1 0 1 2]
 [3 1 1 2]
 [1 2 2 1]
 [1 1 2 2]]
Max demand:
[[5 4 3 5]
 [3 2 2 3]
 [7 5 4 6]
 [4 3 3 4]]
Need:
[[4 4 2 3]
 [0 1 1 1]
 [6 3 2 5]
 [3 2 1 2]]
Available: [4 4 1 2]
------------------------------------------------------------
```

这组测试样例中：

1. 第1次请求成功，分配资源后系统处于安全状态，安全序列为 `[1 3 0 2]` 。
2. 第2次请求失败，不合法，进程0请求的资源超过了其最大需求。
3. 第3次请求失败，不合法，进程2请求的资源超过了可用资源。
4. 第4次请求成功，分配资源后系统处于安全状态，安全序列为 `[1 3 0 2]` 。
5. 第5次请求失败，因为系统处于不安全状态。

## 思考题

（1）银行家算法在实现过程中需注意资源分配的哪些事项才能避免死锁？

答：

## 实验感想
