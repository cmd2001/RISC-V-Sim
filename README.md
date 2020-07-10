# RISC-V-Sim
RISC-V Simulator for PPCA 2020 of ACM Class, SJTU

## Todo
- [x] IF, ID, EX, MEM, WB
- [x] A Valid Serial Version
- [x] Abstract Level between Different Parts
- [x] 3 Clocks Delay for Memory Access
- [x] Forwarding
- [x] Pipeline
- [x] Branch Prediction

## Architecture

五级流水：

`IF->ID->EX->MEM->WB`

通过`Forwarding & Bypass`, `pipeline interlock`解决冲突。

`2`位饱和分支预测，对`PC`二进制后`7`位进行哈希。

## Performance

`OJ`运行时间：`6432ms`

部分测试点分支预测成功率：

`qsort`: `0.874243`

`gcd`: `0.675`

`pi`: `0.824027`

`heart`: `0.696774`

`superloop`: `0.938181`

## Change Log

(众所周知，这是个栈)

#### 2020.07.10

优化程序结束检查，由`iF`移动至`WB`。(方便分支预测)

实现分支预测。

无条件跳转移到`EX`以避免数据冒险。

实现寄存器锁，无条件跳转回`ID`。

优化分支预测效率。

#### 2020.07.09

移动无条件跳转处理位置(`ID->EX`)

添加寄存器修改记录。

实现`forwarding`，完成流水线。

重新实现内存延迟。

速度优化(卡常)：寄存器文件读取全部寄存器改为只读取所需寄存器。

优化无条件跳转，移回`ID`

#### 2020.07.08

调试，手玩`gcd`，修正`BLT`实现有误，`Memory Access`位移打错。

手玩`basicopt1`，修正`BGEU`等号问题。

串行版本完成，通过全部测试数据。

添加抽象类(`IF/ID`, `ID/EX`, `EX/MEM`, `MEM/WB`)

添加内存延迟。

#### 2020.07.07

实现`Execution`，`Memory Access`。

初版完成。

调试(部分行为细节)

#### 2020.07.06

阅读文档。

实现`Instruction Decode`。

调试`Instruction Decode`。

