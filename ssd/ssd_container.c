/* ssd_container.c - Container widget
 * (requires agg)
 *
 * LICENSE:
 *
 *   Copyright 2006 Ehud Shabtai
 *
 *   This file is part of RoadMap.
 *
 *   RoadMap is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   RoadMap is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with RoadMap; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * SYNOPSYS:
 *
 *   See ssd_widget.h
 */

#include <string.h>
#include <stdlib.h>


#include "roadmap.h"
#include "roadmap_canvas.h"
#include "roadmap_sound.h"
#include "roadmap_lang.h"
#include "roadmap_res.h"
#include "roadmap_border.h"
#include "roadmap_skin.h"

#include "ssd_widget.h"
#include "ssd_text.h"
#include "ssd_button.h"
#include "ssd_dialog.h"


#include "roadmap_keyboard.h"

#include "ssd_container.h"

static void draw (SsdWidget widget, RoadMapGuiRect *rect, int flags);
static int initialized;
static RoadMapPen bg;
static RoadMapPen border;
static const char *default_fg = "#000000";

static const char *default_bg = "#ffffff";

static void init_containers (void) {
   bg = roadmap_canvas_create_pen ("container_bg");
   roadmap_canvas_set_foreground (default_bg);

   border = roadmap_canvas_create_pen ("container_border");
   roadmap_canvas_set_foreground ("#e1e1e1");
   roadmap_canvas_set_thickness (1);

   initialized = 1;
}


static RoadMapImage create_title_bar_image (RoadMapImage header_image, RoadMapGuiRect *rect) {

   RoadMapImage image;
   int width = roadmap_canvas_width ();
   int num_images;
   int image_width;
   RoadMapGuiPoint point;
   int i;

   image = roadmap_canvas_new_image (width,
               roadmap_canvas_image_height(header_image));
   image_width = roadmap_canvas_image_width(header_image);

   num_images = width / image_width;

   num_images = (rect->maxx/image_width)+1;
   if (header_image){
      for (i=0;i<=num_images;i++){
             point.x = i * image_width;
             point.y = 0;
             roadmap_canvas_copy_image (image, &point, NULL, header_image, IMAGE_NORMAL);
       }
    }

   return image;
}

void draw_title_bar(RoadMapGuiRect *rect){

	static RoadMapImage header_image;
	RoadMapGuiPoint point;
   static RoadMapImage cached_header_image;
   static int cached_width = -1;


	rect->miny -= 1;
   if (!header_image)
      header_image =    roadmap_res_get(
                                 RES_BITMAP,
                                 RES_SKIN|RES_LOCK,
                                 "header");

   if ((cached_header_image == NULL) || (cached_width != roadmap_canvas_width())){
      if (cached_header_image != NULL)
         roadmap_canvas_free_image(cached_header_image);
      cached_header_image = create_title_bar_image  (header_image, rect);
      cached_width = roadmap_canvas_width();
   }

   point.x = rect->minx;
   point.y = rect->miny;
   roadmap_canvas_draw_image (cached_header_image, &point, 0, IMAGE_NORMAL);
}

static void draw_bg(RoadMapGuiRect *rect){
	static RoadMapImage bg_image, bg_image_night;
	RoadMapGuiPoint point;
	int   width;
	int   num_images;
	int   i;
	RoadMapImage image;

		if (!bg_image)
			bg_image = 	roadmap_res_get(
                                    RES_BITMAP,
                                    RES_SKIN|RES_LOCK,
                                    "container_bg");
		image = bg_image;
      if (!bg_image_night)
         bg_image_night =  roadmap_res_get(
                                    RES_BITMAP,
                                    RES_SKIN|RES_LOCK,
                                    "container_bg_night");
      if (roadmap_skin_state())
         image = bg_image_night;
      else
         image = bg_image;

		if (image){
	    	width = roadmap_canvas_image_width(image);
			num_images = rect->maxx / width + 1;
			point.y = rect->miny;

			for (i=0;i<num_images;i++){
				point.x = i*width;
				roadmap_canvas_draw_image (image, &point, 0, IMAGE_NORMAL);
			}
		}
}

static void draw_title (SsdWidget widget, RoadMapGuiRect *rect, int flags){
	if (!flags & SSD_GET_SIZE)
		draw_title_bar(rect);
	draw(widget, rect, flags);
}



