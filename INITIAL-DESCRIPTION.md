For historical purposes;
1) design decision: disable effects in overview mode entirely to avoid any scaling complexity
2) known next step: all wlr_renderer calls need conversion to fx_renderer as explained below:
```

// Scroll uses standard wlroots renderer
output_render(output, &output->wlr_output->pending, &damage);

// SceneFX REPLACES the renderer with FX renderer
struct fx_renderer *renderer = fx_renderer_create(backend);
// This is a fundamental change to the rendering pipeline
this affects OUTPUT rendering, not just effects
```
3) regarding scenefx we do not workspace-specific effect configs
4) scene node lifecycles: 
```
// Scroll has node lifecycle hooks
node->old_parent;  // For fullscreen state tracking

// SceneFX creates auxiliary nodes (shadow, blur)
// These auxiliary nodes don't participate in Scroll's lifecycle
```
POTENTIAL ISSUE:
When Scroll moves/reparents nodes, effect nodes might get orphaned
Need to ensure effect nodes follow their parent nodes

## overall understanding:
In the context of merging scenefx with the changes made on scroll window manager and extracted them (aka scene-scroll)
The renderer and scene are completely separate systems:
```
// SCENE GRAPH (what Scroll modified)
// Manages: spatial hierarchy, positions, transformations, visibility
sway_scene_node_set_position();  // WHERE to draw
sway_scene_buffer_set_transform(); // HOW to transform
sway_scene_node_set_enabled();  // WHETHER to draw

// RENDERER (what SceneFX replaces)  
// Manages: actual drawing, shaders, effects, GPU operations
wlr_renderer_begin();  // Start drawing
wlr_render_texture();  // HOW to draw (shaders, effects)
wlr_renderer_end();    // Finish drawing
```
Once we combine the two into scenefx-scroll we will then update scroll-standalone to use scenefx rendererr instead of wlroots renderer.

