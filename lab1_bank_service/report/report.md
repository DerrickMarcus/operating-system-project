# 《操作系统》课程实验报告 银行柜员服务问题

> 姓名：陈彦旭
>
> 班级：无24

## 1 实验目的

1. 通过对进程间通信同步/互斥问题的编程实现，加深理解信号量和 P、V 操作的原理；
2. 对 Windows 或 Linux 涉及的几种互斥、同步机制有更进一步的了解；
3. 熟悉 Windows 或 Linux 中定义的与互斥、同步有关的函数。

## 2 问题描述

银行有 n 个柜员负责为顾客服务，顾客进入银行先取一个号码，然后等着叫号。当某个柜员空闲下来，就叫下一个号。编程实现该问题，用 P、V 操作实现柜员和顾客的同步。

## 3 实现要求

1. 某个号码只能由一名顾客取得；
2. 不能有多于一个柜员叫同一个号；
3. 有顾客的时候，柜员才叫号；
4. 无柜员空闲的时候，顾客需要等待；
5. 无顾客的时候，柜员需要等待。

## 4 实现思路

操作系统与编程语言：Ubuntu 24.04.2 LTS x86_64, C++ Language (standard: C++20).

首先，让我们想象银行中有一个处理取号、叫号等操作的“机器人”，它在顾客和柜员之间充当沟通的媒介，顾客取号在机器人上操作，然后机器人通知柜员有新顾客到来，柜员叫号通过机器人通知顾客。因此我们重点关注机器人如何工作。

对于顾客线程，大致流程为：

1. 顾客到达后，先取号码，将自己的号码加入到等待队列中。这里不同的顾客对“当前取号号码”和“等待队列”应设置为互斥访问，因为不能有2个顾客同时取同一个号码、同时加入等待队列。
2. 机器人通知柜员有新顾客到来，必要的时候可以叫号。
3. 顾客等待机器人叫号，当自己处于等待队列第一名且有空闲柜员的时候，开始办理业务，结束后退出。

对于柜员线程，大致流程为：

1. 等待顾客到来。
2. 有顾客到来时叫号，从等待队列中取出一个号码。这里不同的柜员对“当前叫号号码”和“等待队列”应设置为互斥访问，因为不能有两个柜员叫同一个号。
3. 通知机器人可以办理下一个顾客。
4. 服务完当前顾客后，循环执行步骤1。

设置以下主要变量：

1. 当前取号号码 `getting_number` ，有新顾客到来取号后递增。需要加锁，防止多个顾客同时取号。
2. 等待队列 `waiting_queue` ，存放已经取号但未被叫号的顾客。需要加锁，放置顾客取号和柜员叫号同时进行。
3. 当前叫号号码 `calling_number` ，有柜员空闲时，机器人从等待队列中取出一个号码叫号。
4. 代表“有顾客等待被服务”的信号量 `customer_ready` ，值大于0时代表有正在等待的顾客，若有柜员空闲可开始办理；值小于0时代表没有顾客，有多个柜员正在等待。柜员线程执行 P 操作代表机器人尝试为柜员叫号，尝试将信号量减1，如果值小于等于0则柜员线程进入阻塞等待；顾客线程执行 V 操作代表有顾客到来，机器人为顾客取号，将信号量加1。
5. 代表“有柜员空闲”的信号量 `teller_ready` ，值大于0时代表有柜员空闲，顾客到来时可直接开始办理；值小于0时代表没有柜员空闲，有多个顾客正在等待。顾客线程执行 P 操作代表机器人尝试为顾客安排柜员服务，尝试将信号量减1，如果值小于等于0则顾客线程进入阻塞等待；柜员线程执行 V 操作代表有柜员空闲，机器人为柜员叫号，将信号量加1。
6. 互斥锁 `_mutex` ，确保不同线程互斥访问信号量。

基于上述变量，顾客线程的完整流程为：

1. 尝试获取互斥锁，否则等待。
2. 取号，`getting_number++` ，将自己的号码放入等待队列 `waiting_queue` 。
3. 释放互斥锁。
4. 对信号量 `customer_ready` 执行 V 操作，机器人通知柜员有新顾客到来。
5. 对信号量 `teller_ready` 执行 P 操作，准备办理，否则等待有柜员空闲时机器人为自己叫号。
6. 等待当前叫号号码是自己所取号码。
7. 办理业务，线程休眠一定时间。
8. 结束后推出。

柜员线程的完整流程为：

1. 对信号量 `customer_ready` 执行 P 操作，准备服务，否则等待有顾客到来。
2. 获取互斥锁，否则等待。
3. 从等待队列 `waiting_queue` 中取出一个号码，叫号。
4. 释放互斥锁。
5. 对信号量 `teller_ready` 执行 V 操作，机器人通知顾客有柜员空闲。
6. 办理业务。
7. 循环执行步骤1。

