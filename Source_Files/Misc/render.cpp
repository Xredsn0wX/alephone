/*
RENDER.C
Thursday, September 8, 1994 1:58:20 PM  (Jason')

Friday, September 9, 1994 1:36:15 PM  (Jason')
	on the quads, in the sun.
Sunday, September 11, 1994 7:32:49 PM  (Jason')
	the clock on the 540 was wrong, yesterday was Saturday (not Friday).  on quads again, but
	back home now.  something will draw before i go to bed tonight.  dinner at the nile?
Tuesday, September 13, 1994 2:54:56 AM  (Jason')
	no fair!�� it�s still monday, really.  with the aid of some graphical debugging the clipping
	all works now and i�m trying to have the entire floor/ceiling thing going tonight (the nile
	was closed by the time i got around to taking a shower and heading out).
Friday, September 16, 1994 4:06:17 AM  (Jason')
	walls, floors and ceilings texture, wobble, etc.  contemplating objects ... maybe this will
	work after all.
Monday, September 19, 1994 11:03:49 AM  (Jason')
	unified xz_clip_vertical_polygon() and z_clip_horizontal_polygon() to get rid of the last
	known whitespace problem.  can�t wait to see what others i have.  objects now respect the
	clipping windows of all nodes they cross.
Monday, October 24, 1994 4:35:38 PM (Jason)
	fixed render sorting problem with objects of equal depth (i.e., parasitic objects). 
Tuesday, October 25, 1994 5:14:27 PM  (Jason')
	fixed object sort order with respect to nodes and height.
Wednesday, October 26, 1994 3:18:59 PM (Jason)
	fixed half of the object sort order back so that it worked: in order to correctly handle
	objects below the viewer projecting into higher polygons we need to sort objects inside
	nodes (to be draw after their walls and ceilings but before their floors).
Wednesday, November 2, 1994 3:49:57 PM (Jason)
	the bottom panel of short split sides sometimes takes on the ceiling lightsource.
Tuesday, November 8, 1994 5:29:12 PM  (Jason')
	implemented new transfer modes: _slide, _wander.  _render_effect_earthquake doesn�t work
	yet because the player can shake behind his own shape.
Thursday, December 15, 1994 12:15:55 AM  (Jason)
	the object depth sort order problem ocurrs with multiple objects in the same polygon,
	is some function of relative depths and overlap, and does not seem to involve objects at
	the same depth.  also, it seems to sort many further objects in front of a single closer
	one.
Monday, January 23, 1995 6:53:26 AM  (Jason')
	the way to fix the render object sorting problem is to ignore overlap and sort everything
	by depth, regardless.  imagine: two, far, non-overlapping objects; by the old algorithm their
	drawing order is irrelevant.  when a closer object which overlaps both of them is sorted, it
	only attempts to lie in front of the closest of the two (leaving the farthest one in an
	uncertain position).  unfortunately this will cause us to do unnecessary promotion and might
	look stupid.
Sunday, March 26, 1995 12:57:39 AM  (Jason')
	media modifications for marathon2; the object sort order problem still exists (the above
	solution indeed looked stupid).
Thursday, March 30, 1995 11:23:35 PM  (Jason')
	tried to fix object sort problem by attempting to assure that objects are always drawn in
	depth-order within a node.
Monday, June 5, 1995 8:37:42 AM  (Jason)
	blood and fire (baby).

Jan 30, 2000 (Loren Petrich):
	Added some typecasts
	Increased MAXIMUM_NODE_ALIASES to 32
	Added an "assert" for when DEBUG is off in aliases-building section in sort_render_tree().

Feb 1, 2000 (Loren Petrich):
	Added growable list of node aliases; replaced static-list node-alias code
	
Feb 4, 2000 (Loren Petrich):
	Changed halt() to assert(false) for better debugging

Feb 5, 2000 (Loren Petrich):
	Added growable lists of nodes and also clips for endpoints and lines.

Feb 6, 2000 (Loren Petrich):
	Doing initial allocations of the growable lists of various quantities as a defensive measure
	against memory leaks that seem to occur.

Feb 9, 2000 (Loren Petrich):
	Suppressed ambiguous-clip-flag debugging statement;
	it gets activated only for excessively distant objects

Feb 10, 2000 (Loren Petrich):
	Added dynamic-limits setting of MAXIMUM_RENDER_OBJECTS

Feb 14, 2000 (Loren Petrich):
	Added test for other-side polygon to LINE_IS_TRANSPARENT() check in next_polygon_along_line()

Feb 16, 2000 (Loren Petrich):
	Put in handling of overflow digits for the purpose of doing long distance correctly;
	also turning several horizontal-coordinate short integers into long ones.

Feb 17, 2000 (Loren Petrich):
	Made the sprites long-distance-friendly; there is a bug where they flip around when they
	go past half the world size, but that's a short-integer wraparound, and the relevant routine
	is in map.c.

Feb 18, 2000 (Loren Petrich):
	Added support for conditional display of weapons-in-hand; so as to support third-person
	as well as first-person view.

Feb 21, 2000 (Loren Petrich):
	Idiot-proofed next_polygon_along_line(), making it quit a loop if it searches a whole circle.

Feb 24, 2000 (Loren Petrich):
	Added animated-texture support

Mar 3, 2000 (Loren Petrich):
	Set view to normal in initialize_view_data();
	squashed persistent-extravision bug.

Mar 5, 2000 (Loren Petrich):
	Moved extravision-persistence bug fix out of this file.

Mar 9, 2000 (Loren Petrich):
	Sorted the nodes by polygon index in sort_render_tree() and used them to speed up searches
	for nodes with the same polygon index; maps with slowed-down visibility calculations,
	such as Desla, can become twice as fast.

Mar 12, 2000 (Loren Petrich):
	Added OpenGL support

Mar 14, 2000 (Loren Petrich):
	Modified data transmitted to OpenGL renderer; it's now collection/color/frame for
	both walls and sprites. Also added transmission of view data.

Mar 24, 2000 (Loren Petrich):
	Added landscape_yaw calculation; this is the yaw of the landscapes' left edges

Mar 30, 2000 (Loren Petrich):
	Inspired by Rhys Hill's work, I've created a second tree to contain the visibility nodes;
	in addition to their visibility tree, they have a polygon-sort tree.
	This tree is implemented by setting up some additional members of node_data
	for indicating its structure; there are members for polygon >, polygon <,
	and the next member of a chain that shares polygon-index values.
	As a result, the node-aliases list can now be abolished once and for all.

Jun 11, 2000 (Loren Petrich):
	Added support for see-through liquids; this requires several changes.
	The rendering of each map polygon had to be changed so that there would be a
	separate liquid surface; it would no longer replace the floor or the ceiling.
	Next, the inhabitant objects had to be done in two passes, one other side, and one view side.
	Also, whether there is void on the other side had to be indicated, so that
	waterfalls and the like may look right.

Jun 28, 2000 (Loren Petrich):
	Fixed Aaron Davies bug; if a polygon is completely below a liquid, it will not be rendered
	if the viewpoint is above the liquid; the bug was that it was not rendered if the viewpoint
	was below the liquid. This only happened if semitransparent liquid surfaces was turned off.

Jul 10, 2000 (Loren Petrich):
	Fixed liquid visibility bug in render_tree() that happens when liquid surfaces are not semitransparent;
	rendering is skipped if the viewpoint is under a liquid and the polygon is high and dry,
	or else if the viewpoint is above a liquid and the polygon is submerged.

Jul 17, 2000 (Loren Petrich):
	Suppressed view-effect resetting in initialize_view_data(),
	in order to make teleport view-stretching work correctly.

Aug 9, 2000 (Loren Petrich):
	Moved most of the rendering code here into separate files with these classes:
	
	RenderVisTreeClass --
		creates the visibility tree (which polygons can be seen from which other ones)
	RenderSortPolyClass --
		uses that tree to sort the polygons into appropriate depth order
	RenderPlaceObjsClass --
		finds which objects are visible and places them into appropriately sorted order
	RenderRasterize --
		handles the clipping of each object and requests those objects' rasterization
	
	Added a rasterizer class; currently, it does everything from the base class,
	though what it does will be moved into subclasses.

Sep 2, 2000 (Loren Petrich):
	Added some idiot-proofing, since the shapes accessor now returns NULL for nonexistent bitmaps

Nov 12, 2000 (Loren Petrich):
	Added automap reset before rendering

Nov 29, 2000 (Loren Petrich):
	Made teleport static/fold effect optional

Jan 17, 2001 (Loren Petrich):
	Added vertical flipping
*/


