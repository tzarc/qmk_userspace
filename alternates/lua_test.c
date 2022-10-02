// Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-3.0-or-later

//----------------------------------------------------------
// Lua

#ifdef LUA_ENABLE
#    include <quantum.h>
#    include <lua.h>
#    include <lualib.h>
#    include <lauxlib.h>

lua_State *L                 = 0;
bool       lua_test_executed = false;

static int dprint_wrapper(lua_State *L) {
    const char *arg = luaL_checkstring(L, 1); // first arg is what we want to print
    (void)arg;
    dprintf("%s\n", arg);
    return 0;
}

void test_lua(void) {
    if (!lua_test_executed && timer_read32() > 15000) {
        lua_test_executed = true;

        L = luaL_newstate();
        luaL_openlibs(L);

        lua_newtable(L);                                             // new table
        lua_pushnumber(L, 1);                                        // table index
        lua_pushstring(L, "This is a test from executing lua code"); // value
        lua_rawset(L, -3);                                           // set tbl[1]='This is a test from executing lua code'

        // Set the "blah" global table to the newly-created table
        lua_setglobal(L, "blah");

        lua_pushcfunction(L, &dprint_wrapper);
        lua_setglobal(L, "dprint");

        // now we can use blah[1] == 'This is a test from executing lua code'

        const char *code = "dprint(blah[1])"; // should debug print "This is a test from executing lua code" in QMK Toolbox
        if (luaL_loadstring(L, code) == LUA_OK) {
            if (lua_pcall(L, 0, 1, 0) == LUA_OK) {
                lua_pop(L, lua_gettop(L));
            } else {
                dprint("Failed lua_pcall\n");
            }
        } else {
            dprint("Failed luaL_loadstring\n");
        }

        lua_close(L);
    }
}
#endif // LUA_ENABLE
