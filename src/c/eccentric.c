#include <pebble.h>

#define CLAY_SAVE_KEY 1

typedef struct ClaySettings {
  GColor BackgroundOuterColor;
  GColor BackgroundInnerColor;
  GColor HourColor;
  GColor MinuteColor;
  bool SecondsEnabled;
  GColor SecondsColor;
  bool BatteryEnabled;
  GColor BatteryColor;
} ClaySettings;

static Window *s_window;
static Layer *s_canvas_layer;

ClaySettings settings;

static int clamp(int val, int min_val, int max_val) {
  if (val < min_val) {
    return min_val;
  }
  if (val > max_val) {
    return max_val;
  }
  return val;
}

static void canvas_update_proc(Layer *layer, GContext *ctx) {
  GRect layer_bounds = layer_get_bounds(layer);
  time_t unadjusted_time = time(NULL);
  struct tm *cur_time = localtime(&unadjusted_time);
  uint8_t hour = cur_time->tm_hour % 12;
  uint8_t min = cur_time->tm_min;
  uint8_t sec = cur_time->tm_sec;

  GPoint center = GPoint(layer_bounds.size.w / 2, layer_bounds.size.h / 2);
  graphics_context_set_stroke_width(ctx, 0);
#if defined(PBL_RECT)
  graphics_context_set_fill_color(ctx, settings.BackgroundOuterColor);
  graphics_fill_rect(ctx, layer_bounds, 0, GCornerNone);
#endif
  graphics_context_set_fill_color(ctx, settings.BackgroundInnerColor);
  graphics_fill_circle(ctx, center, layer_bounds.size.w / 2 - 1);

  if (settings.BatteryEnabled) {
    BatteryChargeState battery_state = battery_state_service_peek();

    graphics_context_set_fill_color(ctx, settings.BatteryColor);
    graphics_fill_circle(ctx, center, (layer_bounds.size.w / 2 - 1) * battery_state.charge_percent / 100);
  }

  int32_t hour_angle = TRIG_MAX_ANGLE * hour / 12 + TRIG_MAX_ANGLE / 12 * min / 60;
  int32_t hour_hand_length = PBL_IF_ROUND_ELSE(layer_bounds.size.w / 4, layer_bounds.size.h / 4);
  GPoint hour_center = center;
  GPoint hour_hand;
  hour_hand.y = (-cos_lookup(hour_angle) * hour_hand_length / TRIG_MAX_RATIO) + hour_center.y;
  hour_hand.x = (sin_lookup(hour_angle) * hour_hand_length / TRIG_MAX_RATIO) + hour_center.x;
#if defined(PBL_RECT)
  int min_x = center.x - layer_bounds.size.w / 4;
  int max_x = center.x + layer_bounds.size.w / 4;
  int min_y = center.y - layer_bounds.size.h / 4;
  int max_y = center.y + layer_bounds.size.h / 4;
  hour_hand.y = clamp(hour_hand.y, min_y, max_y);
  hour_hand.x = clamp(hour_hand.x, min_x, max_x);
#endif
  uint16_t hour_radius = hour_hand_length * 2 / 3;
  graphics_context_set_fill_color(ctx, settings.HourColor);
  graphics_fill_circle(ctx, hour_hand, hour_radius);
  
  int32_t min_angle = TRIG_MAX_ANGLE * min / 60;
  int32_t min_hand_length = hour_hand_length / 3;
  GPoint min_center = hour_hand;
  GPoint min_hand;
  min_hand.y = (-cos_lookup(min_angle) * min_hand_length / TRIG_MAX_RATIO) + min_center.y;
  min_hand.x = (sin_lookup(min_angle) * min_hand_length / TRIG_MAX_RATIO) + min_center.x;
  uint16_t min_radius = hour_radius / 3;
  graphics_context_set_fill_color(ctx, settings.MinuteColor);
  graphics_fill_circle(ctx, min_hand, min_radius);
  
  if (settings.SecondsEnabled) {
    int32_t sec_angle = TRIG_MAX_ANGLE * sec / 60;
    int32_t sec_hand_length = min_hand_length / 3;
    GPoint sec_center = min_hand;
    GPoint sec_hand;
    sec_hand.y = (-cos_lookup(sec_angle) * sec_hand_length / TRIG_MAX_RATIO) + sec_center.y;
    sec_hand.x = (sin_lookup(sec_angle) * sec_hand_length / TRIG_MAX_RATIO) + sec_center.x;
    uint16_t sec_radius = min_radius / 3;
    graphics_context_set_fill_color(ctx, settings.SecondsColor);
    graphics_fill_circle(ctx, sec_hand, sec_radius);
  }
}