#ifdef QUICKDRAW_DEBUG
#include "macintosh_cseries.h"
#else
#include "cseries.h"
#endif

#include "map.h"
#include "render.h"
#include "interface.h"
#include "lightsource.h"
#include "media.h"
#include "weapons.h"

// LP additions
#include "dynamic_limits.h"
#include "AnimatedTextures.h"
#include "OGL_Render.h"

#ifdef QUICKDRAW_DEBUG
#include "shell.h"
extern WindowPtr screen_window;
#endif

#include <math.h>
#include <string.h>
#include <stdlib.h>

// LP additions for decomposition of this code:
#include "RenderVisTree.h"
#include "RenderSortPoly.h"
#include "RenderPlaceObjs.h"
#include "RenderRasterize.h"
#include "Rasterizer_SW.h"
#include "Rasterizer_OGL.h"

#ifdef env68k
#pragma segment render
#endif

/* use native alignment */
#if defined (powerc) || defined (__powerc)
#pragma options align=power
#endif

/*
//render transparent walls (if a bit is set or if the transparent texture is non-NULL?)
//use side lightsources instead of taking them from their polygons
//respect dark side bit (darken light intensity by k)
//fix solid/opaque endpoint confusion (solidity does not imply opacity)

there exists a problem where an object can overlap into a polygon which is clipped by something
	behind the object but that will clip the object because clip windows are subtractive; how
	is this solved?
it�s still possible to get ambiguous clip flags, usually in very narrow (e.g., 1 pixel) windows
the renderer has a maximum range beyond which it shits bricks yet which it allows to be exceeded
it�s still possible, especially in high-res full-screen, for points to end up (slightly) off
	the screen (usually discarding these has no noticable effect on the scene)
whitespace results when two adjacent polygons are clipped to different vertical windows.  this
	is not trivially solved with the current implementation, and may be acceptable (?)

//build_base_polygon_index_list() should discard lower polygons for objects above the viewer and
//	higher polygons for objects below the viewer because we certainly don�t sort objects
//	correctly in these cases
//in strange cases, objects are sorted out of order.  this seems to involve players in some way
//	(i.e., parasitic objects).
*/

/* ---------- constants */

#define EXPLOSION_EFFECT_RANGE (WORLD_ONE/12)

/* ---------- clip buffer */
// Not used for anything
#define CLIP_INDEX_BUFFER_SIZE 4096

vector<uint16> RenderFlagList;

// uint16 *render_flags;

// LP additions: decomposition of the rendering code into various objects

static RenderVisTreeClass RenderVisTree;			// Visibility-tree object
static RenderSortPolyClass RenderSortPoly;			// Polygon-sorting object
static RenderPlaceObjsClass RenderPlaceObjs;		// Object-placement object
static RenderRasterizerClass RenderRasterize;		// Clipping and rasterization class

static Rasterizer_SW_Class Rasterizer_SW;			// Software rasterizer
#ifdef HAVE_OPENGL
static Rasterizer_OGL_Class Rasterizer_OGL;			// OpenGL rasterizer
#endif


/* ---------- private prototypes */

static void update_view_data(struct view_data *view);
static void update_render_effect(struct view_data *view);
static void shake_view_origin(struct view_data *view, world_distance delta);

