#include "sway/commands.h"
#include "sway/config.h"
#include "sway/tree/arrange.h"
#include "util.h"

struct cmd_results *cmd_titlebar_separator(int argc, char **argv) {
	struct cmd_results *error = checkarg(argc, "titlebar_separator", EXPECTED_AT_LEAST, 1);

	if (error) {
		return error;
	}

	config->titlebar_separator = parse_boolean(argv[0], true);

	arrange_root();
	return cmd_results_new(CMD_SUCCESS, NULL);
}