static void time_handler(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(s_canvas_layer);
}

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

static void default_settings() {
  settings.BackgroundOuterColor = GColorWhite;
  settings.BackgroundInnerColor = GColorWhite;
  settings.HourColor = GColorBlack;
  settings.MinuteColor = GColorWhite;
  settings.SecondsEnabled = false;
  settings.SecondsColor = GColorBlack;
  settings.BatteryEnabled = false;
  settings.BatteryColor = GColorLightGray;
}

static void load_settings() {
  default_settings();

  persist_read_data(CLAY_SAVE_KEY, &settings, sizeof(settings));
}

static void save_settings() {
  persist_write_data(CLAY_SAVE_KEY, &settings, sizeof(settings));

  layer_mark_dirty(s_canvas_layer);
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  // Read color preferences
  Tuple *bg_outer_color_t = dict_find(iter, MESSAGE_KEY_BackgroundOuterColor);
  if(bg_outer_color_t) {
    settings.BackgroundOuterColor = GColorFromHEX(bg_outer_color_t->value->int32);
  }
  
  Tuple *bg_inner_color_t = dict_find(iter, MESSAGE_KEY_BackgroundInnerColor);
  if(bg_inner_color_t) {
    settings.BackgroundInnerColor = GColorFromHEX(bg_inner_color_t->value->int32);
  }
  
  Tuple *hour_color_t = dict_find(iter, MESSAGE_KEY_HourColor);
  if(hour_color_t) {
    settings.HourColor = GColorFromHEX(hour_color_t->value->int32);
  }
  
  Tuple *minute_color_t = dict_find(iter, MESSAGE_KEY_MinuteColor);
  if(minute_color_t) {
    settings.MinuteColor = GColorFromHEX(minute_color_t->value->int32);
  }

  Tuple *seconds_enabled_t = dict_find(iter, MESSAGE_KEY_SecondsEnabled);
  if(seconds_enabled_t) {
    settings.SecondsEnabled = seconds_enabled_t->value->int32 == 1;
    tick_timer_service_subscribe(settings.SecondsEnabled ? SECOND_UNIT : MINUTE_UNIT, time_handler);
  }
  
  Tuple *seconds_color_t = dict_find(iter, MESSAGE_KEY_SecondsColor);
  if(seconds_color_t) {
    settings.SecondsColor = GColorFromHEX(seconds_color_t->value->int32);
  }

  Tuple *battery_enabled_t = dict_find(iter, MESSAGE_KEY_BatteryEnabled);
  if(battery_enabled_t) {
    settings.BatteryEnabled = battery_enabled_t->value->int32 == 1;
  }
  
  Tuple *battery_color_t = dict_find(iter, MESSAGE_KEY_BatteryColor);
  if(battery_color_t) {
    settings.BatteryColor = GColorFromHEX(battery_color_t->value->int32);
  }

  save_settings();
}

static void battery_callback(BatteryChargeState state) {
  layer_mark_dirty(s_canvas_layer);
}

static void init(void) {
  s_window = window_create();

  battery_state_service_subscribe(battery_callback);

  // Load any previous Clay settings
  load_settings();
  
  tick_timer_service_subscribe(settings.SecondsEnabled ? SECOND_UNIT : MINUTE_UNIT, time_handler);

  // Open AppMessage connection
  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(128, 128);

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
