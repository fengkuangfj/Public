#pragma once

#include <stdio.h>

#define MOD_PUBLIC _T("����")

#define printfPublic(FMT, ...) printf(__FILE__, __FUNCSIG__, __LINE__, FMT, __VA_ARGS__)