// LP: using rasterizer-object instead
static void render_viewer_sprite_layer(view_data *view, RasterizerClass *RasPtr);
// static void render_viewer_sprite_layer(struct view_data *view, struct bitmap_definition *destination);
static void position_sprite_axis(short *x0, short *x1, short scale_width, short screen_width,
	short positioning_mode, _fixed position, bool flip, world_distance world_left, world_distance world_right);


#ifdef QUICKDRAW_DEBUG
static void debug_flagged_points(flagged_world_point2d *points, short count);
static void debug_flagged_points3d(flagged_world_point3d *points, short count);
static void debug_vector(world_vector2d *v);
static void debug_x_line(world_distance x);
#endif

/* ---------- code */

void allocate_render_memory(
	void)
{
	assert(NUMBER_OF_RENDER_FLAGS<=16);
	RenderFlagList.resize(RENDER_FLAGS_BUFFER_SIZE);
	/*
	assert(MAXIMUM_LINES_PER_MAP<=RENDER_FLAGS_BUFFER_SIZE);
	assert(MAXIMUM_SIDES_PER_MAP<=RENDER_FLAGS_BUFFER_SIZE);
	assert(MAXIMUM_ENDPOINTS_PER_MAP<=RENDER_FLAGS_BUFFER_SIZE);
	assert(MAXIMUM_POLYGONS_PER_MAP<=RENDER_FLAGS_BUFFER_SIZE);
	render_flags= new uint16[RENDER_FLAGS_BUFFER_SIZE];
	assert(render_flags);
	*/
	
	// LP addition: check out pointer-arithmetic hack
	assert(sizeof(void *) == sizeof(POINTER_DATA));
	
	// LP change: do max allocation
	RenderVisTree.Resize(MAXIMUM_ENDPOINTS_PER_MAP,MAXIMUM_LINES_PER_MAP);
	RenderSortPoly.Resize(MAXIMUM_POLYGONS_PER_MAP);
	
	// LP change: set up pointers
	RenderSortPoly.RVPtr = &RenderVisTree;
	RenderPlaceObjs.RVPtr = &RenderVisTree;
	RenderPlaceObjs.RSPtr = &RenderSortPoly;
	RenderRasterize.RSPtr = &RenderSortPoly;
		
	return;
}

/* just in case anyone was wondering, standard_screen_width will usually be the same as
	screen_width.  the renderer assumes that the given field_of_view matches the standard
	width provided (so if the actual width provided is larger, you'll be able to see more;
	if it's smaller you'll be able to see less).  this allows the destination bitmap to not
	only grow and shrink while maintaining a constant aspect ratio, but to also change in
	geometry without effecting the image being projected onto it.  if you don't understand
	this, pass standard_width==width */
void initialize_view_data(
	struct view_data *view)
{
	double two_pi= 8.0*atan(1.0);
	double half_cone= view->field_of_view*(two_pi/360.0)/2;
	double adjusted_half_cone= View_FOV_FixHorizontalNotVertical() ?
		half_cone :
		asin(view->screen_width*sin(half_cone)/view->standard_screen_width);
	double world_to_screen;
	
	view->half_screen_width= view->screen_width/2;
	view->half_screen_height= view->screen_height/2;
	
	/* if there�s a round-off error in half_cone, we want to make the cone too big (so when we clip
		lines �to the edge of the screen� they�re actually off the screen, thus +1.0) */
	view->half_cone= (angle) (adjusted_half_cone*((double)NUMBER_OF_ANGLES)/two_pi+1.0);
	
	// LP change: find the adjusted yaw for the landscapes;
	// this is the effective yaw value for the left edge.
	// A landscape rotation can also be added if desired.
	view->landscape_yaw = view->yaw - view->half_cone;

	/* calculate world_to_screen; we could calculate this with standard_screen_width/2 and
		the old half_cone and get the same result */
	world_to_screen= view->half_screen_width/tan(adjusted_half_cone);
	view->world_to_screen_x= view->real_world_to_screen_x= (short) ((world_to_screen/view->horizontal_scale)+0.5);
	view->world_to_screen_y= view->real_world_to_screen_y= (short) ((world_to_screen/view->vertical_scale)+0.5);
	
	/* calculate the vertical cone angle; again, overflow instead of underflow when rounding */
	view->half_vertical_cone= (angle) (NUMBER_OF_ANGLES*atan(((double)view->half_screen_height*view->vertical_scale)/world_to_screen)/two_pi+1.0);

	/* calculate left edge vector */
	view->untransformed_left_edge.i= view->world_to_screen_x;
	view->untransformed_left_edge.j= - view->half_screen_width;

	/* calculate right edge vector (negative, so it clips in the right direction) */
	view->untransformed_right_edge.i= - view->world_to_screen_x;
	view->untransformed_right_edge.j= - view->half_screen_width;

	/* reset any active effects */
	// LP: this is now called in render_screen(), so we need to disable the initializing
	// view->effect= NONE;
	
	return;
}

/* origin,origin_polygon_index,yaw,pitch,roll,etc. have probably changed since last call */
void render_view(
	struct view_data *view,
	struct bitmap_definition *destination)
{
	(void) (destination);
	
	update_view_data(view);

	/* clear the render flags */
	objlist_clear(render_flags, RENDER_FLAGS_BUFFER_SIZE);

	ResetOverheadMap();
/*
#ifdef AUTOMAP_DEBUG
	memset(automap_lines, 0, (dynamic_world->line_count/8+((dynamic_world->line_count%8)?1:0)*sizeof(byte)));
	memset(automap_polygons, 0, (dynamic_world->polygon_count/8+((dynamic_world->polygon_count%8)?1:0)*sizeof(byte)));
#endif
*/
	
