//includes
#include <ttgo.hpp>
#include <TinyGPSPlus.h>
#include <draw_util.hpp>
// downloaded from fontsquirrel.com and header generated with 
// https://honeythecodewitch.com/gfx/generator
#include <fonts/Telegrama.hpp>

static const open_font& text_font = Telegrama;

// the frame buffer used for double buffering
using frame_buffer_t = bitmap<typename lcd_t::pixel_type>;
static uint8_t frame_buffer_data[
    frame_buffer_t::sizeof_buffer(
        {lcd_t::base_width,lcd_t::base_height}
    )
];

// The TinyGPSPlus object
TinyGPSPlus gps;

// temporary string
static char tmpsz[256];

static uint32_t ts=0;

void setup() {
    Serial.begin(115200);
    // init the ttgo
    ttgo_initialize();
    // landscape mode, buttons on right
    lcd.rotation(1);
    // get a bitmap over our frame buffer
    frame_buffer_t frame_buffer(
        lcd.dimensions(),
        frame_buffer_data);
    draw::filled_rectangle(frame_buffer,frame_buffer.bounds(),color_t::black);
    ring_meter(frame_buffer,100,0,120,25,15,100,"km/h",Telegrama,3);
    draw::bitmap_async(lcd,lcd.bounds(),frame_buffer,frame_buffer.bounds());
    // set the button callbacks 
    // (already initialized via ttgo_initialize())
    //button_a.on_click(on_click_handler);
    //button_b.on_click(on_click_handler);
    
}
void loop() {
  
    // we don't use the dimmer, so make sure it doesn't timeout
    dimmer.wake();
    // give the TTGO hardware a chance to process
    ttgo_update();
}