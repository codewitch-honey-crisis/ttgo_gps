#include <Arduino.h>

#include <gfx.hpp>

// #########################################################################
// Return a 16 bit rainbow colour
// #########################################################################
gfx::rgb_pixel<16> rainbow(byte value) {
    // Value is expected to be in range 0-127
    // The value is converted to a spectrum colour from 0 = blue through to 127 = red

    byte red = 0;    // Red is the top 5 bits of a 16 bit colour value
    byte green = 0;  // Green is the middle 6 bits
    byte blue = 0;   // Blue is the bottom 5 bits

    byte quadrant = value / 32;

    if (quadrant == 0) {
        blue = 31;
        green = 2 * (value % 32);
        red = 0;
    }
    if (quadrant == 1) {
        blue = 31 - (value % 32);
        green = 63;
        red = 0;
    }
    if (quadrant == 2) {
        blue = 0;
        green = 63;
        red = value % 32;
    }
    if (quadrant == 3) {
        blue = 0;
        green = 63 - 2 * (value % 32);
        red = 31;
    }
    return gfx::rgb_pixel<16>(red, green, blue);
}

// #########################################################################
//  Draw the meter on the screen, returns x coord of righthand side
// #########################################################################
template <typename Destination>
int ring_meter(Destination& dst, int value, int vmin, int vmax, int x, int y, int r, const char* units, const gfx::open_font& text_font, byte scheme) {
    using color_t = gfx::color<gfx::rgb_pixel<16>>;
    // Minimum value of r is about 52 before value text intrudes on ring
    // drawing the text first is an option

    x += r;
    y += r;  // Calculate coords of centre of ring

    int w = r / 4;  // Width of outer ring is 1/4 of radius

    int angle = 90;  // Half the sweep angle of meter (180 degrees)

    int text_colour = 0;  // To hold the text colour

    int v = map(value, vmin, vmax, -angle, angle);  // Map the value to an angle v

    byte seg = 5;  // Segments are 5 degrees wide = 60 segments for 300 degrees
    byte inc = 5;  // Draw segments every 5 degrees, increase to 10 for segmented ring

    // Draw colour blocks every inc degrees
    for (int i = -angle; i < angle; i += inc) {
        // Choose colour from scheme
        gfx::rgb_pixel<16> colour;
        switch (scheme) {
            case 0:
                colour = color_t::red;
                break;  // Fixed colour
            case 1:
                colour = color_t::green;
                break;  // Fixed colour
            case 2:
                colour = color_t::blue;
                break;  // Fixed colour
            case 3:
                colour = rainbow(map(i, -angle, angle, 0, 127));
                break;  // Full spectrum blue to red
            case 4:
                colour = rainbow(map(i, -angle, angle, 63, 127));
                break;  // Green to red (high temperature etc)
            case 5:
                colour = rainbow(map(i, -angle, angle, 127, 63));
                break;  // Red to green (low battery etc)
            default:
                colour = color_t::blue;
                break;  // Fixed colour
        }

        // Calculate pair of coordinates for segment start
        float sx = cos((i - 90) * 0.0174532925);
        float sy = sin((i - 90) * 0.0174532925);
        uint16_t x0 = sx * (r - w) + x;
        uint16_t y0 = sy * (r - w) + y;
        uint16_t x1 = sx * r + x;
        uint16_t y1 = sy * r + y;

        // Calculate pair of coordinates for segment end
        float sx2 = cos((i + seg - 90) * 0.0174532925);
        float sy2 = sin((i + seg - 90) * 0.0174532925);
        int x2 = sx2 * (r - w) + x;
        int y2 = sy2 * (r - w) + y;
        int x3 = sx2 * r + x;
        int y3 = sy2 * r + y;
        gfx::spoint16 points1[] = {
            gfx::spoint16(x0, y0),
            gfx::spoint16(x1, y1),
            gfx::spoint16(x2, y2),
        };
        gfx::spath16 tri1(sizeof(points1) / sizeof(gfx::spoint16), points1);
        gfx::spoint16 points2[] = {
            gfx::spoint16(x1, y1),
            gfx::spoint16(x2, y2),
            gfx::spoint16(x3, y3),
        };
        gfx::spath16 tri2(sizeof(points2) / sizeof(gfx::spoint16), points2);

        if (i < v) {  // Fill in coloured segments with 2 triangles
            gfx::draw::filled_polygon(dst, tri1, colour);
            gfx::draw::filled_polygon(dst, tri2, colour);
        } else  // Fill in blank segments
        {
            gfx::draw::filled_polygon(dst, tri1, color_t::gray);
            gfx::draw::filled_polygon(dst, tri2, color_t::gray);
        }
    }

    // Convert value to a string
    char buf[10];
    byte len = 4;
    if (value > 999) len = 5;
    dtostrf(value, len, 0, buf);
    gfx::open_text_info oti;
    oti.font = &text_font;
    oti.text = buf;
    
    // Print value, if the meter is large then use big font 6, othewise use 4
    if (r > 84) {
        oti.scale = oti.font->scale(30);
    } else {
        oti.scale = oti.font->scale(20);
    }
    gfx::srect16 tr = oti.font->measure_text(gfx::ssize16::max(),gfx::spoint16::zero(),oti.text,oti.scale).bounds();
    uint16_t xm = tr.width()/2,ym=tr.height()/2;
    tr.offset_inplace(x-5-xm,y-20-ym);
    gfx::draw::text(dst,tr,oti,color_t::white,color_t::black);
    // Print units, if the meter is large then use big font 4, othewise use 2
    if (r > 84) {
        oti.scale = oti.font->scale(20);
    } else {
        oti.scale = oti.font->scale(15);
    }
    oti.text = units;
    tr = oti.font->measure_text(gfx::ssize16::max(),gfx::spoint16::zero(),oti.text,oti.scale).bounds();
    xm = tr.width()/2;ym=tr.height()/2;
    tr.offset_inplace(x-xm,y+5-ym);
    gfx::draw::text(dst,tr,oti,color_t::white,color_t::black);
    // Calculate and return right hand side x coordinate
    return x + r;
}
