#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "sway/config.h"
#include "sway/layer_criteria.h"
#include "sway/layers.h"
#include "list.h"

/**
 * Destroy a layer criteria and free its memory
 */
void layer_criteria_destroy(struct layer_criteria *criteria) {
	if (!criteria) {
		return;
	}

	free(criteria->namespace);
	free(criteria->cmdlist);
	free(criteria);
}

/**
 * Add a new layer criteria for the given namespace
 * Returns the created criteria or NULL on failure
 */
struct layer_criteria *layer_criteria_add(char *namespace, char *cmdlist) {
	struct layer_criteria *criteria = calloc(1, sizeof(struct layer_criteria));
	if (!criteria) {
		sway_log(SWAY_ERROR, "Failed to allocate layer criteria");
		return NULL;
	}

	criteria->namespace = strdup(namespace);
	criteria->cmdlist = cmdlist ? strdup(cmdlist) : NULL;

	if (!criteria->namespace) {
		sway_log(SWAY_ERROR, "Failed to duplicate namespace string");
		layer_criteria_destroy(criteria);
		return NULL;
	}

	// Initialize effect properties to defaults
	criteria->shadow_enabled = false;
	criteria->blur_enabled = false;
	criteria->blur_xray = false;
	criteria->blur_ignore_transparent = false;
	criteria->corner_radius = 0;

	// Add to config layer criteria list
	if (!config->layer_criteria) {
		config->layer_criteria = create_list();
		if (!config->layer_criteria) {
			sway_log(SWAY_ERROR, "Failed to create layer criteria list");
			layer_criteria_destroy(criteria);
			return NULL;
		}
	}

	list_add(config->layer_criteria, criteria);

	return criteria;
}

/**
 * Find layer criteria for a given namespace
 * Returns the matching criteria or NULL if not found
 */
struct layer_criteria *layer_criteria_for_namespace(char *namespace) {
	if (!config->layer_criteria || !namespace) {
		return NULL;
	}

	for (int i = 0; i < config->layer_criteria->length; ++i) {
		struct layer_criteria *criteria = config->layer_criteria->items[i];
		if (criteria && criteria->namespace &&
				strcmp(criteria->namespace, namespace) == 0) {
			return criteria;
		}
	}

	return NULL;
}

/**
 * Apply layer criteria effects to a layer surface
 */
void layer_apply_criteria(struct sway_layer_surface *surface,
		struct layer_criteria *criteria) {
	if (!surface) {
		return;
	}

	if (criteria) {
		surface->shadow_enabled = criteria->shadow_enabled;
		surface->blur_enabled = criteria->blur_enabled;
		surface->blur_xray = criteria->blur_xray;
		surface->blur_ignore_transparent = criteria->blur_ignore_transparent;
		surface->corner_radius = criteria->corner_radius;
	} else {
		// Reset to defaults if no criteria found
		surface->shadow_enabled = false;
		surface->blur_enabled = false;
		surface->blur_xray = false;
		surface->blur_ignore_transparent = false;
		surface->corner_radius = 0;
	}
}