	if(view->terminal_mode_active)
	{
		/* Render the computer interface. */
		render_computer_interface(view);
	}
	else
	{
		// LP: the render objects have a pointer to the current view in them,
		// so that one can get rid of redundant references to it in them.
		
		// LP: now from the visibility-tree class
		/* build the render tree, regardless of map mode, so the automap updates while active */
		RenderVisTree.view = view;
		RenderVisTree.build_render_tree();
		
		if (view->overhead_map_active)
		{
			/* if the overhead map is active, render it */
			render_overhead_map(view);
		}
		else /* do something complicated and difficult to explain */
		{			
			// LP: now from the polygon-sorter class
			/* sort the render tree (so we have a depth-ordering of polygons) and accumulate
				clipping information for each polygon */
			RenderSortPoly.view = view;
			RenderSortPoly.sort_render_tree();
			
			// LP: now from the object-placement class
			/* build the render object list by looking at the sorted render tree */
			RenderPlaceObjs.view = view;
			RenderPlaceObjs.build_render_object_list();
			
			// LP addition: set the current rasterizer to whichever is appropriate here
			RasterizerClass *RasPtr;
#ifdef HAVE_OPENGL
			if (OGL_IsActive())
				RasPtr = &Rasterizer_OGL;
			else
			{
#endif
				// The software renderer needs this but the OpenGL one doesn't...
				Rasterizer_SW.screen = destination;
				RasPtr = &Rasterizer_SW;
#ifdef HAVE_OPENGL
			}
#endif
			
			// Set its view:
			RasPtr->SetView(*view);
			
			// Start rendering main view
			RasPtr->Begin();
			
			// LP: now from the clipping/rasterizer class
			/* render the object list, back to front, doing clipping on each surface before passing
				it to the texture-mapping code */
			RenderRasterize.view = view;
			RenderRasterize.RasPtr = RasPtr;
			RenderRasterize.render_tree();
			
			// LP: won't put this into a separate class
			/* render the player�s weapons, etc. */		
			render_viewer_sprite_layer(view, RasPtr);
			
			// Finish rendering main view
			RasPtr->End();
		}
	}

	return;
}

void start_render_effect(
	struct view_data *view,
	short effect)
{
	view->effect= effect;
	view->effect_phase= NONE;
	
	return;
}

/* ---------- private code */

static void update_view_data(
	struct view_data *view)
{
	angle theta;

	// LP change: doing all the FOV changes here:
	View_AdjustFOV(view->field_of_view,view->target_field_of_view);
	
	if (view->effect==NONE)
	{
		view->world_to_screen_x= view->real_world_to_screen_x;
		view->world_to_screen_y= view->real_world_to_screen_y;
	}
	else
	{
		update_render_effect(view);
	}
	
	view->untransformed_left_edge.i= view->world_to_screen_x;
	view->untransformed_right_edge.i= - view->world_to_screen_x;
	
	/* calculate world_to_screen_y*tan(pitch) */
	view->dtanpitch= (view->world_to_screen_y*sine_table[view->pitch])/cosine_table[view->pitch];

	/* calculate left cone vector */
	theta= NORMALIZE_ANGLE(view->yaw-view->half_cone);
	view->left_edge.i= cosine_table[theta], view->left_edge.j= sine_table[theta];
	
	/* calculate right cone vector */
	theta= NORMALIZE_ANGLE(view->yaw+view->half_cone);
	view->right_edge.i= cosine_table[theta], view->right_edge.j= sine_table[theta];
	
	/* calculate top cone vector (negative to clip the right direction) */
	view->top_edge.i= - view->world_to_screen_y;
	view->top_edge.j= - (view->half_screen_height + view->dtanpitch); /* ==k */

	/* calculate bottom cone vector */
	view->bottom_edge.i= view->world_to_screen_y;
	view->bottom_edge.j= - view->half_screen_height + view->dtanpitch; /* ==k */

	/* if we�re sitting on one of the endpoints in our origin polygon, move us back slightly (�1) into
		that polygon.  when we split rays we�re assuming that we�ll never pass through a given
		vertex in different directions (because if we do the tree becomes a graph) but when
		we start on a vertex this can happen.  this is a destructive modification of the origin. */
	{
		short i;
		struct polygon_data *polygon= get_polygon_data(view->origin_polygon_index);
		
		for (i= 0;i<polygon->vertex_count;++i)
		{
			struct world_point2d *vertex= &get_endpoint_data(polygon->endpoint_indexes[i])->vertex;
			
			if (vertex->x==view->origin.x && vertex->y==view->origin.y)
			{
				world_point2d *ccw_vertex= &get_endpoint_data(polygon->endpoint_indexes[WRAP_LOW(i, polygon->vertex_count-1)])->vertex;
				world_point2d *cw_vertex= &get_endpoint_data(polygon->endpoint_indexes[WRAP_HIGH(i, polygon->vertex_count-1)])->vertex;
				world_vector2d inset_vector;
				
				inset_vector.i= (ccw_vertex->x-vertex->x) + (cw_vertex->x-vertex->x);
				inset_vector.j= (ccw_vertex->y-vertex->y) + (cw_vertex->y-vertex->y);
				view->origin.x+= SGN(inset_vector.i);
				view->origin.y+= SGN(inset_vector.j);
				
				break;
			}
		}
		
		/* determine whether we are under or over the media boundary of our polygon; we will see all
			other media boundaries from this orientation (above or below) or fail to draw them. */
		if (polygon->media_index==NONE)
		{
			view->under_media_boundary= false;
		}
		else
		{
			struct media_data *media= get_media_data(polygon->media_index);
			
			// LP change: idiot-proofing
			if (media)
			{
				view->under_media_boundary= UNDER_MEDIA(media, view->origin.z);
				view->under_media_index= polygon->media_index;
			} else {
				view->under_media_boundary= false;
			}
		}
	}

	return;
}

static void update_render_effect(
	struct view_data *view)
{
	short effect= view->effect;
	short phase= view->effect_phase==NONE ? 0 : (view->effect_phase+view->ticks_elapsed);
	short period;

	view->effect_phase= phase;

