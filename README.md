# xv6-public（操作系统课程实验）

本仓库是 [MIT 6.828 xv6-public](https://github.com/mit-pdos/xv6-public)（x86 版）的 fork，在上面完成了
**2026 年春季学期《操作系统》课程**的全部实验改动。

改动完成于 **2026 年 4 月 – 6 月**，学期结束后整理为单个提交归档。

## 实验内容

### 1. 系统调用：`date` 与 `alarm`

- `date(struct rtcdate *d)`：读 CMOS 实时钟，把当前日期时间写回用户态。
- `alarm(int ticks, void (*handler)())`：用户级定时中断。随进程保存 `alarmticks` /
  `alarmcount` / `alarmhandler` / `in_alarm` 四个字段，在内核 `trap` 的时钟中断路径里按 tick 计数，
  到期后返回用户态执行回调；回调执行期间不再嵌套触发。
- 涉及：`syscall.c/h`、`sysproc.c`、`usys.S`、`user.h`、`proc.c/h`、`trap.c`、`date.c/h`

### 2. 文件系统：二级间接块

inode 的 `addrs[]` 从 `NDIRECT+1` 扩到 `NDIRECT+2`，`NDIRECT` 由 12 改为 11，在原有的
间接块之外接入 doubly-indirect block：

```
MAXFILE = NDIRECT + NINDIRECT + NINDIRECT*NINDIRECT = 11 + 128 + 16384 = 16523
```

单文件容量上限从 **140 扇区提升到 16523 扇区**；文件系统大小 `FSSIZE` 从 1000 块扩到 20000 块，
配套写了 `big.c` 验证大文件读写。

- 涉及：`fs.c/h`、`file.h`、`param.h`、`big.c`

### 3. 惰性内存分配

`mappages` 改为对外可见，`copyuvm` 遇到尚未建立的页表项、或页表项存在但页不在内存时不再 `panic`
而是跳过，使 `fork` 能与按需分页配合工作。

- 涉及：`vm.c`

## 测试程序

`alarmtest.c`、`big.c`、`date.c`，以及 `Makefile` 里对应的 `UPROGS` 条目与运行目标。

## 构建与运行

需要 32 位 x86 工具链与 QEMU：

```sh
make qemu
```

原始的 MIT README 见 [README](README)。
