#include <pebble.h>
#include <stdlib.h>
	
static Window *window;
static TextLayer *time_text;
static TextLayer *battery;
static TextLayer *date;
static GBitmap *SAO;
static BitmapLayer* SAO_Layer;
static GFont font;
static GFont font15;

void handle_battery(BatteryChargeState charge_state) {
	
	static char battery_text[] = "100%";
	
	if (charge_state.is_charging) {
		
		snprintf(battery_text, sizeof(battery_text), "chg");
		
	} else {
		
		snprintf(battery_text, sizeof(battery_text), "%d%%", charge_state.charge_percent);
		
	}
	text_layer_set_text(battery, battery_text);
}


void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
	
	static char dateBuffer[] = "Tuesday 7";
	static char timeBuffer[] = "00:00";
	
	//update watchface display swag swag
	
	//if settings are set to 24hr time 
	if(clock_is_24h_style()){
		strftime(timeBuffer, sizeof(timeBuffer), "%H:%M", tick_time);
	} else {
		strftime(timeBuffer, sizeof(timeBuffer), "%I:%M", tick_time);
	}
	strftime(dateBuffer, sizeof(dateBuffer), "%b %e", tick_time);
	text_layer_set_text(time_text, timeBuffer);
	text_layer_set_text(date, dateBuffer);
	
}

static void window_load(Window *window)
{
     font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SWAG_20));
	font15 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SWAG_15));

	Layer *window_layer = window_get_root_layer(window);
	
	//Set up background swag
	SAO = gbitmap_create_with_resource(RESOURCE_ID_sao_background);
	SAO_Layer = bitmap_layer_create(GRect(0, 0, 144, 160));
	bitmap_layer_set_background_color(SAO_Layer, GColorBlack);
	bitmap_layer_set_bitmap(SAO_Layer, SAO);
	layer_add_child(window_layer, bitmap_layer_get_layer(SAO_Layer));
	
	//Set up text (Date)
	date = text_layer_create(GRect(0, 138, 144, 50));
	text_layer_set_font(date, font);
	text_layer_set_background_color(date, GColorBlack);
    text_layer_set_text_color(date, GColorWhite);
    layer_add_child(window_layer, text_layer_get_layer(date));
	
	//Set up text (Battery)
	battery = text_layer_create(GRect(120,0,60,40));
	text_layer_set_font(battery, font15);
    text_layer_set_background_color(battery, GColorBlack);
    text_layer_set_text_color(battery, GColorWhite);
    layer_add_child(window_layer, text_layer_get_layer(battery));
	
	//Set up text (Time)
	time_text = text_layer_create(GRect(0,0,60,40));
	text_layer_set_font(time_text, font15);
    text_layer_set_background_color(time_text, GColorBlack);
    text_layer_set_text_color(time_text, GColorWhite);
    text_layer_set_text(time_text, "Press a button");
    text_layer_set_text_alignment(time_text, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(time_text));
	
	struct tm *t;
	time_t temp;
	temp = time(NULL);
	t = localtime(&temp);
	tick_handler(t,MINUTE_UNIT);
	
	//get battery
	BatteryChargeState btchg = battery_state_service_peek();
	handle_battery(btchg);
	
}

static void window_unload(Window *window) {
	
	text_layer_destroy(time_text);
	text_layer_destroy(battery);
	text_layer_destroy(date);
	fonts_unload_custom_font(font);
	
}

static void init(void) {
	
	window = window_create();
	tick_timer_service_subscribe(MINUTE_UNIT, (TickHandler) tick_handler);
	battery_state_service_subscribe(&handle_battery);
	window_set_background_color(window, GColorBlack);
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	
	
	const int inbound_size = 64;
	const int outbound_size = 64;
	app_message_open(inbound_size, outbound_size);
	const bool animated = true;
	window_stack_push(window, animated);
	
}

static void deinit(void)
{
	
	gbitmap_destroy(SAO);
	bitmap_layer_destroy(SAO_Layer);
	tick_timer_service_unsubscribe();
	battery_state_service_unsubscribe();
	window_destroy(window);
	
}

int main(void)
{
	
	init();
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);
	app_event_loop();
	deinit();
	
}