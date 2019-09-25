#include <stdio.h>

#ifdef __cplusplus
# include <lua.hpp>
#else
# include <lua.h>
# include <lualib.h>
# include <lauxlib.h>
#endif

#include "callback.h"

void print_error(lua_State* L)
{
	// the error message is on top of the stack
	// fetch it, print it and then pop it off the stack
	const char* msg = lua_tostring(L, -1);
	puts(msg);
	lua_pop(L, 1);
}

lua_State* init()
{
	lua_State* L = luaL_newstate();

	// make std libs available in the Lua object
	luaL_openlibs(L);

	// exposing c functions to the lua environment
	lua_pushcfunction(L, c_fib);
	lua_setglobal(L, "c_fib");

	return L;
}

void close(lua_State* L)
{
	lua_close(L);
}

int execute_script(lua_State* L, const char* filename)
{
	int res;

	// load the program; this supports both source code and bytecode files.
	res = luaL_loadfile(L, filename);

	if (res != LUA_OK)
	{
		print_error(L);
		return -1;
	}

	// execute the program by calling into it
	// lua_pcall(lua_State *L, int nargs, int nresults, int msgh)
	res = lua_pcall(L, 0, LUA_MULTRET, 0);

	// handle errors caused by the lua script
	if (res != LUA_OK)
	{
		print_error(L);
		return -2;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	lua_State* L = init();
	execute_script(L, "fib.lua");
	close(L);
	return 0;
}