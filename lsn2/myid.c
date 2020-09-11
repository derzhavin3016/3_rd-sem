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
    return MEM_ERR;                                           

// useful typedefs
typedef struct passwd Passwd;
typedef struct group  Group;

// Errors enum
enum ERR
{
  MEM_ERR =  0,
  USR_ERR = -1
};

// useful srtucture to cntain all user data
typedef struct
{
  Passwd *user_pw;        // pointer to user passwd structure.
  Group  *group_gr;       // pointer to group info structure. 
  gid_t  *groups;         // pointer to array with supplementary groups' IDs.
  size_t  groups_size;    // size of this array. 
} User_info;

/**
 * \brief Print user info function.
 * \param [in] user_pw     pointer to user passwd structure. 
 * \return 1 if all is ok,
 * \return code of error otherwise
 */
void print_info( const User_info *usr )
{
  printf("uid = %d(%s) ",           usr->user_pw->pw_uid, usr->user_pw->pw_name);
  printf("gid = %d(%s) groups = ", usr->group_gr->gr_gid, usr->group_gr->gr_name);
  
  for (int i = 0; i < usr->groups_size; ++i)
  {
    Group *gr_i = getgrgid(usr->groups[i]);
    printf("%d(%s)%s", usr->groups[i], gr_i->gr_name,
                       (i == usr->groups_size - 1) ? "\n" : ", ");
  }
} /* End of 'print_info' fucntion */ 


/**
 * \brief Get current user info.
 * \param [in] user_pw     pointer to user passwd structure.
 * \return 1 if all is ok,
 * \return code of error otherwise
 */
int cur_user_info( User_info *usr )
{
  uid_t uid = getuid();
  usr->user_pw = getpwuid(uid);
  
  gid_t gid = getgid();
  usr->group_gr = getgrgid(gid);
  
  
  usr->groups_size = getgroups(0, NULL);
  usr->groups = calloc(usr->groups_size, sizeof(gid_t));
  
  MEM_CHECK(usr->groups);
  
  getgroups(usr->groups_size, usr->groups);
  
  return 1;
} /* End of 'cur_user_info' function */

/**
 * \brief Get user info function.
 * \param [in] user_pw     pointer to user passwd structure.
 * \return 1 if all is ok,
 * \return code of error otherwise
 */
int user_info( User_info *usr )
{
#define GET_GR_LST \
  getgrouplist(usr->user_pw->pw_name, usr->group_gr->gr_gid, usr->groups, (int *)&usr->groups_size)

  usr->group_gr = getgrgid(usr->user_pw->pw_gid);
  
  usr->groups = calloc(10, sizeof(gid_t));
  
  MEM_CHECK(usr->groups);
                             
  if (GET_GR_LST == -1)
  {
    usr->groups = realloc(usr->groups, sizeof(gid_t) * usr->groups_size);
    MEM_CHECK(usr->groups);
  }

  GET_GR_LST;
  
#undef GET_GR_LST  

  return 1;
} /* End of 'user_info' function */

/**
 * \brief Process argument string function.
 * \param [in] arg pointer to string with arguments (UID or username).
 * \return 1 if all is ok,
 * \return code of error otherwise 
 */
int user_process( const char arg[], User_info *usr )
{
  if (isdigit(arg[0]))
  {
    uid_t uid = strtol(arg, NULL, 0);
    usr->user_pw   = getpwuid(uid);
  }
  else
    usr->user_pw = getpwnam(arg);
    
  if (usr->user_pw == NULL)
    return USR_ERR;
  
  user_info(usr);
  
  return 1;
} /* End of 'user_process' function */

/**
 * \brief Processing errors function.
 * \param [in] err_code  code of error.
 * \param [in] arr       pointer to string with username or UID (especially for USR_ERR).
 * \return err_code.
 */
int process_error( int err_code, const char arg[] )
{
  switch (err_code)
  {
  case MEM_ERR:
    printf("myid: Error with allocating memory\n");
    break;
  case USR_ERR:
    printf("myid: %s: no such user\n", arg);
    break;
  default:
    printf("Unrecognized error code: %d\n", err_code);
  }

  return err_code;
} /* End of 'process_error' function*/

/**
 * \brief Main program function.
 * \return None. 
 */
int main( int argc, char *argv[] )
{
  User_info usr = {0};
  int is_ok = 0;

  switch (argc)
  {
  case 1:
    is_ok = cur_user_info(&usr);
    break;
  case 2:
    is_ok = user_process(argv[1], &usr);   
    break;
  default:
    printf("Invalid amount of arguments: %d\n", argc);
    return 0;
  }
  if (is_ok)  
    print_info(&usr);
  else
    process_error(is_ok, argv[1]);
  
  if (usr.groups != NULL)
    free(usr.groups);
        
  return 0;
} /* End of 'main' function */

