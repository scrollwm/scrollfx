#include "sway/commands.h"
#include "sway/config.h"
#include "util.h"

struct cmd_results *cmd_cursor_shake_magnify(int argc, char **argv) {
	struct cmd_results *error =
		checkarg(argc, "cursor_shake_magnify", EXPECTED_EQUAL_TO, 1);
	if (error) {
		return error;
	}

	config->cursor_shake_magnify = parse_boolean(argv[0], config->cursor_shake_magnify);

	return cmd_results_new(CMD_SUCCESS, NULL);
}
