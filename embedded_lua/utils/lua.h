#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#ifdef __cplusplus
# include <lua.hpp>
#else
# include <lua.h>
# include <lualib.h>
# include <lauxlib.h>
#endif

void print_error(lua_State* L);
void dump_glob_tbl(lua_State* L);

void expose_func(lua_State* L, lua_CFunction c_func, char* func_name);
void expose_lua_mod(lua_State* L, const char* modname, lua_CFunction openf, int glb, int whitelist_size, const char* const whitelist[]);

int execute_script(lua_State* L, const char* filename);