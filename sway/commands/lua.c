#include "sway/commands.h"

struct cmd_results *cmd_lua(int argc, char **argv) {
	struct cmd_results *error = NULL;
	if ((error = checkarg(argc, "lua", EXPECTED_AT_LEAST, 1))) {
		return error;
	}
	int err = luaL_dofile(config->lua.state, argv[0]);
	if (err == LUA_OK) {
		return cmd_results_new(CMD_SUCCESS, NULL);
	}
	return cmd_results_new(CMD_FAILURE, "Error %d executing lua script %s", err, argv[0]);
}