static void draw_text_box(SsdWidget widget, RoadMapGuiRect *rect){

	int i;
	int num_images;
   static RoadMapImage   left;
   static RoadMapImage   middle;
   static RoadMapImage   right;
   static RoadMapImage   left_s;
   static RoadMapImage   middle_s;
   static RoadMapImage   right_s;
   RoadMapImage   image_left;
   RoadMapImage   image_middle;
   RoadMapImage   image_right;
   RoadMapGuiPoint point;
	int   width_left, width_right, width_middle;

	if (!left)
			left = 	roadmap_res_get(
                                    RES_BITMAP,
                                    RES_SKIN,
                                    "Txtbox_left");

	if (!right)
			right = 	roadmap_res_get(
                                    RES_BITMAP,
                                    RES_SKIN,
                                    "Txtbox_right");

	if (!middle)
			middle = 	roadmap_res_get(
                                    RES_BITMAP,
                                    RES_SKIN,
                                    "Txtbox_middle");

   if (!left_s)
         left_s =   roadmap_res_get(
                                    RES_BITMAP,
                                    RES_SKIN,
                                    "Txtbox_left_selected");

   if (!right_s)
         right_s =  roadmap_res_get(
                                    RES_BITMAP,
                                    RES_SKIN,
                                    "Txtbox_right_selected");

   if (!middle_s)
         middle_s =    roadmap_res_get(
                                    RES_BITMAP,
                                    RES_SKIN,
                                    "Txtbox_middle_selected");

	if ( widget->in_focus && widget->focus_highlight ){
	   image_left = left_s;
	   image_right = right_s;
	   image_middle = middle_s;
	}
	else{
      image_left = left;
      image_right = right;
      image_middle = middle;
	}
   width_left = roadmap_canvas_image_width(image_left);
	width_right = roadmap_canvas_image_width(image_right);
	width_middle = roadmap_canvas_image_width(image_middle);

	point.x = rect->minx;
	point.y = rect->miny;
	roadmap_canvas_draw_image (image_left, &point, 0, IMAGE_NORMAL);

	num_images = (rect->maxx - rect->minx - width_right - width_left)/width_middle;
	for (i = 0; i < num_images; i++){
		point.x = +rect-> minx + width_right + i * width_middle;
		point.y = rect->miny;
		roadmap_canvas_draw_image (image_middle, &point, 0, IMAGE_NORMAL);
	}

	point.x = rect->maxx - width_right;
	point.y = rect->miny;
	roadmap_canvas_draw_image (image_right, &point, 0, IMAGE_NORMAL);

}

