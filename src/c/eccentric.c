#include <pebble.h>

static Window *s_window;
static Layer *s_canvas_layer;
static uint32_t s_bg_color = 0xffffff;
static uint32_t s_hour_color = 0x000000;
static uint32_t s_min_color = 0xffffff;

static void canvas_update_proc(Layer *layer, GContext *ctx) {
  time_t unadjusted_time = time(NULL);
  struct tm *cur_time = localtime(&unadjusted_time);
  uint8_t hour = cur_time->tm_hour % 12;
  uint8_t min = cur_time->tm_min;
  // uint8_t hour = s_hour;
  // uint8_t min = s_min;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Time is %d:%d", hour, min);

  GPoint center = GPoint(PBL_DISPLAY_WIDTH / 2, PBL_DISPLAY_HEIGHT / 2);
  graphics_context_set_stroke_width(ctx, 0);
#if defined(PBL_RECT)
  graphics_context_set_fill_color(ctx, GColorLightGray);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
  graphics_context_set_fill_color(ctx, GColorFromHEX(s_bg_color));
  graphics_fill_circle(ctx, center, PBL_DISPLAY_WIDTH / 2 - 1);
#else
  graphics_context_set_fill_color(ctx, GColorFromHEX(s_bg_color));
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
#endif

  int32_t hour_angle = TRIG_MAX_ANGLE * hour / 12 + TRIG_MAX_ANGLE / 12 * min / 60;
  int32_t hour_hand_length = PBL_DISPLAY_WIDTH / 4; //PBL_IF_ROUND_ELSE(180, 144) / 4;
  GPoint hour_center = center;
  GPoint hour_hand;
  hour_hand.y = (-cos_lookup(hour_angle) * hour_hand_length / TRIG_MAX_RATIO) + hour_center.y;
  hour_hand.x = (sin_lookup(hour_angle) * hour_hand_length / TRIG_MAX_RATIO) + hour_center.x;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Drawing hour at %d, %d", hour_hand.x, hour_hand.y);
  uint16_t hour_radius = hour_hand_length * 2 / 3;
  graphics_context_set_fill_color(ctx, GColorFromHEX(s_hour_color));
  graphics_fill_circle(ctx, hour_hand, hour_radius);
  
  int32_t min_angle = TRIG_MAX_ANGLE * min / 60;
  int32_t min_hand_length = hour_hand_length / 3;
  GPoint min_center = hour_hand;
  GPoint min_hand;
  min_hand.y = (-cos_lookup(min_angle) * min_hand_length / TRIG_MAX_RATIO) + min_center.y;
  min_hand.x = (sin_lookup(min_angle) * min_hand_length / TRIG_MAX_RATIO) + min_center.x;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Drawing min at %d, %d", min_hand.x, min_hand.y);
  uint16_t min_radius = hour_radius / 3;
  graphics_context_set_fill_color(ctx, GColorFromHEX(s_min_color));
  graphics_fill_circle(ctx, min_hand, min_radius);
}

static void time_handler(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(s_canvas_layer);
}

// static void timer_handle(void* context) {
//   s_min++;
//   if (s_min == 60) {
//     s_min = 0;
//     s_hour = (s_hour + 1) % 12;
//   }
//   layer_mark_dirty(s_canvas_layer);
//   app_timer_register(15, timer_handle, NULL);
// }

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_canvas_layer = layer_create(bounds);
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
  layer_add_child(window_layer, s_canvas_layer);

  layer_mark_dirty(s_canvas_layer);
}

static void window_unload(Window *window) {
  layer_destroy(s_canvas_layer);
}

static void init(void) {
  s_window = window_create();
  tick_timer_service_subscribe(MINUTE_UNIT, time_handler);

  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(s_window, animated);
}

static void deinit(void) {
  window_destroy(s_window);
}

int main(void) {
  init();

  app_event_loop();
  deinit();
}
