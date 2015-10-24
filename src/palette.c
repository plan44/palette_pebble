/*

  Color Gradient Watchface
  (c) 2015 by Lukas Zeller, plan44.ch

 */

#include <pebble.h>


// Geometry constants
#ifdef PBL_ROUND
#define MINUTE_DIAMETER 180
#define HOUR_DIAMETER 118
#else
#define MINUTE_DIAMETER 144
#define HOUR_DIAMETER 95
#endif

#define NUM_SEGMENTS 60


static GColor8 minuteColors[NUM_SEGMENTS] = {
  { .argb = GColorPastelYellowARGB8 },
  { .argb = GColorIcterineARGB8 },
  { .argb = GColorYellowARGB8 },
  { .argb = GColorSpringBudARGB8 },
  { .argb = GColorInchwormARGB8 },
  { .argb = GColorBrightGreenARGB8 },
  { .argb = GColorGreenARGB8 },
  { .argb = GColorScreaminGreenARGB8 },
  { .argb = GColorMalachiteARGB8 },
  { .argb = GColorMintGreenARGB8 },
  { .argb = GColorMediumSpringGreenARGB8 },
  { .argb = GColorMediumAquamarineARGB8 },
  { .argb = GColorCelesteARGB8 },
  { .argb = GColorElectricBlueARGB8 },
  { .argb = GColorCyanARGB8 },
  { .argb = GColorVividCeruleanARGB8 },
  { .argb = GColorPictonBlueARGB8 },
  { .argb = GColorBabyBlueEyesARGB8 },
  { .argb = GColorRichBrilliantLavenderARGB8 },
  { .argb = GColorShockingPinkARGB8 },
  { .argb = GColorMagentaARGB8 },
  { .argb = GColorFashionMagentaARGB8 },
  { .argb = GColorBrilliantRoseARGB8 },
  { .argb = GColorLavenderIndigoARGB8 },
  { .argb = GColorVividVioletARGB8 },
  { .argb = GColorElectricUltramarineARGB8 },
  { .argb = GColorVeryLightBlueARGB8 },
  { .argb = GColorBlueMoonARGB8 },
  { .argb = GColorBlueARGB8 },
  { .argb = GColorCadetBlueARGB8 },
  { .argb = GColorTiffanyBlueARGB8 },
  { .argb = GColorMayGreenARGB8 },
  { .argb = GColorJaegerGreenARGB8 },
  { .argb = GColorIslamicGreenARGB8 },
  { .argb = GColorKellyGreenARGB8 },
  { .argb = GColorBrassARGB8 },
  { .argb = GColorLimerickARGB8 },
  { .argb = GColorRajahARGB8 },
  { .argb = GColorChromeYellowARGB8 },
  { .argb = GColorMelonARGB8 },
  { .argb = GColorSunsetOrangeARGB8 },
  { .argb = GColorOrangeARGB8 },
  { .argb = GColorRedARGB8 },
  { .argb = GColorFollyARGB8 },
  { .argb = GColorDarkCandyAppleRedARGB8 },
  { .argb = GColorJazzberryJamARGB8 },
  { .argb = GColorPurpleARGB8 },
  { .argb = GColorPurpureusARGB8 },
  { .argb = GColorRoseValeARGB8 },
  { .argb = GColorWindsorTanARGB8 },
  { .argb = GColorArmyGreenARGB8 },
  { .argb = GColorDarkGreenARGB8 },
  { .argb = GColorMidnightGreenARGB8 },
  { .argb = GColorCobaltBlueARGB8 },
  { .argb = GColorDukeBlueARGB8 },
  { .argb = GColorLibertyARGB8 },
  { .argb = GColorIndigoARGB8 },
  { .argb = GColorImperialPurpleARGB8 },
  { .argb = GColorBulgarianRoseARGB8 },
  { .argb = GColorOxfordBlueARGB8 },
};

// Variables

// - the window
Window *window = NULL;

// - graphic layers
Layer *minute_layer = NULL;
// Layer *second_layer = NULL;
// - paths
GPath *minuteSegment = NULL;
GPoint minuteSegmentPoints[4];
GPath *hourSegment = NULL;
GPoint hourSegmentPoints[4];

// - internal variables
int currentSecHexAngle;
int currentMinHexAngle;
int currentHourHexAngle;
int dateValid;
int minuteValid;

// global path info temp var
GPathInfo pathInfo;

void calc_segment(int aDiameter, GPoint *pp)
{
  int r = aDiameter/2;
  // create points
  // - center
  pp[0].x = 0;
  pp[0].y = 0;
  // - to twelve
  pp[1].x = 0;
  pp[1].y = -r;
  // - to one minute (6 degrees) after twelve
//   pp[2].x = 0.104528463267654 * r; // sin(8)
//   pp[2].y = -0.994521895368273 * r; // cos(6)
//   pp[2].x = 0.139173100960065 * r; // sin(8)
//   pp[2].y = -0.99026806874157 * r; // cos(8)
  pp[2].x = (r+3)/6; // enough overlap
  pp[2].y = -r+1;
  pp[3].x = 2; // to avoid invisible tip
  pp[3].y = 0;
  pathInfo.num_points = 4;
  pathInfo.points = pp;
}