static void draw (SsdWidget widget, RoadMapGuiRect *rect, int flags) {


   RoadMapGuiPoint points[5];
   int count = 5;
   const char* background = widget->bg_color;


//   if (((widget->flags & SSD_ROUNDED_CORNERS) && !(widget->flags & SSD_DIALOG_FLOAT)) && !(widget->flags & SSD_ROUNDED_WHITE)){
//      widget->flags &= ~SSD_ROUNDED_CORNERS;
//      ssd_widget_set_color(widget, "#000000","#ffffff");
//   }

   if (!(flags & SSD_GET_SIZE)) {


      if (widget->in_focus && widget->focus_highlight )
		 background = "#b0d504";
      else if (widget->background_focus)
         background = "#8a8a8a";

      if (background) {
      	if (widget->flags & SSD_ROUNDED_CORNERS){
      		RoadMapGuiRect erase_rect;
			erase_rect.minx = rect->minx + SSD_ROUNDED_CORNER_WIDTH;
      		erase_rect.miny = rect->miny + SSD_ROUNDED_CORNER_HEIGHT;
      		erase_rect.maxx = rect->maxx - SSD_ROUNDED_CORNER_WIDTH;
      		erase_rect.maxy = rect->maxy - SSD_ROUNDED_CORNER_HEIGHT;

      		roadmap_canvas_select_pen (bg);
         	roadmap_canvas_set_foreground (background);

	        //roadmap_canvas_erase_area (&erase_rect);

  	  	 }
  	  	 else{
         	roadmap_canvas_select_pen (bg);
         	roadmap_canvas_set_foreground (background);

            if (widget->flags & SSD_CONTAINER_TXT_BOX){
      			rect->minx += 10;
      			rect->maxx -= 10;
            }
         	roadmap_canvas_erase_area (rect);
  	  	 }
      }

      if ((widget->flags & SSD_CONTAINER_TITLE) &&
    		  !(widget->flags & SSD_DIALOG_FLOAT) &&
    		  !(widget->flags & SSD_NO_BG_IMAGE ) && !(widget->flags & SSD_DIALOG_TRANSPARENT) )
      {
   			draw_bg(rect);
      }
   }



   if (widget->flags & SSD_CONTAINER_BORDER) {

      points[0].x = rect->minx + 1;
#ifndef TOUCH_SCREEN
      points[0].y = rect->miny + 1;
      points[1].y = rect->miny + 1;
      points[4].y = rect->miny + 1;
#else
	  points[0].y = rect->miny;
	  points[1].y = rect->miny;
      points[4].y = rect->miny ;
#endif
      points[1].x = rect->maxx - 0;
      points[2].x = rect->maxx - 0;
      points[2].y = rect->maxy - 0;
      points[3].x = rect->minx + 1;
      points[3].y = rect->maxy - 0;
      points[4].x = rect->minx + 1;


      if (!(flags & SSD_GET_SIZE)) {
         const char *color = default_fg;
	 RoadMapPosition *position = NULL;

         roadmap_canvas_select_pen (border);
         if (widget->fg_color) color = widget->fg_color;
         roadmap_canvas_set_foreground (color);

		 if (widget->flags & SSD_ROUNDED_CORNERS){
		 	int pointer_type = POINTER_NONE;
		 	int header_type = HEADER_NONE;
		 	int style = STYLE_NORMAL;

		 	if (widget->flags & SSD_HEADER_BLACK)
		 		header_type = HEADER_BLACK;

#ifdef TOUCH_SCREEN
			widget->flags &=  ~SSD_POINTER_MENU;
#endif
		 	if (widget->flags & SSD_POINTER_MENU){

		 		pointer_type = POINTER_MENU;
		 		points[2].y -= 17;
		 	}

		 	else if (widget->flags & SSD_POINTER_COMMENT){
		 		pointer_type = POINTER_COMMNET;
		 		points[2].y -= 7;
		 	}
		 	else if (widget->flags & SSD_POINTER_NONE){
		 		pointer_type = POINTER_NONE;
		 		if (!((widget->flags & SSD_ROUNDED_WHITE) || (widget->flags & SSD_ROUNDED_BLACK)))
		 			points[2].y -= 10;
		 	}

		 	if (widget->in_focus  && widget->focus_highlight )
				background = "#b0d504";
         	else if ((widget->flags & SSD_POINTER_COMMENT) | (widget->flags & SSD_POINTER_MENU) |  (widget->flags & SSD_POINTER_NONE))
         		background = "#e4f1f9";
         	else
         		background = "#f3f3f5";

		 	background = "#d2dfef";
		 	if (header_type != HEADER_BLACK)
		 	   header_type = HEADER_NONE;

			if (widget->flags & SSD_ROUNDED_WHITE){
				style = STYLE_WHITE;
				if (widget->in_focus  && widget->focus_highlight )
					background = "#b0d504";
        		else
					background = "#ffffff";
			}

			if (widget->flags & SSD_ROUNDED_BLACK){
            style = STYLE_BLACK;
            if (widget->in_focus  && widget->focus_highlight )
               background = "#b0d504";
            else
               background = "#000000";
         }

			if (widget->flags & SSD_POINTER_LOCATION){
				if (widget->context != NULL){
					pointer_type = POINTER_POSITION;
					position = (RoadMapPosition *)widget->context;
				}
			}

         	roadmap_display_border(style, header_type, pointer_type, &points[2], &points[0], background, position);

		 }
		 else
	        roadmap_canvas_draw_multiple_lines (1, &count, points, 1);
      }

	  if (widget->flags & SSD_ROUNDED_CORNERS){
	  	if (widget->flags & SSD_ROUNDED_WHITE){
	      	rect->minx += 4;
    	  	rect->miny += 4;
      		rect->maxx -= 4;
      		rect->maxy -= 4;
	  	}
	  	else{
			rect->minx += 10;
			if ((widget->flags & SSD_CONTAINER_TITLE) || (widget->flags & SSD_POINTER_MENU) ||  (widget->flags & SSD_POINTER_NONE))
      			rect->miny += 10;
      		else
      			rect->miny += 4;
      		rect->maxx -= 10;
      		rect->maxy -= 10;
	  	}
  	  }
	  else{
      	rect->minx += 2;
      	rect->miny += 2;
      	rect->maxx -= 2;
      	rect->maxy -= 2;
	  }

   }

   if (widget->flags & SSD_CONTAINER_TXT_BOX){
      	if (!(flags & SSD_GET_SIZE)){
      		if (background){
      			rect->minx -= 10;
      			rect->maxx += 10;
      		}
   			draw_text_box(widget, rect);
      	}
      	rect->minx += 10;
      	rect->miny += 10;
      	rect->maxx -= 10;
      	rect->maxy -= 10;

   }

   if (widget->flags & SSD_CONTAINER_TITLE){
   		ssd_widget_set_left_softkey_text(widget, widget->left_softkey);
   		ssd_widget_set_right_softkey_text(widget, widget->right_softkey);
   }

   if ((flags & SSD_GET_CONTAINER_SIZE) &&
       (widget->flags & SSD_CONTAINER_TITLE)) {
      SsdWidget title;

      title = ssd_widget_get (widget, "title_bar");

      if (title) {
         SsdSize title_size;
         SsdSize max_size;
         max_size.width = rect->maxx - rect->minx + 1;
         max_size.height = rect->maxy - rect->miny + 1;

         ssd_widget_get_size (title, &title_size, &max_size);
         rect->miny += title_size.height;

      }

   }
}


