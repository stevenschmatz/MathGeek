#include <pebble.h>

static Window *window;
GBitmap *future_bitmap;
BitmapLayer *future_layer;
static GRect window_frame;
static Layer *disc_layer;
static Layer *equation_layer;

#define MAXWIDTH 144
#define MAXHEIGHT 168

#define NUMBER_OF_IMAGES 12

const int IMAGE_RESOURCE_IDS[NUMBER_OF_IMAGES] = {
  RESOURCE_ID_ONE, RESOURCE_ID_TWO, RESOURCE_ID_THREE, 
  RESOURCE_ID_FOUR, RESOURCE_ID_FIVE, RESOURCE_ID_SIX, 
  RESOURCE_ID_SEVEN, RESOURCE_ID_EIGHT, RESOURCE_ID_NINE, 
  RESOURCE_ID_TEN, RESOURCE_ID_ELEVEN, RESOURCE_ID_TWELVE
};


int get_number_of_perimeter_pixels(int width, int height) {
  return (2*width + 2*height - 4);
}

int get_disc_x_position(int minutes) {
  double middle = MAXWIDTH/2.0;
  int perimeter = get_number_of_perimeter_pixels(MAXWIDTH, MAXHEIGHT);
  double increment = perimeter/60.0; // multiply by minutes to get pixels
  double pixels = (increment*minutes);
  
  if ((pixels >= 0) && (pixels < MAXWIDTH/2.0)) 
  {
    return pixels + middle;
  }

  else if ((pixels >= middle) && (pixels < middle+MAXHEIGHT))
  {
    return MAXWIDTH;
  }

  else if ((pixels >= middle + MAXHEIGHT) && (pixels < (middle + MAXHEIGHT + MAXWIDTH))) {
    return MAXWIDTH - (pixels - (middle + MAXHEIGHT));
  }

  else if ((pixels >= (middle + MAXHEIGHT + MAXWIDTH)) && (pixels < (middle + MAXHEIGHT*2 + MAXWIDTH))) {
    return 0;
  }

  else {
    return (pixels - (middle + MAXHEIGHT*2 + MAXWIDTH));
  }

}

int get_disc_y_position(int minutes) {
  double middle = MAXWIDTH/2.0;
  int perimeter = get_number_of_perimeter_pixels(MAXWIDTH, MAXHEIGHT);
  double increment = perimeter/60.0; // multiply by minutes to get pixels
  double pixels = (increment*minutes);

    if ((pixels >= 0) && (pixels < MAXWIDTH/2.0)) 
  {
    return 0;
  }

  else if ((pixels >= middle) && (pixels < middle+MAXHEIGHT))
  {
    return pixels - middle;
  }

  else if ((pixels >= middle + MAXHEIGHT) && (pixels < (middle + MAXHEIGHT + MAXWIDTH))) {
    return MAXHEIGHT;
  }

  else if ((pixels >= (middle + MAXHEIGHT + MAXWIDTH)) && (pixels < (middle + MAXHEIGHT*2 + MAXWIDTH))) {
    return MAXHEIGHT - (pixels - (middle + MAXHEIGHT + MAXWIDTH));
  }

  else {
    return 0;
  }
}


static void disc_draw(GContext *ctx, int x, int y) {
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, GPoint(x, y), 10);
}

static void window_load(Window *window) {

  /*Layer *window_layer = window_get_root_layer(window);
  GRect frame = window_frame = layer_get_frame(window_layer);

  future_bitmap = gbitmap_create_with_resource(IMAGE_RESOURCE_IDS[0]);
  future_layer = bitmap_layer_create(GRect(0, 0, MAXWIDTH, MAXHEIGHT));
  bitmap_layer_set_bitmap(future_layer, future_bitmap);
  bitmap_layer_set_background_color(future_layer, GColorClear);
  bitmap_layer_set_alignment(future_layer, GAlignCenter);
  layer_add_child(window_layer, bitmap_layer_get_layer(future_layer));
  */
}

static void window_unload(Window *window) {
}

void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(disc_layer);
  //
}

void handle_hour_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(equation_layer);
  //
}

void disc_layer_update_callback(Layer *me, GContext* ctx) {

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  int minutes = t->tm_min;
  int hours = t->tm_hour;

  disc_draw(ctx, get_disc_x_position(minutes), get_disc_y_position(minutes));

  gbitmap_destroy(future_bitmap);
  bitmap_layer_destroy(future_layer);

  Layer *window_layer = window_get_root_layer(window);
  GRect frame = window_frame = layer_get_frame(window_layer);
  

  future_bitmap = gbitmap_create_with_resource(IMAGE_RESOURCE_IDS[(hours-1)%12]);
  future_layer = bitmap_layer_create(GRect(0, 0, MAXWIDTH, MAXHEIGHT));
  bitmap_layer_set_bitmap(future_layer, future_bitmap);
  bitmap_layer_set_background_color(future_layer, GColorClear);
  bitmap_layer_set_alignment(future_layer, GAlignCenter);
  layer_add_child(window_layer, bitmap_layer_get_layer(future_layer));
}

void equation_layer_update_callback(Layer *me, GContext* ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  int hour = t->tm_hour;

  Layer *window_layer = window_get_root_layer(window);
  GRect frame = window_frame = layer_get_frame(window_layer);

  future_bitmap = gbitmap_create_with_resource(IMAGE_RESOURCE_IDS[hour]);
  future_layer = bitmap_layer_create(GRect(0, 0, MAXWIDTH, MAXHEIGHT));
  bitmap_layer_set_bitmap(future_layer, future_bitmap);
  bitmap_layer_set_background_color(future_layer, GColorClear);
  bitmap_layer_set_alignment(future_layer, GAlignCenter);
  layer_add_child(window_layer, bitmap_layer_get_layer(future_layer));
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_set_background_color(window, GColorBlack);
  window_stack_push(window, animated);

  Layer *root_layer = window_get_root_layer(window);
  GRect frame = layer_get_frame(root_layer);

  disc_layer = layer_create(frame);
  layer_set_update_proc(disc_layer, &disc_layer_update_callback);
  layer_add_child(root_layer, disc_layer);

  /*equation_layer = layer_create(frame);
  layer_set_update_proc(equation_layer, &equation_layer_update_callback);
  layer_add_child(root_layer, equation_layer);*/

  tick_timer_service_subscribe(SECOND_UNIT, &handle_second_tick);
  //tick_timer_service_subscribe(HOUR_UNIT, &handle_hour_tick);
}

static void deinit(void) {
  window_destroy(window);
  gbitmap_destroy(future_bitmap);
  bitmap_layer_destroy(future_layer);
  tick_timer_service_unsubscribe();
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