	switch (effect)
	{
		// LP change: suppressed all the FOV changes
		case _render_effect_fold_in: case _render_effect_fold_out: period= TICKS_PER_SECOND/2; break;
		// case _render_effect_going_fisheye: case _render_effect_leaving_fisheye: period= TICKS_PER_SECOND; break;
		case _render_effect_explosion: period= TICKS_PER_SECOND; break;
		// LP additions:
		// case _render_effect_going_tunnel: case _render_effect_leaving_tunnel: period= TICKS_PER_SECOND; break;
		default:
			// LP change:
			assert(false);
			// halt();
	}
	
	if (phase>period)
	{
		view->effect= NONE;
	}
	else
	{
		switch (effect)
		{
			case _render_effect_explosion:
				shake_view_origin(view, EXPLOSION_EFFECT_RANGE - ((EXPLOSION_EFFECT_RANGE/2)*phase)/period);
				break;
			
			case _render_effect_fold_in:
				phase= period-phase;
			case _render_effect_fold_out:
				/* calculate world_to_screen based on phase */
				view->world_to_screen_x= view->real_world_to_screen_x + (4*view->real_world_to_screen_x*phase)/period;
				view->world_to_screen_y= view->real_world_to_screen_y - (view->real_world_to_screen_y*phase)/(period+period/4);
				break;
			// LP change: suppressed all the FOV changes here
			/*
			case _render_effect_leaving_fisheye:
				phase= period-phase;
				// LP addition: goes to normal instead of tunnel vision
				view->tunnel_vision_active = false;
			case _render_effect_going_fisheye:
			*/
				/* calculate field of view based on phase */
				/*
				// LP change: supporting tunnel vision
				if (view->tunnel_vision_active)
					view->field_of_view= TUNNEL_VISION_FIELD_OF_VIEW + ((EXTRAVISION_FIELD_OF_VIEW-TUNNEL_VISION_FIELD_OF_VIEW)*phase)/period;
				else
					view->field_of_view= NORMAL_FIELD_OF_VIEW + ((EXTRAVISION_FIELD_OF_VIEW-NORMAL_FIELD_OF_VIEW)*phase)/period;
				// view->field_of_view= NORMAL_FIELD_OF_VIEW + ((EXTRAVISION_FIELD_OF_VIEW-NORMAL_FIELD_OF_VIEW)*phase)/period;
				initialize_view_data(view);
				view->effect= effect;
				break;
			
			// LP additions:
			case _render_effect_leaving_tunnel:
				phase= period-phase;
			case _render_effect_going_tunnel:
			*/
				/* calculate field of view based on phase */
				/*
				view->field_of_view= NORMAL_FIELD_OF_VIEW + ((TUNNEL_VISION_FIELD_OF_VIEW-NORMAL_FIELD_OF_VIEW)*phase)/period;
				initialize_view_data(view);
				view->effect= effect;
				break;
			*/
		}
	}

	return;
}


/* ---------- transfer modes */

/* given a transfer mode and phase, cause whatever changes it should cause to a rectangle_definition
	structure */
void instantiate_rectangle_transfer_mode(
	view_data *view,
	rectangle_definition *rectangle,
	short transfer_mode,
	_fixed transfer_phase)
{
	switch (transfer_mode)
	{
		case _xfer_invisibility:
		case _xfer_subtle_invisibility:
			if (view->shading_mode!=_shading_infravision)
			{
				rectangle->transfer_mode= _tinted_transfer;
				rectangle->shading_tables= get_global_shading_table();
				rectangle->transfer_data= (transfer_mode==_xfer_invisibility) ? 0x000f : 0x0018;
				break;
			}
			/* if we have infravision, fall through to _textured_transfer (i see you...) */
		case _xfer_normal:
			rectangle->transfer_mode= _textured_transfer;
			break;
		
		case _xfer_static:
		case _xfer_50percent_static:
			rectangle->transfer_mode= _static_transfer;
			rectangle->transfer_data= (transfer_mode==_xfer_static) ? 0x0000 : 0x8000;
			break;

		case _xfer_fade_out_static:
			rectangle->transfer_mode= _static_transfer;
			rectangle->transfer_data= transfer_phase;
			break;
			
		case _xfer_pulsating_static:
			rectangle->transfer_mode= _static_transfer;
			rectangle->transfer_data= 0x8000+((0x6000*sine_table[FIXED_INTEGERAL_PART(transfer_phase*NUMBER_OF_ANGLES)])>>TRIG_SHIFT);
			break;

		case _xfer_fold_in:
			transfer_phase= FIXED_ONE-transfer_phase; /* do everything backwards */
		case _xfer_fold_out:
			if (View_DoStaticEffect())
			{
				short delta= FIXED_INTEGERAL_PART((((rectangle->x1-rectangle->x0)>>1)-1)*transfer_phase);
				
				rectangle->transfer_mode= _static_transfer;
				rectangle->transfer_data= (transfer_phase>>1);
				rectangle->x0+= delta;
				rectangle->x1-= delta;
			}
			else
				rectangle->transfer_mode= _textured_transfer;
			break;

#if 0		
		case _xfer_fade_out_to_black:
			rectangle->shading_tables= get_global_shading_table();
			if (transfer_phase<FIXED_ONE_HALF)
			{
				/* fade to black */
				rectangle->ambient_shade= (rectangle->ambient_shade*(transfer_phase-FIXED_ONE_HALF))>>(FIXED_FRACTIONAL_BITS-1);
				rectangle->transfer_mode= _textured_transfer;
			}
			else
			{
				/* vanish */
				rectangle->transfer_mode= _tinted_transfer;
				rectangle->transfer_data= 0x1f - ((0x1f*(FIXED_ONE_HALF-transfer_phase))>>(FIXED_FRACTIONAL_BITS-1));
			}
			break;
#endif
		
		// LP change: made an unrecognized mode act like normal
		default:
			rectangle->transfer_mode= _textured_transfer;
			// vhalt(csprintf(temporary, "rectangles don�t support render mode #%d", transfer_mode));
	}
	
	return;
}

