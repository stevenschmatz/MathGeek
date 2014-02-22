#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
GBitmap *future_bitmap;
BitmapLayer *future_layer;
GPoint p0, p1;
static GRect window_frame;
static Layer *disc_layer;

#define MAXWIDTH 144
#define MAXHEIGHT 168

int get_number_of_perimeter_pixels(int width, int height) {
  return (2*width + 2*height - 4);
}

static void disc_draw(GContext *ctx) {
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, GPoint(50, 50), 20);
}


static void disc_layer_update_callback(Layer *me, GContext *ctx) {
  disc_draw(ctx);
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