void minute_layer_callback(struct Layer *layer, GContext *ctx)
{
  // draw 60 colored segments
  int i;
  for (i = 0; i<60; i++) {
    gpath_rotate_to(minuteSegment, currentMinHexAngle + i*TRIG_MAX_ANGLE/60);
    graphics_context_set_fill_color(ctx, minuteColors[i]);
    gpath_draw_filled(ctx, minuteSegment);
  }
  for (i = 0; i<60; i++) {
    gpath_rotate_to(hourSegment, currentHourHexAngle + i*TRIG_MAX_ANGLE/60);
//    graphics_context_set_fill_color(ctx, minuteColors[59-i]);
    graphics_context_set_fill_color(ctx, minuteColors[i]);
    gpath_draw_filled(ctx, hourSegment);
  }
}


// void second_layer_callback(struct Layer *layer, GContext *ctx)
// {
//   // second
//   gpath_rotate_to(secondPentagon, currentSecHexAngle);
//   graphics_context_set_fill_color(ctx, SECOND_P_COLOR);
//   gpath_draw_filled(ctx, secondPentagon);
//   #ifndef PBL_COLOR
//   // - b&w: need outline of second pentagon in hour pentagon color to visually separate from hour triangle
//   graphics_context_set_stroke_color(ctx, HOUR_P_COLOR);
//   gpath_draw_outline(ctx, secondPentagon);
//   #endif
// }


void update_hour_minute_angles(struct tm *tick_time)
{
  currentMinHexAngle = TRIG_MAX_ANGLE / 60 * tick_time->tm_min;
  currentHourHexAngle = TRIG_MAX_ANGLE / 60 * (tick_time->tm_hour*5 + tick_time->tm_min/12);
}

void update_second_angle(struct tm *tick_time)
{
  currentSecHexAngle = TRIG_MAX_ANGLE / 60 * tick_time->tm_sec;
}


// set this to 1 to quickly see all minute texts, one per second, to debug layout
// It also displays the LONGEST_HOUR_TEXT instead of the current hour
#define DEBUG_SECONDS_AS_MIN 0
#define DEBUG_SECONDS_AS_DAYS 0

// handle second tick
void handle_second_tick(struct tm *tick_time, TimeUnits units_changed)
{
  #if DEBUG_SECONDS_AS_DAYS
  dateValid = 0;
  #else
  if (units_changed & DAY_UNIT) dateValid = 0; // new day, needs new text
  #endif
  if (!dateValid) {
    // done
    dateValid = 1; // now valid
  }
  if (units_changed & MINUTE_UNIT) minuteValid = 0;
  if (!minuteValid) {
    // update minute and hour
    update_hour_minute_angles(tick_time);
    layer_mark_dirty(minute_layer);
    // done
    minuteValid = 1; // now valid
  }
//   anyway, update second
//   update_second_angle(tick_time);
//   layer_mark_dirty(second_layer);
}



void init()
{
  // the window
  window = window_create();
  window_set_background_color(window, GColorBlack);
  window_stack_push(window, true /* Animated */);

  // init the engine
  dateValid = 0; // text not valid
  minuteValid = 0; // hands not valid
  struct tm *t;
  time_t now = time(NULL);
  t = localtime(&now);
  // start with correct hands positions
//   update_second_angle(t);
  update_hour_minute_angles(t);

  // get the window frame
  GRect wf = layer_get_frame(window_get_root_layer(window));

  // graphic layer for hour and minute
  GRect tf = wf; // start calculation with window frame
  tf.origin.x += (tf.size.w-MINUTE_DIAMETER)/2;
  tf.size.w = MINUTE_DIAMETER;
  tf.origin.y += (tf.size.h-MINUTE_DIAMETER)/2;
  tf.size.h = MINUTE_DIAMETER;
  minute_layer = layer_create(tf);
  layer_set_update_proc(minute_layer, &minute_layer_callback);

//   - second
//   tf = wf; // start calculation with window frame
//   tf.origin.x += (tf.size.w-SECOND_DIAMETER)/2;
//   tf.size.w = SECOND_DIAMETER;
//   tf.origin.y += (tf.size.h-SECOND_DIAMETER)/2;
//   tf.size.h = SECOND_DIAMETER;
//   second_layer = layer_create(tf);
//   layer_set_update_proc(second_layer, &second_layer_callback);


  // calculate the basic paths
  // - minute
  GPoint mh_center = { x:MINUTE_DIAMETER/2, y:MINUTE_DIAMETER/2 };
  calc_segment(MINUTE_DIAMETER, minuteSegmentPoints);
  minuteSegment = gpath_create(&pathInfo);
  gpath_move_to(minuteSegment, mh_center);
  // - hour
  calc_segment(HOUR_DIAMETER, hourSegmentPoints);
  hourSegment = gpath_create(&pathInfo);
  gpath_move_to(hourSegment, mh_center);

  // add the layers
  // - hour_minute
  layer_add_child(window_get_root_layer(window), minute_layer);

  // trigger first update immediately
  layer_mark_dirty(minute_layer);
//   layer_mark_dirty(hour_layer);
//   layer_mark_dirty(second_layer);

  // now subscribe to ticks
  tick_timer_service_subscribe(SECOND_UNIT, &handle_second_tick);
}


void deinit()
{
//   layer_destroy(second_display_layer);
  layer_destroy(minute_layer);
  gpath_destroy(minuteSegment);
  window_destroy(window);
}



int main(void)
{
  init();
  app_event_loop();
  deinit();
  return 0;
}