/* given a transfer mode and phase, cause whatever changes it should cause to a polygon_definition
	structure (unfortunately we need to know whether this is a horizontal or vertical polygon) */
void instantiate_polygon_transfer_mode(
	struct view_data *view,
	struct polygon_definition *polygon,
	short transfer_mode,
	short transfer_phase,
	bool horizontal)
{
	world_distance x0, y0;
	world_distance vector_magnitude;
	short alternate_transfer_phase;

	polygon->transfer_mode= _textured_transfer;
	switch (transfer_mode)
	{
		case _xfer_fast_horizontal_slide:
		case _xfer_horizontal_slide:
		case _xfer_vertical_slide:
		case _xfer_fast_vertical_slide:
		case _xfer_wander:
		case _xfer_fast_wander:
			x0= y0= 0;
			transfer_phase= view->tick_count;
			switch (transfer_mode)
			{
				case _xfer_fast_horizontal_slide: transfer_phase<<= 1;
				case _xfer_horizontal_slide: x0= (transfer_phase<<2)&(WORLD_ONE-1); break;
				
				case _xfer_fast_vertical_slide: transfer_phase<<= 1;
				case _xfer_vertical_slide: y0= (transfer_phase<<2)&(WORLD_ONE-1); break;
				
				case _xfer_fast_wander: transfer_phase<<= 1;
				case _xfer_wander:
					alternate_transfer_phase= transfer_phase%(10*FULL_CIRCLE);
					transfer_phase= transfer_phase%(6*FULL_CIRCLE);
					x0= (cosine_table[NORMALIZE_ANGLE(alternate_transfer_phase)] +
						(cosine_table[NORMALIZE_ANGLE(2*alternate_transfer_phase)]>>1) +
						(cosine_table[NORMALIZE_ANGLE(5*alternate_transfer_phase)]>>1))>>(WORLD_FRACTIONAL_BITS-TRIG_SHIFT+2);
					y0= (sine_table[NORMALIZE_ANGLE(transfer_phase)] +
						(sine_table[NORMALIZE_ANGLE(2*transfer_phase)]>>1) +
						(sine_table[NORMALIZE_ANGLE(3*transfer_phase)]>>1))>>(WORLD_FRACTIONAL_BITS-TRIG_SHIFT+2);
					break;
			}
			if (horizontal)
			{
				polygon->origin.x+= x0;
				polygon->origin.y+= y0;
			}
			else
			{
				vector_magnitude= isqrt(polygon->vector.i*polygon->vector.i + polygon->vector.j*polygon->vector.j);
				polygon->origin.x+= (polygon->vector.i*x0)/vector_magnitude;
				polygon->origin.y+= (polygon->vector.j*x0)/vector_magnitude;
				polygon->origin.z-= y0;
			}
			break;
		
		case _xfer_pulsate:
		case _xfer_wobble:
		case _xfer_fast_wobble:
			if (transfer_mode==_xfer_fast_wobble) transfer_phase*= 15;
			transfer_phase&= WORLD_ONE/16-1;
			transfer_phase= (transfer_phase>=WORLD_ONE/32) ? (WORLD_ONE/32+WORLD_ONE/64 - transfer_phase) : (transfer_phase - WORLD_ONE/64);
			if (horizontal)
			{
				polygon->origin.z+= transfer_phase;
			}
			else
			{
				if (transfer_mode==_xfer_pulsate) /* translate .origin perpendicular to .vector */
				{
					world_vector2d offset;
					world_distance vector_magnitude= isqrt(polygon->vector.i*polygon->vector.i + polygon->vector.j*polygon->vector.j);
	
					offset.i= (polygon->vector.j*transfer_phase)/vector_magnitude;
					offset.j= (polygon->vector.i*transfer_phase)/vector_magnitude;
	
					polygon->origin.x+= offset.i;
					polygon->origin.y+= offset.j;
				}
				else /* ==_xfer_wobble, wobble .vector */
				{
					polygon->vector.i+= transfer_phase;
					polygon->vector.j+= transfer_phase;
				}
			}
			break;

		case _xfer_normal:
			break;
		
		case _xfer_smear:
			polygon->transfer_mode= _solid_transfer;
			break;
			
		case _xfer_static:
			polygon->transfer_mode= _static_transfer;
			polygon->transfer_data= 0x0000;
			break;
		
		case _xfer_landscape:
			polygon->transfer_mode= _big_landscaped_transfer;
			break;
//		case _xfer_big_landscape:
//			polygon->transfer_mode= _big_landscaped_transfer;
//			break;
			
		default:
			// LP change: made an unrecognized mode act like normal
			break;
			// vhalt(csprintf(temporary, "polygons don�t support render mode #%d", transfer_mode));
	}

	return;
}

/* ---------- viewer sprite layer (i.e., weapons) */

