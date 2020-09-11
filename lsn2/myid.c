#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <ctype.h>



void print_info( struct passwd* user_pw, struct group *group_gr, gid_t *groups, int groups_size )
{
  printf("uid = %d(%s) gid = %d(%s) groups = ", user_pw->pw_uid, user_pw->pw_name, group_gr->gr_gid, group_gr->gr_name);
  
  for (int i = 0; i < groups_size; ++i)
  {
    struct group *gr_i = getgrgid(groups[i]);
    printf("%d(%s)%s", groups[i], gr_i->gr_name, (i == groups_size - 1) ? "\n" : ", ");
  }
}


void cur_user_info( void )
{
  uid_t uid = getuid();
  struct passwd *user_pw = getpwuid(uid);
  
  gid_t gid = getgid();
  struct group *group_gr = getgrgid(gid);
  
  
  int groups_size = getgroups(0, NULL);
  gid_t *groups = calloc(groups_size, sizeof(gid_t));
  if (groups == NULL)
  {
    printf("Not enough memory for calloc\n");
    return;
  }
  
  getgroups(groups_size, groups);

  print_info(user_pw, group_gr, groups, groups_size);
  
  free(groups);
}

void user_info( struct passwd *user_pw, struct group *group_gr )
{
  int groups_size = 0;
  gid_t *groups = calloc(1, sizeof(gid_t));
  
  if (getgrouplist(user_pw->pw_name, group_gr->gr_gid, groups, &groups_size) == -1)
    groups = realloc(groups, sizeof(gid_t) * groups_size);

  getgrouplist(user_pw->pw_name, group_gr->gr_gid, groups, &groups_size);
  
  print_info(user_pw, group_gr, groups, groups_size);
  
  free(groups);  
}

int get_by_name( const char user_name[], struct passwd **user_pw, struct group **group_gr)
{
  *user_pw = getpwnam(user_name);
  if (*user_pw == NULL)
  {
    printf("myid: %s: no such user\n", user_name);
    return 0;
  }
  
  *group_gr = getgrnam(user_name);
  
  return 1;
}

int get_by_uid( uid_t uid, struct passwd **user_pw, struct group **group_gr )
{
  *user_pw = getpwuid(uid);
  if (*user_pw == NULL)
  {
    printf("myid: %d: no such user\n", uid);
    return 0;
  }
  
  *group_gr = getgrgid((*user_pw)->pw_gid);
  
  return 1;
}

void user_process( const char arg[] )
{
  struct passwd *user_pw; 
  struct group  *group_gr; 

  if (isdigit(arg[0]))
  {
    uid_t uid = 0;
    sscanf(arg, "%d", &uid);
    if (!get_by_uid(uid, &user_pw, &group_gr))
      return;
  }
  else
    if (!get_by_name(arg, &user_pw, &group_gr))
      return;
  
  user_info(user_pw, group_gr);
}

int main( int argc, char *argv[] )
{ 
  if (argc == 1)
    cur_user_info();
  else if (argc == 2)
    user_process(argv[1]);   
  
  return 0;
}

