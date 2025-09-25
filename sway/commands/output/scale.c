#include <strings.h>
#include "sway/commands.h"
#include "sway/config.h"

struct cmd_results *output_cmd_scale(int argc, char **argv) {
	if (!config->handler_context.output_config) {
		return cmd_results_new(CMD_FAILURE, "Missing output config");
	}
	if (!argc) {
		return cmd_results_new(CMD_INVALID, "Missing scale argument.");
	}

	char *end;
	config->handler_context.output_config->scale = strtof(*argv, &end);
	if (*end) {
		return cmd_results_new(CMD_INVALID, "Invalid scale.");
	}

	int nargs = 1;
	if (argc > 1) {
		if (strcasecmp(argv[1], "force") == 0) {
			++nargs;
			config->handler_context.output_config->scale_force = true;
		} else {
			return cmd_results_new(CMD_INVALID, "Invalid extra parameter: scale <number> [force].");
		}
	}

	config->handler_context.leftovers.argc = argc - nargs;
	config->handler_context.leftovers.argv = argv + nargs;
	return NULL;
}
