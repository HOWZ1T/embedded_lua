#include "lua.h"

//extern C if in cpp env so that name mangling doesn't mess up function names
#ifdef __cplusplus
extern "C" {
#endif

void print_error(lua_State* L)
{
	// the error message is on top of the stack
	// fetch it, print it and then pop it off the stack
	const char* msg = lua_tostring(L, -1);
	puts(msg);
	lua_pop(L, 1);
}

// dumps the lua state's global table
void dump_glob_tbl(lua_State* L)
{
	lua_pushglobaltable(L); // push the global table onto the stack
	lua_pushnil(L);  // first key on stack

	while (lua_next(L, -2) != 0)
	{
		printf("%s\n", lua_tostring(L, -2));
		lua_pop(L, 1);
	}
	lua_pop(L, 1); // remove global table
}

// exposes the given function to the lua environment
void expose_func(lua_State* L, lua_CFunction c_func, char* func_name)
{
	lua_pushcfunction(L, c_func);
	lua_setglobal(L, func_name);
}

// int glb, bool for exposing the mod to the global table
void expose_lua_mod(lua_State* L, const char* modname, lua_CFunction openf, int glb, int whitelist_size, const char* const whitelist[])
{
	// helpful links
	//http://lua-users.org/wiki/SandBoxes
	//https://stackoverflow.com/questions/10108933/how-to-remove-particular-functions-from-os-library-without-editing-the-lua-heade

	// load the library
	luaL_requiref(L, modname, openf, glb);

	// get the library global
	lua_getglobal(L, modname); // i: -1

	// generate blacklist of functions based on the given whitelist
	lua_pushnil(L);  // first key i: -1, library global now i: -2
	int buff_len = 256;
	int to_remove_index = 0;
	char** to_remove = calloc(buff_len, sizeof(char*));

	// checking calloc for errors
	if (!to_remove) perror("calloc failed in expose_lua_mod()"), exit(EXIT_FAILURE);

	// traverse the mod's table
	// -2 -> location of table on stack, it is at -2 and not -1 since we have pushed a nil onto the stack before hand
	while (lua_next(L, -2) != 0)
	{
		// get key (at index -2)
		char* fn_name = (char *) lua_tostring(L, -2);
		int is_allowed = 0;

		// check if in whitelist
		for (int i = 0; i < whitelist_size; i++)
		{
			if (strcmp(fn_name, whitelist[i]) == 0)
			{
				is_allowed = 1;
				break;
			}
		}

		// if not allowed, add to remove list
		if (is_allowed == 0)
		{
			to_remove[to_remove_index++] = fn_name;
		}

		lua_pop(L, 1); // freeing up value to allow key to move on to in the next lua_next call
	}

	// remove functions based on blacklist entries
	for (int i = 0; i < to_remove_index; i++)
	{
		printf("removed: %s, from mod: %s's table\n", to_remove[i], modname);
		lua_pushnil(L);
		lua_setfield(L, -2, to_remove[i]);
	}
	lua_pop(L, 1); // removing library global from stack

	// allocate null to write over data in to_remove to avoid messing with lua handled data
	for (int i = 0; i < buff_len; i++)
	{
		to_remove[i] = '\0';
	}

	// freeing up allocated data
	for (int i = 0; i < buff_len; i++)
	{
		free(to_remove[i]);
	}
	free(to_remove);
}

/* attempts to execute a given script located by it's filename in the given lua environment L
 * 
 * Returns int indicating success or errors:
 *  0 - indicates success
 * -1 - indicates that the lua script file was unable to be loaded
 * -2 - indicates that the lua script caused an error
 */
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

#ifdef __cplusplus
}
#endif