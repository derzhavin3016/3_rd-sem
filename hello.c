#include <stdlib.h>
#include <unistd.h>

/**
 * Main program function
 * ARGUMENTS: None.
 * RETURNS:
 * 0 if all is ok
 * not 0 otherwise
 */
int main( void )
{
  // String for output
  const char str[] = "Hello world!\n";
  // String size (0 byte included)
  size_t str_size = sizeof(str);
  
  // printing String and checking if print was correct
  if (write(STDOUT_FILENO, str, str_size) != (ssize_t)str_size)
    return 1;
  
  return 0;
} /* End of 'main' function */ 

/* END OF 'HELLO.C' FILE */
