#include <pebble.h>

#define CHANNEL 1
#define SPEED 2

// UI Elements
static Window *s_main_window;

static Layer *channel_layer;
static TextLayer *s_channel_label;
static TextLayer *s_channel_layer;

static Layer *speed_layer;
static TextLayer *s_speed_label;
static TextLayer *s_speed_layer;

static AppSync s_sync;
static uint8_t s_sync_buffer[32];

static char s_channel[] = "1";
static char s_speed[] = "1";

/*
 * Recieving messages from Android app
 */
static void sync_changed_handler(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context) {
  static char buffer[64];
  // da kommen komisch Werte an. nicht beachten
  if (new_tuple->value->int32 > 7) return;
  snprintf(buffer, sizeof(buffer), "%u -> %u", (uint)key, (uint)new_tuple->value->int32);
  APP_LOG(APP_LOG_LEVEL_ERROR, buffer);
  if (key == CHANNEL) {
    // channel
    snprintf(s_channel, sizeof(s_channel), "%u", (int)new_tuple->value->int32);
    text_layer_set_text(s_channel_layer, s_channel);
  } else if (key == SPEED) {
    //speed
    snprintf(s_speed, sizeof(s_speed), "%u", (int)new_tuple->value->int32);
    text_layer_set_text(s_speed_layer, s_speed);
  }
}

static void sync_error_handler(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  // An error occured!
  APP_LOG(APP_LOG_LEVEL_ERROR, "sync error!");
}

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
  channel_layer = layer_create(GRect(10, 10, 124, 50));
  layer_set_update_proc(channel_layer, update_layer);
  layer_add_child(window_get_root_layer(window), channel_layer);

  s_channel_label = text_layer_create(GRect(10, 10, 90, 50));
  text_layer_set_text(s_channel_label, "Channel:");
  init_text_layer(s_channel_label, channel_layer);

  s_channel_layer = text_layer_create(GRect(90, 10, 24, 50));
  init_text_layer(s_channel_layer, channel_layer);

  // Speed
  speed_layer = layer_create(GRect(10, 70, 124, 50));
  layer_set_update_proc(speed_layer, update_layer);
  layer_add_child(window_get_root_layer(window), speed_layer);

  s_speed_label = text_layer_create(GRect(10, 10, 90, 50));
  text_layer_set_text(s_speed_label, "Speed:");
  init_text_layer(s_speed_label, speed_layer);

  s_speed_layer = text_layer_create(GRect(90, 10, 24, 50));
  init_text_layer(s_speed_layer, speed_layer);
}

static void main_window_unload(Window *window) {
    layer_destroy(channel_layer);
    layer_destroy(speed_layer);
}

static void init() {
  s_main_window = window_create();

  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  window_stack_push(s_main_window, true);

  // Setup AppSync
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  // Setup initial values
  Tuplet initial_values[] = {
    TupletInteger(CHANNEL, 0),
    TupletInteger(SPEED, 0)
  };

  // Begin using AppSync
  app_sync_init(&s_sync, s_sync_buffer, sizeof(s_sync_buffer), initial_values, ARRAY_LENGTH(initial_values), sync_changed_handler, sync_error_handler, NULL);
}

static void deinit() {
  window_destroy(s_main_window);

  // Finish using AppSync
  app_sync_deinit(&s_sync);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}