static int short_click (SsdWidget widget, const RoadMapGuiPoint *point) {

	widget->force_click = FALSE;

   if (widget->callback) {
      RoadMapSoundList list = roadmap_sound_list_create (0);
      if (roadmap_sound_list_add (list, "click") != -1) {
         roadmap_sound_play_list (list);
      }

      (*widget->callback) (widget, SSD_BUTTON_SHORT_CLICK);
      return 1;
   }

   return 0;
}

#ifdef TOUCH_SCREEN
static int button_callback (SsdWidget widget, const char *new_value) {
   ssd_dialog_hide_current (dec_ok);

   return 1;
}
#endif

static void add_title (SsdWidget w, int flags) {
   SsdWidget title, text;
	int rt_soft_key_flag;
	int lf_soft_key_flag;

#ifdef TOUCH_SCREEN
	const char *back_buttons[] = {"back_button", "back_button_s"};
#endif

	if (ssd_widget_rtl (NULL)) {
		rt_soft_key_flag = SSD_END_ROW;
		lf_soft_key_flag = SSD_ALIGN_RIGHT;
	}
	else{
		rt_soft_key_flag = SSD_ALIGN_RIGHT;
		lf_soft_key_flag = SSD_END_ROW;
	}


  if (!((w->flags & SSD_DIALOG_FLOAT) && !(w->flags & SSD_DIALOG_TRANSPARENT)))
  {
	int height = 28;
    if ( roadmap_screen_is_hd_screen() )
    {
    	height = 60;
    }
    else
    {
#ifdef TOUCH_SCREEN
    	height = 40;
#endif
    }

   title = ssd_container_new ("title_bar", NULL, SSD_MAX_SIZE, height, SSD_END_ROW);
   ssd_widget_set_click_offsets_ext( title, 0, 0, 0, 10 );
   ssd_widget_set_offset(title, 0, 0);
   title->draw = draw_title;
  }
  else{
   title = ssd_container_new ("title_bar", NULL, SSD_MAX_SIZE, 22,
                              SSD_END_ROW);
  }

   ssd_widget_set_color (title, "#ffffff", "#ff0000000");

   text = ssd_text_new ("title_text", "" , 18, SSD_WIDGET_SPACE|SSD_ALIGN_VCENTER|SSD_ALIGN_CENTER);

   ssd_widget_set_color (text, "#ffffff", "#ff0000000");



   if ((w->flags & SSD_ROUNDED_CORNERS) && (!(w->flags & SSD_POINTER_MENU)))
   	ssd_widget_set_color (text, "#ffffff", "#ff0000000");

   if (w->flags & SSD_CONTAINER_TITLE){
   		if (w->flags & SSD_ROUNDED_BLACK)
   		   ssd_widget_set_color (text, "#ffffff", "#ffffff");
   		else if (!(w->flags & SSD_DIALOG_FLOAT))
   			ssd_widget_set_color (text, "#ffffff", "#ff0000000");
   		else
   		   if (flags & SSD_DIALOG_TRANSPARENT)
   		      ssd_widget_set_color (text, "#ffffff", "#ff0000000");
   		   else
   		      ssd_widget_set_color (text, "#000000", "#ff0000000");
   }
#ifdef TOUCH_SCREEN
   if (!( ((flags & SSD_DIALOG_FLOAT)&& !(flags & SSD_DIALOG_TRANSPARENT)) || (flags & SSD_DIALOG_NO_BACK))){
	   SsdWidget btn =             ssd_button_new ("back", "", back_buttons, 2,
               SSD_ALIGN_VCENTER, button_callback );


	   ssd_widget_set_click_offsets_ext( btn, -20, -20, 70, 10 );
	   if ( ssd_widget_rtl(NULL) )
	   {
		   ssd_widget_add (title,text);
		   ssd_widget_set_flags( btn, SSD_ALIGN_VCENTER|SSD_ALIGN_RIGHT );
		   ssd_widget_add( title, btn );
	   }
	   else
	   {
		   ssd_widget_add( title, btn );
		   ssd_widget_add (title,text);
	   }

	}
   else
   {
	   ssd_widget_add ( title, text );
   }
#else
   ssd_widget_add ( title, text );
#endif

   ssd_widget_add (w, title);
}


