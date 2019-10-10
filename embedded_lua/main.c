#include <stdio.h>

#ifdef __cplusplus
# include <lua.hpp>
#else
# include <lua.h>
# include <lualib.h>
# include <lauxlib.h>
#endif

#include "callback.h"
#include "utils/lua.h"

lua_State* init()
{
	lua_State* L = luaL_newstate();

	// adding safe lua std functions
	// wl -> whitelist
	
	// adding base module
	const char* const base_wl[13] = {
		"error",
		"tonumber",
		"tostring",
		"assert",
		"ipairs",
		"next",
		"pairs",
		"pcall",
		"select",
		"type",
		"unpack",
		"_VERSION",
		"xpcall"
	};
	expose_lua_mod(L, "base", luaopen_base, 1, 13, base_wl);

	// adding os module
	const char* const os_wl[3] = {
		"clock",
		"difftime",
		"time"
	};
	expose_lua_mod(L, "os", luaopen_os, 1, 3, os_wl);

	// exposing c functions
	expose_func(L, c_fib, "c_fib");
	expose_func(L, c_print, "print");
	return L;
}

void close(lua_State* L)
{
	lua_close(L);
}

int main(int argc, char *argv[])
{
	lua_State* L = init();
	printf("\n");
	dump_glob_tbl(L);
	printf("\nexec fib.lua\n");
	execute_script(L, "scripts/fib.lua");
	close(L);
	return 0;
}