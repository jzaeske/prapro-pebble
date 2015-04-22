#include <pebble.h>

// UI Elements
static Window *s_main_window;

static Layer *train_layer;
static TextLayer *s_train_label;
static TextLayer *s_train_layer;

static Layer *speed_layer;
static TextLayer *s_speed_label;
static TextLayer *s_speed_layer;

// Display Values
static int train = 3;
static int speed = 7;

static char s_train[] = "1";
static char s_speed[] = "1";

static void update_layer(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, bounds, 5, GCornersAll);
}

static void init_text_layer(TextLayer *layer, Layer *parent) {
  text_layer_set_background_color(layer, GColorClear);
  text_layer_set_text_color(layer, GColorWhite);
  text_layer_set_font(layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(parent, text_layer_get_layer(layer));
}

static void main_window_load(Window *window) {
  // Zugnummer
  train_layer = layer_create(GRect(10, 10, 124, 50));
  layer_set_update_proc(train_layer, update_layer);
  layer_add_child(window_get_root_layer(window), train_layer);

  s_train_label = text_layer_create(GRect(0, 0, 90, 50));
  text_layer_set_text(s_train_label, "Zug:");
  init_text_layer(s_train_label, train_layer);

  s_train_layer = text_layer_create(GRect(90, 0, 24, 50));
  snprintf(s_train, sizeof(s_train), "%u", train);
  text_layer_set_text(s_train_layer, s_train);
  init_text_layer(s_train_layer, train_layer);

  // Speed
  speed_layer = layer_create(GRect(10, 70, 124, 50));
  layer_set_update_proc(speed_layer, update_layer);
  layer_add_child(window_get_root_layer(window), speed_layer);

  s_speed_label = text_layer_create(GRect(0, 0, 90, 50));
  text_layer_set_text(s_speed_label, "Spd:");
  init_text_layer(s_speed_label, speed_layer);

  s_speed_layer = text_layer_create(GRect(90, 0, 24, 50));
  snprintf(s_speed, sizeof(s_speed), "%u", speed);
  text_layer_set_text(s_speed_layer, s_speed);
  init_text_layer(s_speed_layer, speed_layer);
}

static void main_window_unload(Window *window) {
    layer_destroy(train_layer);
    layer_destroy(speed_layer);
}

static void init() {
  s_main_window = window_create();

  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  window_stack_push(s_main_window, true);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}