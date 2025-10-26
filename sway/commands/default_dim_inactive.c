#include <stdlib.h>
#include "sway/commands.h"
#include "sway/config.h"
#include "sway/tree/arrange.h"

struct cmd_results *cmd_default_dim_inactive(int argc, char **argv) {
	struct cmd_results *error = checkarg(argc, "default_dim_inactive", EXPECTED_AT_LEAST, 1);

	if (error) {
		return error;
	}

	char *err;
	float val = strtof(argv[0], &err);
	if (*err || val < 0.0f || val > 1.0f) {
		return cmd_results_new(CMD_FAILURE,
				"default_dim_inactive float invalid. Must be between 0.0 and 1.0");
	}

	config->default_dim_inactive = val;

	if (config->active) {
		arrange_root();
	}

	return cmd_results_new(CMD_SUCCESS, NULL);
}
