#ifndef _SWAY_LAYER_CRITERIA_H
#define _SWAY_LAYER_CRITERIA_H
#include <stdbool.h>

/**
 * Layer shell criteria for applying effects to specific layer surfaces
 */
struct layer_criteria {
	char *namespace;
	char *cmdlist;

	bool shadow_enabled;
	bool blur_enabled;
	bool blur_xray;
	bool blur_ignore_transparent;
	int corner_radius;
};

/**
 * Destroy a layer criteria structure
 */
void layer_criteria_destroy(struct layer_criteria *criteria);

/**
 * Add a new layer criteria with the given namespace and command list
 */
struct layer_criteria *layer_criteria_add(char *namespace, char *cmdlist);

/**
 * Get the matching criteria for a specified namespace
 */
struct layer_criteria *layer_criteria_for_namespace(char *namespace);

#endif
