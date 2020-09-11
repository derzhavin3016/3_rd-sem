#include <stdio.h>
#include <string.h>

int main( int argc, char *argv[] )
{
 
  if (argc <= 1)
  {
    printf("\n");
    return 0;
  }

  int is_nl = 1, i = 1;
      
  if (strcmp(argv[1], "-n") == 0)
    is_nl = 0, ++i;
    
  
  for (; i < argc; ++i)
    printf("%s%s", argv[i], i == argc - 1 ? "" : " ");
    
  if (is_nl)
    printf("\n");
    
  return 0;
}
