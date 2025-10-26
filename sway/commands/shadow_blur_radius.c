#include "sway/commands.h"
#include "sway/config.h"
#include "sway/tree/arrange.h"

struct cmd_results *cmd_shadow_blur_radius(int argc, char **argv) {
	struct cmd_results *error = checkarg(argc, "shadow_blur_radius", EXPECTED_AT_LEAST, 1);

	if (error) {
		return error;
	}

	char *inv;
	int value = strtol(argv[0], &inv, 10);
	if (*inv != '\0' || value < 0 || value > 99) {
		return cmd_results_new(CMD_FAILURE, "Invalid size specified");
	}

	config->shadow_blur_sigma = value;
	arrange_root();

	return cmd_results_new(CMD_SUCCESS, NULL);
}
