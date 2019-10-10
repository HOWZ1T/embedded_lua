#ifdef __cplusplus
	#include "lua.hpp"
#else
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
#endif

#include "callback.h"

//so that name mangling doesn't mess up function names
#ifdef __cplusplus
extern "C" {
#endif

lua_Integer fib(lua_Integer n, lua_Integer a, lua_Integer b)
{
	if (n == 0)
	{
		return a;
	}

	if (n == 1)
	{
		return b;
	}

	return fib(n - 1, b, a + b);
}

int c_fib(lua_State* L)
{
	// each function has it's own function stack
	// the num of function args will be on top of the stack
	if (lua_gettop(L) != 1)
	{
		return luaL_error(L, "expecting exactly 1 argument");
	}

	// luaL_check* checks the argument's type and raises an error if it doesn't match
	lua_Integer n = luaL_checkinteger(L, 1); // 1 = first argument

	if (n < 0)
	{
		return luaL_error(L, "argument n must be positive!");
	}

	lua_pushinteger(L, fib(n, 0, 1)); // pushing result to lua stack as an lua integer

	return 1; // number of results pushed to the stack
}

int c_print(lua_State* L)
{
	// each function has it's own function stack
	// the num of function args will be on top of the stack
	if (lua_gettop(L) != 1)
	{
		return luaL_error(L, "expecting exactly 1 argument");
	}

	// luaL_check* checks the argument's type and raises an error if it doesn't match
	const char* str = luaL_checkstring(L, 1); // 1 = first argument
	const char* ext = "\n";
	char* out;

	// creating the fmt str for printf
	int buff_size = strlen(str) + strlen(ext) + 1;
	out = malloc(buff_size);
	if (out == NULL)
	{
		return luaL_error(L, "print(msg) failed to allocate memory for msg!");
	}

	strcpy_s(out, buff_size, str);
	strcat_s(out, buff_size, ext);
	printf(out);
	free(out);

	return 0; // number of results pushed to the stack
}

#ifdef __cplusplus
}
#endif