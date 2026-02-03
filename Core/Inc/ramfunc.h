#pragma once

#if defined(__GNUC__)
  #define RAMFUNC  __attribute__((section(".ramfunc"), noinline))
  #define RAMDATA  __attribute__((section(".ramdata")))
#else
  #define RAMFUNC
  #define RAMDATA
#endif