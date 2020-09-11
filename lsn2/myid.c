#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <ctype.h>


#define MEM_CHECK(ptr)                                        \
  if ((ptr) == NULL)                                          \
  {                                                           \
    printf("Error with allocating memory for %s\n"            \
           "In line %d, function %s, file %s\n",              \
           #ptr, __LINE__, __PRETTY_FUNCTION__, __FILE__);    \
    return;                                                   \
  }

// useful typedefs
typedef struct passwd Passwd;
typedef struct group  Group;


/**
 * \brief Print user info function.
 * \param [in] user_pw     pointer to user passwd structure.
 * \param [in] group_pr    pointer to group info structure.
 * \param [in] groups      pointer to array with supplementary groups' IDs.
 * \param [in] groups_size size of this array.
 * \return None. 
 */
void print_info( Passwd *user_pw, Group *group_gr, gid_t *groups, int groups_size )
{
  printf("uid = %d(%s) ",          user_pw->pw_uid , user_pw->pw_name);
  printf("gid = %d(%s) groups = ", group_gr->gr_gid, group_gr->gr_name);
  
  for (int i = 0; i < groups_size; ++i)
  {
    Group *gr_i = getgrgid(groups[i]);
    printf("%d(%s)%s", groups[i], gr_i->gr_name, (i == groups_size - 1) ? "\n" : ", ");
  }
} /* End of 'print_info' fucntion */ 


/**
 * \brief Get current user info.
 * \return None. 
 */
void cur_user_info( void )
{
  uid_t uid = getuid();
  Passwd *user_pw = getpwuid(uid);
  
  gid_t gid = getgid();
  Group *group_gr = getgrgid(gid);
  
  
  int groups_size = getgroups(0, NULL);
  gid_t *groups = calloc(groups_size, sizeof(gid_t));
  
  MEM_CHECK(groups);
  
  getgroups(groups_size, groups);

  print_info(user_pw, group_gr, groups, groups_size);
  
  free(groups);
} /* End of 'cur_user_info' function */

/**
 * \brief Get user info function.
 * \param [in] user_pw     pointer to user passwd structure.
 * \return None. 
 */
void user_info( Passwd *user_pw )
{
  Group *group_gr = getgrgid(user_pw->pw_gid);

  int groups_size = 0;
  
  gid_t *groups = calloc(10, sizeof(gid_t));
  
  MEM_CHECK(groups);
   
  if (getgrouplist(user_pw->pw_name, group_gr->gr_gid, groups, &groups_size) == -1)
  {
    groups = realloc(groups, sizeof(gid_t) * groups_size);
    MEM_CHECK(groups);
  }

  getgrouplist(user_pw->pw_name, group_gr->gr_gid, groups, &groups_size);
  
  print_info(user_pw, group_gr, groups, groups_size);
  
  free(groups);  
} /* End of 'user_info' function */

/**
 * \brief Process argument string function.
 * \param [in] arg pointer to string with arguments (UID or username).
 * \return None. 
 */
void user_process( const char arg[] )
{
  Passwd *user_pw  = NULL;

  if (isdigit(arg[0]))
  {
    uid_t uid = strtol(arg, NULL, 0);
    user_pw   = getpwuid(uid);
  }
  else
    user_pw = getpwnam(arg);
    
  if (user_pw == NULL)
  {
    printf("myid: %s: no such user\n", arg);
    return;
  }  
  
  user_info(user_pw);
} /* End of 'user_process' function */

/**
 * \brief Main program function.
 * \return None. 
 */
int main( int argc, char *argv[] )
{
  switch (argc)
  {
  case 1:
    cur_user_info();
    break;
  case 2:
    user_process(argv[1]);   
    break;
  default:
    printf("Invalid amount of arguments: %d\n", argc);
  }
    
  return 0;
} /* End of 'main' function */

