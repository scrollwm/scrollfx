#include <lua.h>
#include <lauxlib.h>
#include "log.h"
#include "sway/lua.h"
#include "sway/commands.h"
#include "sway/tree/view.h"
#include "sway/tree/container.h"
#include "sway/tree/workspace.h"

#if 0
static void stack_print(lua_State *L) {
	int top = lua_gettop(L);
	sway_log(SWAY_DEBUG, "Printing Lua stack...");
	for (int i = 1; i <= top; ++i) {
		sway_log(SWAY_DEBUG, "%d\t%s", i, luaL_typename(L, i));
		switch (lua_type(L, i)) {
		case LUA_TNUMBER:
			sway_log(SWAY_DEBUG, "%g", lua_tonumber(L, i));
			break;
		case LUA_TSTRING:
			sway_log(SWAY_DEBUG, "%s", lua_tostring(L, i));
			break;
		case LUA_TBOOLEAN:
			sway_log(SWAY_DEBUG, "%s", lua_toboolean(L, i) ? "true" : "false");
			break;
		case LUA_TNIL:
			sway_log(SWAY_DEBUG, "%s", "nil");
			break;
		default:
			sway_log(SWAY_DEBUG, "%p", lua_topointer(L, i));
			break;
		}
	}
}
#endif

// scroll.command(container|nil, command)
static int scroll_command(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc < 2) {
		lua_createtable(L, 0, 0);
		return 1;
	}
	struct sway_container *container = lua_isnil(L, 1) ? NULL : lua_touserdata(L, 1);
	const char *lua_cmd = luaL_checkstring(L, 2);
	char *cmd = strdup(lua_cmd);
	list_t *results = execute_command(cmd, NULL, container);
	lua_createtable(L, results->length, 0);
	for (int i = 0; i < results->length; ++i) {
		struct cmd_results *result = results->items[i];
		if (result->error) {
			lua_pushstring(L, result->error);
		} else {
			lua_pushinteger(L, result->status);
		}
		lua_rawseti(L, -2, i + 1);
	}
	free(cmd);
	return 1;
}

static struct sway_node *get_focused_node() {
	struct sway_seat *seat = input_manager_get_default_seat();
	struct sway_node *node = seat_get_focus_inactive(seat, &root->node);
	return node;
}

static int scroll_focused_view(lua_State *L) {
	struct sway_node *node = get_focused_node();
	struct sway_container *container = node->type == N_CONTAINER ?
		node->sway_container : NULL;

	if (container->view) {
		lua_pushlightuserdata(L, container->view);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int scroll_focused_container(lua_State *L) {
	struct sway_node *node = get_focused_node();
	struct sway_container *container = node->type == N_CONTAINER ?
		node->sway_container : NULL;

	if (container) {
		lua_pushlightuserdata(L, container);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int scroll_focused_workspace(lua_State *L) {
	struct sway_node *node = get_focused_node();
	struct sway_workspace *workspace = node->type == N_WORKSPACE ?
		node->sway_workspace : node->sway_container->pending.workspace;

	if (workspace) {
		lua_pushlightuserdata(L, workspace);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static bool find_urgent(struct sway_container *container, void *data) {
	return (container->view && view_is_urgent(container->view));
}

static int scroll_urgent_view(lua_State *L) {
	struct sway_container *container = root_find_container(find_urgent, NULL);
	if (container && container->view) {
		lua_pushlightuserdata(L, container->view);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int scroll_view_get_container(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc == 0) {
		lua_pushnil(L);
		return 1;
	}
	struct sway_view *view = lua_touserdata(L, -1);
	lua_pushlightuserdata(L, view ? view->container : NULL);
	return 1;
}

static int scroll_view_get_app_id(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc == 0) {
		lua_pushnil(L);
		return 1;
	}
	struct sway_view *view = lua_touserdata(L, -1);
	if (!view) {
		lua_pushnil(L);
		return 1;
	}
	const char *app_id = view_get_app_id(view);
	lua_pushstring(L, app_id);
	return 1;
}

static int scroll_view_get_title(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc == 0) {
		lua_pushnil(L);
		return 1;
	}
	struct sway_view *view = lua_touserdata(L, -1);
	if (!view) {
		lua_pushnil(L);
		return 1;
	}
	const char *app_id = view_get_title(view);
	lua_pushstring(L, app_id);
	return 1;
}

static int scroll_view_get_pid(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc == 0) {
		lua_pushnil(L);
		return 1;
	}
	struct sway_view *view = lua_touserdata(L, -1);
	if (!view) {
		lua_pushnil(L);
		return 1;
	}
	lua_pushinteger(L, view->pid);
	return 1;
}

static int scroll_view_get_urgent(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc == 0) {
		lua_pushboolean(L, 0);
		return 1;
	}
	struct sway_view *view = lua_touserdata(L, -1);
	if (!view) {
		lua_pushnil(L);
		return 1;
	}
	lua_pushboolean(L, view_is_urgent(view));
	return 1;
}

static int scroll_view_set_urgent(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc < 2) {
		return 0;
	}
	bool urgent = lua_toboolean(L, 2);
	struct sway_view *view = lua_touserdata(L, 1);
	if (view) {
		view_set_urgent(view, urgent);
	}
	return 0;
}

static int scroll_view_get_shell(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc == 0) {
		lua_pushnil(L);
		return 1;
	}
	struct sway_view *view = lua_touserdata(L, -1);
	if (!view) {
		lua_pushnil(L);
		return 1;
	}
	const char *shell = view_get_shell(view);
	lua_pushstring(L, shell);
	return 1;
}

static int scroll_view_close(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc == 0) {
		return 0;
	}
	struct sway_view *view = lua_touserdata(L, -1);
	if (view) {
		view_close(view);
	}
	return 0;
}

