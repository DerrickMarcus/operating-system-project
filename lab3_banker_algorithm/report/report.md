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
> Language: Python 3.12.3

流程图：

## 文件结构说明

## 样例测试

## 思考题

## 实验感想

# include "banker.hpp"

# include <iostream>

# include <iomanip>

Banker::Banker(
    const std::vector<int>& total,
    const std::vector<std::vector<int>>& allocation,
    const std::vector<std::vector<int>>& max_demand,
    const std::vector<std::pair<int, std::vector<int>>>& requests
)
    : total(total),
      allocation(allocation),
      max_demand(max_demand),
      requests(requests),
      num_processes(allocation.size()),
      num_resources(total.size())
{
    calculate_need();
    calculate_available();
}

void Banker::calculate_need() {
    need.resize(num_processes, std::vector<int>(num_resources));
    for (int i = 0; i < num_processes; ++i) {
        for (int j = 0; j < num_resources; ++j) {
            need[i][j] = max_demand[i][j] - allocation[i][j];
        }
    }
}

void Banker::calculate_available() {
    available = total;
    for (const auto& alloc : allocation) {
        for (int j = 0; j < num_resources; ++j) {
            available[j] -= alloc[j];
        }
    }
}

bool Banker::is_request_valid(int pid, const std::vector<int>& request) const {
    if (pid < 0 || pid >= num_processes) {
        std::cerr << "Invalid process ID: " << pid << '\n';
        return false;
    }

    for (int j = 0; j < num_resources; ++j) {
        if (request[j] > need[pid][j]) {
            std::cerr << "Request exceeds process " << pid << "'s need.\n";
            return false;
        }
        if (request[j] > available[j]) {
            std::cerr << "Request exceeds available resources.\n";
            return false;
        }
    }
    return true;
}

bool Banker::is_safe_state() const {
    std::vector<int> work = available;
    std::vector<bool> finish(num_processes, false);
    int finished = 0;

    while (finished < num_processes) {
        bool found = false;
        for (int i = 0; i < num_processes; ++i) {
            if (!finish[i]) {
                bool can_finish = true;
                for (int j = 0; j < num_resources; ++j) {
                    if (need[i][j] > work[j]) {
                        can_finish = false;
                        break;
                    }
                }
                if (can_finish) {
                    for (int j = 0; j < num_resources; ++j) {
                        work[j] += allocation[i][j];
                    }
                    finish[i] = true;
                    found = true;
                    ++finished;
                }
            }
        }
        if (!found) return false;
    }
    return true;
}

void Banker::process_all_requests() {
    for (const auto& [pid, request] : requests) {
        std::cout << "\n>>> Processing request from process " << pid << ": ";
        print_vector(request);
        if (!is_request_valid(pid, request)) {
            std::cout << "Request denied (invalid).\n";
            continue;
        }

        // Tentatively allocate
        for (int j = 0; j < num_resources; ++j) {
            available[j] -= request[j];
            allocation[pid][j] += request[j];
            need[pid][j] -= request[j];
        }

        if (is_safe_state()) {
            std::cout << "Request granted (system is in safe state).\n";
        } else {
            // Rollback
            for (int j = 0; j < num_resources; ++j) {
                available[j] += request[j];
                allocation[pid][j] -= request[j];
                need[pid][j] += request[j];
            }
            std::cout << "Request denied (would lead to unsafe state).\n";
        }

        print_state();
    }
}

void Banker::print_state() const {
    std::cout << "\n=== Current System State ===\n";
    std::cout << "Total:       "; print_vector(total);
    std::cout << "Available:   "; print_vector(available);
    std::cout << "\nAllocation:\n"; print_matrix(allocation);
    std::cout << "\nMax Demand:\n"; print_matrix(max_demand);
    std::cout << "\nNeed:\n"; print_matrix(need);
}

void Banker::print_vector(const std::vector<int>& vec) const {
    for (int val : vec) {
        std::cout << std::setw(3) << val << " ";
    }
    std::cout << '\n';
}

void Banker::print_matrix(const std::vector<std::vector<int>>& mat) const {
    for (const auto& row : mat) {
        print_vector(row);
    }
}
