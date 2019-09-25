#include <stdio.h>

#ifdef __cplusplus
# include <lua.hpp>
#else
# include <lua.h>
# include <lualib.h>
# include <lauxlib.h>
#endif

void print_error(lua_State* state)
{
	// the error message is on top of the stack
	// fetch it, print it and then pop it off the stack
	const char* msg = lua_tostring(state, -1);
	puts(msg);
	lua_pop(state, 1);
}

void execute_script(const char* filename)
{
	lua_State* state = luaL_newstate();

	// make std libs available in the Lua object
	luaL_openlibs(state);

	int res;

	// load the program; this supports both source code and bytecode files.
	res = luaL_loadfile(state, filename);

	if (res != LUA_OK)
	{
		print_error(state);
		lua_close(state);
		return;
	}

	// execute the program by calling into it
	// lua_pcall(lua_State *L, int nargs, int nresults, int msgh)
	res = lua_pcall(state, 0, LUA_MULTRET, 0);

	// handle errors caused by the lua script
	if (res != LUA_OK)
	{
		print_error(state);
		lua_close(state);
		return;
	}

	// finally close the lua state
	lua_close(state);
}

int main(int argc, char *argv[])
{
	if (argc <= 1)
	{
		puts("Usage: run lua file(s)");
		puts("Loads and executes Lua programs.");
		return 1;
	}

	// execute all lua scripts given in the cmd line as arguments
	for (int n = 1; n < argc; ++n)
	{
		execute_script(argv[n]);
	}

	return 0;
}