static int scroll_container_get_workspace(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc == 0) {
		lua_pushnil(L);
		return 1;
	}
	struct sway_container *container = lua_touserdata(L, -1);
	lua_pushlightuserdata(L, container ? container->pending.workspace : NULL);
	return 1;
}

static int scroll_container_get_marks(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc == 0) {
		lua_createtable(L, 0, 0);
		return 1;
	}
	struct sway_container *container = lua_touserdata(L, -1);
	if (!container) {
		lua_createtable(L, 0, 0);
		return 1;
	}
	lua_createtable(L, container->marks->length, 0);
	for (int i = 0; i < container->marks->length; ++i) {
		char *mark = container->marks->items[i];
		lua_pushstring(L, mark);
		lua_rawseti(L, -2, i + 1);
	}
	return 1;
}

static int scroll_container_get_floating(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc == 0) {
		lua_pushboolean(L, 0);
		return 1;
	}
	struct sway_container *container = lua_touserdata(L, -1);
	lua_pushboolean(L, container ? container_is_floating(container) : false);
	return 1;
}

static int scroll_container_get_opacity(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc == 0) {
		lua_pushnil(L);
		return 1;
	}
	struct sway_container *container = lua_touserdata(L, -1);
	if (!container) {
		lua_pushnil(L);
		return 1;
	}
	lua_pushnumber(L, container->alpha);
	return 1;
}

static int scroll_container_get_sticky(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc == 0) {
		lua_pushboolean(L, 0);
		return 1;
	}
	struct sway_container *container = lua_touserdata(L, -1);
	if (!container) {
		lua_pushboolean(L, 0);
		return 1;
	}
	lua_pushboolean(L, container->is_sticky);
	return 1;
}

static int scroll_container_get_views(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc == 0) {
		lua_createtable(L, 0, 0);
		return 1;
	}
	struct sway_container *container = lua_touserdata(L, -1);
	if (!container || container->node.type != N_CONTAINER) {
		lua_createtable(L, 0, 0);
		return 1;
	}
	if (container->view) {
		lua_createtable(L, 1, 0);
		lua_pushlightuserdata(L, container->view);
		lua_rawseti(L, -2, 1);
	} else {
		int len = container->pending.children->length;
		lua_createtable(L, len, 0);
		for (int i = 0; i < len; ++i) {
			struct sway_container *con = container->pending.children->items[i];
			lua_pushlightuserdata(L, con->view);
			lua_rawseti(L, -2, i + 1);
		}
	}
	return 1;
}

static int scroll_workspace_get_name(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc == 0) {
		lua_pushnil(L);
		return 1;
	}
	struct sway_workspace *workspace = lua_touserdata(L, -1);
	if (!workspace) {
		lua_pushnil(L);
		return 1;
	}
	lua_pushstring(L, workspace->name);
	return 1;
}

static int scroll_workspace_get_tiling(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc == 0) {
		lua_createtable(L, 0, 0);
		return 1;
	}
	struct sway_workspace *workspace = lua_touserdata(L, -1);
	if (!workspace || workspace->node.type != N_WORKSPACE ||
		workspace->tiling->length == 0) {
		lua_createtable(L, 0, 0);
		return 1;
	}
	lua_createtable(L, workspace->tiling->length, 0);
	for (int i = 0; i < workspace->tiling->length; ++i) {
		struct sway_container *container = workspace->tiling->items[i];
		lua_pushlightuserdata(L, container);
		lua_rawseti(L, -2, i + 1);
	}
	return 1;
}

static int scroll_workspace_get_floating(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc == 0) {
		lua_createtable(L, 0, 0);
		return 1;
	}
	struct sway_workspace *workspace = lua_touserdata(L, -1);
	if (!workspace || workspace->node.type != N_WORKSPACE) {
		lua_createtable(L, 0, 0);
		return 1;
	}
	lua_createtable(L, workspace->floating->length, 0);
	for (int i = 0; i < workspace->floating->length; ++i) {
		struct sway_container *container = workspace->floating->items[i];
		lua_pushlightuserdata(L, container);
		lua_rawseti(L, -2, i + 1);
	}
	return 1;
}