需要保证所有顾客线程同时启动，而柜员线程其实其实可以提前启动，因为没有顾客的时候它们本身就处于等待状态。

约定将到达时间、服务时间的单位设为毫秒。

C++ 的标准输出 `std::cout` 不是线程安全的，如果同时有顾客到达取号、柜员叫号这些行为，可能会导致输出内容交错在一起，因此对 `std::cout` 也加上互斥锁，封装为线程安全的输出函数 `safe_print()` 。

由于顾客总人数已知，维护一个计数器，每一个顾客服务完成后计数器加一。当所有顾客服务完成后，柜员线程才能退出。

顾客取号+入队，要锁号码+队列。
柜员取号叫号，要锁队列+叫号牌。
结果就导致：顾客取号入队 和 柜员叫号取队列 都要锁住等待队列。所以 顾客取号和柜员叫号不能并发 —— 都会互相阻塞。

针对这个问题，我采取的方法是：对于顾客，取号和加入等待队列这两个操作分离，分别加锁；对于柜员，从队列中取出顾客和叫号这两个操作分离，分别加锁。事实上这也可能会导致一个问题：实际中应该是取号和加入等待队列是同时进行的，一个顾客在机器上取号，及其就会自动将他加入等待队列。而在程序中，可能会出现两个顾客几乎同时取号，但是后取号的那位顾客先拿到了队列锁，先加入了等待队列，这是不太符合实际的，现在的程序无法完全避免这一点。

对于柜员较好的方式，本程序没有采用“全局叫号号码，顾客循环监听叫号号码是否为自己所取号码”的方式，这样每一位等待的顾客都需要轮流获取叫号号码的锁，效率较低。本程序使用了 C++20 中 `std::promise, std::future` 的特性，由于等待队列中存放的是顾客对象的指针，柜员互斥地从队列中取出顾客对象指针，然后调用它提供的 `notify_called()` 接口，直接通过 `called_promise_.set_value(teller_name)` 把自己的姓名发送给顾客，顾客在 `called_future_.get()` 阻塞着。这种“一对一”地通知到对应顾客，类似于清华大学玉树园餐厅的取餐方式：顾客买餐后得到一个叫号器，菜品准备好后工作人员发送到对应叫号器，顾客看到叫号器闪烁后取餐。

TODO：this->served_by_ = called_future_.get();和teller_ready.acquire();顺序是否会导致问题？

std::chrono::steady_clock::time_point start_time_point_;相当于银行开门时间，而柜员通常在银行开始工作之间就准备好了。



# 实验感想

我在编写程序之前，一直将各种变量、信号量想象成为分属顾客和柜员的，但后来我发现，将这些所有变量想象成为银行里集中处理所有数据的“号码机器”，这样只需要考虑号码机器和顾客之间的通信、号码机器与柜员之间的通信。

```cpp
#include <thread>
#include <fstream>
#include "globals.hpp"
#include "customer.hpp"
#include "teller.hpp"
#include "utils.hpp"

int main() {
    using namespace globals;

    // （1）读取顾客数据，初始化顾客对象，TODO 初始化队列！！！
    std::vector<std::shared_ptr<Customer>> customers = load_customers_from_file("data.txt");
    total_customers = customers.size();  // 设置全局顾客总数

    // （2）初始化屏障（顾客线程 + 主线程）
    customers_barrier = new std::barrier<>(total_customers + 1);

    // （3）初始化信号量（用最大值兜底）
    customer_ready = new std::counting_semaphore<1000>(0);
    teller_ready = new std::counting_semaphore<10>(0);  // 假设最多10个柜员

    // （4）启动柜员线程
    std::vector<std::unique_ptr<Teller>> tellers;
    std::vector<std::thread> teller_threads;
    for (int i = 0; i < TELLER_COUNT; ++i) {
        auto teller = std::make_unique<Teller>("Teller" + std::to_string(i + 1));
        teller_threads.emplace_back(&Teller::serve, teller.get());
        tellers.emplace_back(std::move(teller));
    }

    // （5）启动顾客线程
    std::vector<std::thread> customer_threads;
    for (auto& customer : customers) {
        customer_threads.emplace_back(&Customer::visit_bank, customer);
    }

    // （6）记录“银行开门”时间并释放顾客线程
    global_open_time = std::chrono::steady_clock::now();
    customers_barrier->arrive_and_wait();  // 同时放行所有顾客

    // （7）等待顾客线程完成
    for (auto& t : customer_threads) {
        t.join();
    }

    // （8）结束所有柜员线程（你可能需要约定结束条件）
    for (auto& t : teller_threads) {
        t.join();
    }

    // （9）输出日志
    utils::output_customer_thread_info(customers, "customers.txt");
    utils::output_teller_thread_info(tellers, "tellers.txt");

    // （10）释放资源
    delete customers_barrier;
    delete customer_ready;
    delete teller_ready;

    return 0;
}

```