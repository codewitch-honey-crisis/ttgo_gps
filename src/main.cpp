//includes
#include <ttgo.hpp>
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

// temporary strings for formatting
static char tmpsz1[256];
static char tmpsz2[256];
static char tmpsz3[256];

static uint32_t ts=0;

// draw text in center of screen
static void draw_center_text(const char* text, int y, int size=30) {
    // finish any pending async draws
    draw::wait_all_async(lcd);

    // get a bitmap over our frame buffer
    frame_buffer_t frame_buffer(
        lcd.dimensions(),
        frame_buffer_data);
    // clear it to purple
    draw::filled_rectangle(
        frame_buffer,
        frame_buffer.bounds(),
        color_t::black);

    // fill the text structure
    open_text_info oti;
    oti.font = &text_font;
    oti.text = text;
    // scale the font to the right line height
    oti.scale = oti.font->scale(size);
    // measure the text
    srect16 txtr = oti.font->measure_text(
                                ssize16::max(),
                                spoint16::zero(),
                                oti.text,
                                oti.scale).bounds();
    // center what we got back
    txtr.center_horizontal_inplace((srect16)frame_buffer.bounds());
    txtr.offset(0,y);
    // draw it to the frame buffer
    draw::text(frame_buffer,txtr,oti,color_t::white);

    // asynchronously send the frame buffer to the LCD (uses DMA)
    draw::bitmap_async(
                    lcd,
                    lcd.bounds(),
                    frame_buffer,
                    frame_buffer.bounds());
}

void setup() {
    Serial.begin(115200);
    // init the ttgo
    ttgo_initialize();
    // landscape mode, buttons on right
    lcd.rotation(1);

    // set the button callbacks 
    // (already initialized via ttgo_initialize())
    //button_a.on_click(on_click_handler);
    //button_b.on_click(on_click_handler);
    
}
void loop() {
    // trivial timer
    uint32_t ms = millis();
    // ten times a second...
    if(ms>ts+100) {
        ts = ms;
        
    }
    // we don't use the dimmer, so make sure it doesn't timeout
    dimmer.wake();
    // give the TTGO hardware a chance to process
    ttgo_update();
}