static void render_viewer_sprite_layer(view_data *view, RasterizerClass *RasPtr)
{
	rectangle_definition textured_rectangle;
	weapon_display_information display_data;
	shape_information_data *shape_information;
	short count;

	// LP change: bug out if weapons-in-hand are not to be displayed
	if (!view->show_weapons_in_hand) return;

	/* get_weapon_display_information() returns true if there is a weapon to be drawn.  it
		should initially be passed a count of zero.  it returns the weapon�s texture and
		enough information to draw it correctly. */
	count= 0;
	while (get_weapon_display_information(&count, &display_data))
	{
		/* fetch relevant shape data */
		shape_information= extended_get_shape_information(display_data.collection, display_data.low_level_shape_index);
		// Nonexistent frame: skip
		if (!shape_information) continue;
		
		if (shape_information->flags&_X_MIRRORED_BIT) display_data.flip_horizontal= !display_data.flip_horizontal;
		if (shape_information->flags&_Y_MIRRORED_BIT) display_data.flip_vertical= !display_data.flip_vertical;

		/* calculate shape rectangle */
		position_sprite_axis(&textured_rectangle.x0, &textured_rectangle.x1, view->screen_height, view->screen_width, display_data.horizontal_positioning_mode,
			display_data.horizontal_position, display_data.flip_horizontal, shape_information->world_left, shape_information->world_right);
		position_sprite_axis(&textured_rectangle.y0, &textured_rectangle.y1, view->screen_height, view->screen_height, display_data.vertical_positioning_mode,
			display_data.vertical_position, display_data.flip_vertical, -shape_information->world_top, -shape_information->world_bottom);

		/* set rectangle bitmap and shading table */
		extended_get_shape_bitmap_and_shading_table(display_data.collection, display_data.low_level_shape_index, &textured_rectangle.texture, &textured_rectangle.shading_tables, view->shading_mode);
		if (!textured_rectangle.texture) continue;
		
		// LP change: for the convenience of the OpenGL renderer
		textured_rectangle.ShapeDesc = BUILD_DESCRIPTOR(display_data.collection,display_data.low_level_shape_index);

		textured_rectangle.flags= 0;

		/* initialize clipping window to full screen */
		textured_rectangle.clip_left= 0;
		textured_rectangle.clip_right= view->screen_width;
		textured_rectangle.clip_top= 0;
		textured_rectangle.clip_bottom= view->screen_height;

		/* copy mirror flags */
		textured_rectangle.flip_horizontal= display_data.flip_horizontal;
		textured_rectangle.flip_vertical= display_data.flip_vertical;
		
		/* lighting: depth of zero in the camera�s polygon index */
		textured_rectangle.depth= 0;
		textured_rectangle.ambient_shade= get_light_intensity(get_polygon_data(view->origin_polygon_index)->floor_lightsource_index);
		textured_rectangle.ambient_shade= MAX(shape_information->minimum_light_intensity, textured_rectangle.ambient_shade);
		if (view->shading_mode==_shading_infravision) textured_rectangle.flags|= _SHADELESS_BIT;

		/* make the weapon reflect the owner�s transfer mode */
		instantiate_rectangle_transfer_mode(view, &textured_rectangle, display_data.transfer_mode, display_data.transfer_phase);
		
		/* and draw it */
		// LP: added OpenGL support
		RasPtr->texture_rectangle(textured_rectangle);
		/*
		if (!OGL_RenderSprite(textured_rectangle))
			texture_rectangle(&textured_rectangle, destination, view);
		*/
	}
	
	return;
}

static void position_sprite_axis(
	short *x0,
	short *x1,
	short scale_width,
	short screen_width,
	short positioning_mode,
	_fixed position,
	bool flip,
	world_distance world_left,
	world_distance world_right)
{
	short origin;

	/* if this shape is mirrored, reverse the left/right world coordinates */
	if (flip)
	{
		world_distance swap= world_left;
		world_left= -world_right, world_right= -swap;
	}
	
	switch (positioning_mode)
	{
		case _position_center:
			/* origin is the screen coordinate where the logical center of the shape will be drawn */
			origin= (screen_width*position)>>FIXED_FRACTIONAL_BITS;
			break;
		case _position_low:
		case _position_high:
			/* origin is in [0,WORLD_ONE] and represents the amount of the weapon visible off the side */
			origin= ((world_right-world_left)*position)>>FIXED_FRACTIONAL_BITS;
			break;
		
		default:
			// LP change:
			assert(false);
			// halt();
	}
	
	switch (positioning_mode)
	{
		case _position_high:
			*x0= screen_width - ((origin*scale_width)>>WORLD_FRACTIONAL_BITS);
			*x1= *x0 + (((world_right-world_left)*scale_width)>>WORLD_FRACTIONAL_BITS);
			break;
		case _position_low:
			*x1= ((origin*scale_width)>>WORLD_FRACTIONAL_BITS);
			*x0= *x1 - (((world_right-world_left)*scale_width)>>WORLD_FRACTIONAL_BITS);
			break;
		
		case _position_center:
			*x0= origin + ((world_left*scale_width)>>WORLD_FRACTIONAL_BITS);
			*x1= origin + ((world_right*scale_width)>>WORLD_FRACTIONAL_BITS);
			break;
		
		default:
			// LP change:
			assert(false);
			// halt();
	}
	
	return;
}

static void shake_view_origin(
	struct view_data *view,
	world_distance delta)
{
	world_point3d new_origin= view->origin;
	short half_delta= delta>>1;
	
	new_origin.x+= half_delta - ((delta*sine_table[NORMALIZE_ANGLE((view->tick_count&~3)*(7*FULL_CIRCLE))])>>TRIG_SHIFT);
	new_origin.y+= half_delta - ((delta*sine_table[NORMALIZE_ANGLE(((view->tick_count+5*TICKS_PER_SECOND)&~3)*(7*FULL_CIRCLE))])>>TRIG_SHIFT);
	new_origin.z+= half_delta - ((delta*sine_table[NORMALIZE_ANGLE(((view->tick_count+7*TICKS_PER_SECOND)&~3)*(7*FULL_CIRCLE))])>>TRIG_SHIFT);

	/* only use the new origin if we didn�t cross a polygon boundary */
	if (find_line_crossed_leaving_polygon(view->origin_polygon_index, (world_point2d *) &view->origin,
		(world_point2d *) &new_origin)==NONE)
	{
		view->origin= new_origin;
	}
	
	return;
}

// LP: begin no-compile
#if 0

/* ---------- mac-specific debugging calls */

#ifdef QUICKDRAW_DEBUG

#define SCALEF 5

static void debug_flagged_points(
	flagged_world_point2d *points,
	short count)
{
	short i;
	
	SetPort(screen_window);
	PenSize(1, 1);
	RGBForeColor(&rgb_black);
	RGBBackColor(&rgb_white);
	EraseRect(&screen_window->portRect);
	SetOrigin(-640/2, -480/2);
	MoveTo(-320, 0); LineTo(320, 0);
	MoveTo(0, 240); LineTo(0, -240);
	PenSize(2, 2);
	MoveTo(points[count-1].y>>SCALEF, - (points[count-1].x>>SCALEF));
	for (i=0;i<count;++i)
	{
		LineTo(points[i].y>>SCALEF, - (points[i].x>>SCALEF));
		psprintf(ptemporary, "%d", i);
		DrawString(temporary);
		MoveTo(points[i].y>>SCALEF, - (points[i].x>>SCALEF));
	}
}

