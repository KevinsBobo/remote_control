#include "stdafx.h"
#include "myFile.h"

void my_init_floder(char* szFloder)
{
  char szCmd[ MY_MAXPATHLEN ] = { 0 };
  sprintf_s(szCmd , MY_MAXPATHLEN ,
            "if not exist \"%s\" (md \"%s\")" , szFloder, szFloder);
  system(szCmd);
}

int my_init_file(char* szFile)
{
  FILE* fp = NULL;
  fopen_s(&fp , szFile, "a+");
  if(fp != NULL)
  {
    fclose(fp);
    fp = NULL;
    return 0;
  }

  return -1;
}
