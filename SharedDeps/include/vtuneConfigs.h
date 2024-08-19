#ifndef VTUNE_CONFIGS_H
#define VTUNE_CONFIGS_H

#if (VTUNE_ENABLED==1)
  #include <ittnotify.h>
  #define VTUNE_BEGIN __itt_resume();
  #define VTUNE_END __itt_pause();
#else
  #define VTUNE_BEGIN
  #define VTUNE_END
#endif //VTUNE_ENABLED

  
#endif //VTUNE_CONFIGS_H
