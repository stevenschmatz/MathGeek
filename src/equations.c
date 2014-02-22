#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
GBitmap *future_bitmap;
BitmapLayer *future_layer;



static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  future_bitmap = gbitmap_create_with_resource(RESOURCE_ID_EQUATION_IMAGE);
  future_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(future_layer, future_bitmap);
  bitmap_layer_set_background_color(future_layer, GColorClear);
  bitmap_layer_set_alignment(future_layer, GAlignCenter);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(future_layer));

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
