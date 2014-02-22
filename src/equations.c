#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
GBitmap *future_bitmap;
BitmapLayer *future_layer;
GPoint p0, p1;
static GRect window_frame;
static Layer *disc_layer;

static AppTimer *timer;

#define MAXWIDTH 144
#define MAXHEIGHT 168

int get_number_of_perimeter_pixels(int width, int height) {
  return (2*width + 2*height - 4);
}

static void disc_draw(GContext *ctx, int x, int y) {
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, GPoint(x, y), 10);
}


static void disc_layer_update_callback(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  const GPoint center = grect_center_point(&bounds);
  const int16_t secondHandLength = bounds.size.w / 2;



  GPoint secondHand;

  

  time_t now = time(NULL);
  struct tm *t = localtime(&now);


  disc_draw(ctx, (t->tm_sec)*(MAXWIDTH/60), MAXHEIGHT);

}

static void window_load(Window *window) {

  Layer *window_layer = window_get_root_layer(window);
  GRect frame = window_frame = layer_get_frame(window_layer);

  future_bitmap = gbitmap_create_with_resource(RESOURCE_ID_EQUATION_IMAGE);
  future_layer = bitmap_layer_create(GRect(0, 0, MAXWIDTH, MAXHEIGHT));
  bitmap_layer_set_bitmap(future_layer, future_bitmap);
  bitmap_layer_set_background_color(future_layer, GColorClear);
  bitmap_layer_set_alignment(future_layer, GAlignCenter);
  layer_add_child(window_layer, bitmap_layer_get_layer(future_layer));


  disc_layer = layer_create(frame);
  layer_set_update_proc(disc_layer, disc_layer_update_callback);
  layer_add_child(window_layer, disc_layer);

}



static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
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

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
}

static void deinit(void) {
  window_destroy(window);
  gbitmap_destroy(future_bitmap);
  bitmap_layer_destroy(future_layer);
}


int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
