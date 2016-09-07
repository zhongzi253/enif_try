#include <stdbool.h>  
#include <math.h>  
#include <stdio.h>
#include <stdlib.h>
#include "erl_nif.h"  

static bool isPrime(int i)
{
  int j;  
  int t = sqrt(i) + 1;  
  for(j = 2; j <= t; ++j)  
  {  
    if(i % j == 0)  
      return false;  
  }  
  return true;  
}

static ERL_NIF_TERM nif_findPrime(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])  
{  
  int n;  
  if(!enif_get_int(env, argv[0], &n))  
    return enif_make_badarg(env);  
  else  
  {  
    int i;  
    ERL_NIF_TERM res = enif_make_list(env, 0);  
    for(i = 2; i < n; ++i)  
    {  
      if(isPrime(i))  
        res = enif_make_list_cell(env, enif_make_int(env, i), res);  
    }  
    return res;  
  }  
}  
/*
static ERL_NIF_TERM nif_binfunc(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
  ErlNifBinary bin;
  unsigned i;

  if (argc != 1 || !enif_inspect_binary(env, argv[0], &bin))
    return enif_make_badarg(env);
  printf("received binary of length %zu\r\ndata: ", bin.size);
  for (i = 0; i < bin.size; ++i)
    printf("%x ", bin.data[i]);
  printf("\r\n");
  return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM nif_create_tuple(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]){
  char atom[20] = "";
  int a1 = 0;
  int a2 = 0;

  enif_get_atom(env, argv[0], atom, 20, ERL_NIF_LATIN1);
  enif_get_int(env, argv[1], &a1); 
  enif_get_int(env, argv[1], &a2); 

  printf("Get att: a1=%s, a2=%d, a3=%d\n", atom, a1, a2);

  return enif_make_tuple(
      env, 
      3, 
      enif_make_atom(env, atom),
      enif_make_int(env, a1), 
      enif_make_int(env, a2)
  );

}

*/
static ErlNifFunc nif_funcs[] = {  
  {"findPrime", 1, nif_findPrime}
 // {"create_tuple", 3, nif_create_tuple}
};  

ERL_NIF_INIT(prime, nif_funcs, NULL, NULL, NULL, NULL)  