static void debug_flagged_points3d(
	flagged_world_point3d *points,
	short count)
{
	short i;
	
	SetPort(screen_window);
	PenSize(1, 1);
	RGBForeColor(&rgb_black);
	RGBBackColor(&rgb_white);
	EraseRect(&screen_window->portRect);
	SetOrigin(-640/2, -480/2);
	MoveTo(-320, 0); LineTo(320, 0);
	MoveTo(0, 240); LineTo(0, -240);
	PenSize(2, 2);
	MoveTo(points[count-1].z>>SCALEF, - (points[count-1].x>>SCALEF));
	for (i=0;i<count;++i)
	{
		LineTo(points[i].z>>SCALEF, - (points[i].x>>SCALEF));
		psprintf(ptemporary, "%d", i);
		DrawString(temporary);
		MoveTo(points[i].z>>SCALEF, - (points[i].x>>SCALEF));
	}
}

static void debug_vector(
	world_vector2d *v)
{
	PenSize(1, 1);
	MoveTo(0, 0);
	LineTo(v->j, - v->i);
	MoveTo(0, 0);
	LineTo(- v->j, v->i);
	
	while (!Button()); while (Button());
}

static void debug_x_line(
	world_distance x)
{
	PenSize(1, 1);
	MoveTo(-320, - x>>SCALEF);
	LineTo(320, - x>>SCALEF);
	
	while (!Button()); while (Button());
}

#endif /* QUICKDRAW DEBUG */

			
#ifdef OBSOLETE
static void sort_render_object_into_tree(
	struct render_object_data *new_render_object, /* null-terminated linked list */
	struct sorted_node_data **base_nodes,
	short base_node_count)
{
	struct render_object_data *render_object, *last_new_render_object;
	struct render_object_data *deep_render_object= (struct render_object_data *) NULL;
	struct render_object_data *shallow_render_object= (struct render_object_data *) NULL;
	struct sorted_node_data *desired_node;
	short i;

	/* find the last render_object in the given list of new objects */
	for (last_new_render_object= new_render_object;last_new_render_object->next_object;
		last_new_render_object= last_new_render_object->next_object);

	/* find the two objects we must be lie between */	
	for (render_object= render_objects;render_object<new_render_object;++render_object)
	{
		/* if these two objects intersect... */
		if (render_object->rectangle.x1>new_render_object->rectangle.x0 && render_object->rectangle.x0<new_render_object->rectangle.x1 &&
			render_object->rectangle.y1>new_render_object->rectangle.y0 && render_object->rectangle.y0<new_render_object->rectangle.y1)
		{
			/* update our closest and farthest matches */
			if (render_object->rectangle.depth>new_render_object->rectangle.depth) /* found deeper intersecting object */
			{
				if (!deep_render_object || deep_render_object->rectangle.depth>render_object->rectangle.depth)
				{
					deep_render_object= render_object;
				}
			}
			else
			{
				if (render_object->rectangle.depth<new_render_object->rectangle.depth) /* found shallower intersecting object */
				{
					if (!shallow_render_object || shallow_render_object->rectangle.depth<=render_object->rectangle.depth)
					{
						shallow_render_object= render_object;
					}
				}
			}
		}
	}

	/* find the node we�d like to be in (ignoring polygons which were not lit up by the view cone) */
	desired_node= base_nodes[0];
	for (i= 1;i<base_node_count;++i) if (base_nodes[i]>desired_node) desired_node= base_nodes[i];
	assert(desired_node>=sorted_nodes && desired_node<next_sorted_node);
	
	/* adjust desired node based on the nodes of the deep and shallow render object; only
		one of deep_render_object and shallow_render_object will be non-null after this if
		block.  the current object must be sorted with respect to this non-null object inside
		the object list of the desired_node */
	if (shallow_render_object && desired_node>=shallow_render_object->node)
	{
		/* we tried to sort too close to the front of the node list */
		desired_node= shallow_render_object->node;
		deep_render_object= (struct render_object_data *) NULL;
	}
	else
	{
		if (deep_render_object && desired_node<=deep_render_object->node)
		{
			/* we tried to sort too close to the back of the node list */
			desired_node= deep_render_object->node;
			shallow_render_object= (struct render_object_data *) NULL;
		}
		else
		{
			deep_render_object= shallow_render_object= (struct render_object_data *) NULL;
		}
	}
	
	/* update the .node fields of all the objects we�re about to add to reflect their new
		location in the sorted node list */
	for (render_object= new_render_object;render_object;render_object= render_object->next_object)
	{
		render_object->node= desired_node;
	}
	
	if (deep_render_object)
	{
		/* sort after all objects as deep as the deep render object in this node */
		while (deep_render_object->next_object && deep_render_object->next_object->rectangle.depth==deep_render_object->rectangle.depth) deep_render_object= deep_render_object->next_object;
		last_new_render_object->next_object= deep_render_object->next_object;
		deep_render_object->next_object= new_render_object;
	}
	else
	{
		if (shallow_render_object)
		{
			struct render_object_data **reference;
			
			/* find the reference to the first object as shallow as the shallow render object in this node */
			for (reference= &desired_node->exterior_objects;
				(*reference)->rectangle.depth!=shallow_render_object->rectangle.depth && *reference;
				reference= &(*reference)->next_object);
			assert(*reference);
			
			/* sort before this object in the given node */
			last_new_render_object->next_object= *reference;
			*reference= new_render_object;
		}
		else
		{
			/* sort anywhere in the node */
			last_new_render_object->next_object= desired_node->exterior_objects;
			desired_node->exterior_objects= new_render_object;
		}
	}

	return;
}
#endif

// LP: end no-compile
#endif
