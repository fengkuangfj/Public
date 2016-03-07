#pragma once

#ifndef MOD_PUBLIC
#define MOD_PUBLIC _T("¹«¹²")
#endif

#define printfPublic(FMT, ...) printf(__FILE__, __FUNCSIG__, __LINE__, FMT, __VA_ARGS__)