static int scroll_scratchpad_get_containers(lua_State *L) {
	lua_createtable(L, root->scratchpad->length, 0);
	for (int i = 0; i < root->scratchpad->length; ++i) {
		struct sway_container *container = root->scratchpad->items[i];
		lua_pushlightuserdata(L, container);
		lua_rawseti(L, -2, i + 1);
	}
	return 1;
}

// local id = scroll.add_callback(event, on_create, data)
static int scroll_add_callback(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc < 3) {
		lua_pushnil(L);
		return 1;
	}
	struct sway_lua_closure *closure = malloc(sizeof(struct sway_lua_closure));
	closure->cb_data = luaL_ref(L, LUA_REGISTRYINDEX);
	closure->cb_function = luaL_ref(L, LUA_REGISTRYINDEX);
	const char *event = luaL_checkstring(L, 1);
	if (strcmp(event, "view_map") == 0) {
		list_add(config->lua.cbs_view_map, closure);
	} else if (strcmp(event, "view_unmap") == 0) {
		list_add(config->lua.cbs_view_unmap, closure);
	} else if (strcmp(event, "view_urgent") == 0) {
		list_add(config->lua.cbs_view_urgent, closure);
	} else {
		free(closure);
		lua_pushnil(L);
		return 1;
	}
	lua_pushlightuserdata(L, closure);
	return 1;
}

//scroll.remove_callback(id)
static int scroll_remove_callback(lua_State *L) {
	int argc = lua_gettop(L);
	if (argc < 1) {
		return 0;
	}
	struct sway_lua_closure *closure = lua_touserdata(L, 1);
	for (int i = 0; i < config->lua.cbs_view_map->length; ++i) {
		if (config->lua.cbs_view_map->items[i] == closure) {
			list_del(config->lua.cbs_view_map, i);
			luaL_unref(config->lua.state, LUA_REGISTRYINDEX, closure->cb_function);
			luaL_unref(config->lua.state, LUA_REGISTRYINDEX, closure->cb_data);
			free(closure);
			return 0;
		}
	}
	for (int i = 0; i < config->lua.cbs_view_unmap->length; ++i) {
		if (config->lua.cbs_view_unmap->items[i] == closure) {
			list_del(config->lua.cbs_view_unmap, i);
			luaL_unref(config->lua.state, LUA_REGISTRYINDEX, closure->cb_function);
			luaL_unref(config->lua.state, LUA_REGISTRYINDEX, closure->cb_data);
			free(closure);
			return 0;
		}
	}
	for (int i = 0; i < config->lua.cbs_view_urgent->length; ++i) {
		if (config->lua.cbs_view_urgent->items[i] == closure) {
			list_del(config->lua.cbs_view_urgent, i);
			luaL_unref(config->lua.state, LUA_REGISTRYINDEX, closure->cb_function);
			luaL_unref(config->lua.state, LUA_REGISTRYINDEX, closure->cb_data);
			free(closure);
			return 0;
		}
	}
	return 0;
}

// Module functions
static luaL_Reg const scroll_lib[] = {
	{ "command", scroll_command },
	{ "focused_view", scroll_focused_view },
	{ "focused_container", scroll_focused_container },
	{ "focused_workspace", scroll_focused_workspace },
	{ "urgent_view", scroll_urgent_view },
	{ "view_get_container", scroll_view_get_container },
	{ "view_get_app_id", scroll_view_get_app_id },
	{ "view_get_title", scroll_view_get_title },
	{ "view_get_pid", scroll_view_get_pid },
	{ "view_get_urgent", scroll_view_get_urgent },
	{ "view_set_urgent", scroll_view_set_urgent },
	{ "view_get_shell", scroll_view_get_shell },
	{ "view_close", scroll_view_close },
	{ "container_get_workspace", scroll_container_get_workspace },
	{ "container_get_marks", scroll_container_get_marks },
	{ "container_get_floating", scroll_container_get_floating },
	{ "container_get_opacity", scroll_container_get_opacity },
	{ "container_get_sticky", scroll_container_get_sticky },
	{ "container_get_views", scroll_container_get_views },
	{ "workspace_get_name", scroll_workspace_get_name },
	{ "workspace_get_tiling", scroll_workspace_get_tiling },
	{ "workspace_get_floating", scroll_workspace_get_floating },
	{ "scratchpad_get_containers", scroll_scratchpad_get_containers },
	{ "add_callback", scroll_add_callback },
	{ "remove_callback", scroll_remove_callback },
	{ NULL, NULL }
};

// Module Loader
int luaopen_scroll(lua_State *L) {
	luaL_newlib(L, scroll_lib);
	return 1;
}