static int set_value (SsdWidget widget, const char *value) {

   if (!(widget->flags & SSD_CONTAINER_TITLE)) return -1;

   return ssd_widget_set_value (widget, "title_text", value);
}


static const char *get_value (SsdWidget widget) {

   if ( !( widget->flags & SSD_CONTAINER_TITLE ) ) return "";

   return ssd_widget_get_value (widget, "title_text");
}

static BOOL ssd_container_on_key_pressed( SsdWidget   widget,
                                          const char* utf8char,
                                          uint32_t      flags)
{
   if( KEY_IS_ENTER && widget->callback)
   {
      widget->callback( widget, SSD_BUTTON_SHORT_CLICK);
      return TRUE;
   }

   return FALSE;
}

int on_pointer_down( SsdWidget this, const RoadMapGuiPoint *point)
{
   if( !this->tab_stop)
      return 0;
return  0; //AVI

   if( !this->in_focus)
      ssd_dialog_set_focus( this);

   return 0;
}

SsdWidget ssd_container_new (const char *name, const char *title,
                             int width, int height, int flags) {

   SsdWidget w;
#ifdef ANDROID
   flags |= SSD_DIALOG_NO_BACK;
#endif

   if (!initialized) {
      init_containers();
   }

   w = ssd_widget_new (name, ssd_container_on_key_pressed, flags);

   w->_typeid = "Container";

   w->flags = flags;

   w->draw = draw;

   w->size.width = width;
   w->size.height = height;

   w->get_value = get_value;
   w->set_value = set_value;
   w->short_click = short_click;

   w->fg_color = default_fg;
   w->bg_color = default_bg;

   w->pointer_down = on_pointer_down;

   if (flags & SSD_CONTAINER_TITLE) {
       add_title (w, flags);
   }

   if ((flags & SSD_CONTAINER_TITLE) || ((flags & SSD_DIALOG_TRANSPARENT) && (flags & SSD_DIALOG_FLOAT))){
   	   ssd_widget_set_right_softkey_text(w, roadmap_lang_get("Back_key"));
   	   ssd_widget_set_left_softkey_text(w, roadmap_lang_get("Exit_key"));
   }

   set_value (w, title);

   return w;
}

void ssd_container_get_zero_offset(
                     SsdWidget         this,
                     int*              zero_offset_x,
                     int*              zero_offset_y)
{
   RoadMapGuiRect org_rect;
   RoadMapGuiRect dra_rect;

   assert(this);
   assert(zero_offset_x);
   assert(zero_offset_y);

   org_rect.minx = this->position.x;
   org_rect.miny = this->position.y;
   org_rect.maxx = org_rect.minx + this->size.width;
   org_rect.maxy = org_rect.miny + this->size.height;
   dra_rect      = org_rect;

   draw( this, &dra_rect, SSD_GET_SIZE);

   (*zero_offset_x) = dra_rect.minx - org_rect.minx;
   (*zero_offset_y) = dra_rect.miny - org_rect.miny;
}

void ssd_container_get_visible_dimentions(
                     SsdWidget         this,
                     RoadMapGuiPoint*  position,
                     SsdSize*          size)
{
   assert(this);
   assert(position);
   assert(size);

   (*position) = this->position;
   (*size)     = this->size;

   if( SSD_ROUNDED_CORNERS & this->flags)
   {
      position->x += 3;
      position->y += 4;

      size->width -= (3 + 3);
      size->height-= (4 + 3);

      if( (SSD_POINTER_MENU|SSD_POINTER_NONE) & this->flags)
         size->height -= 8;
   }
}