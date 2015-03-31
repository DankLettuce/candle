//Gavin McMahon
//March 6 2015
//Adafruit neopixel_counter candle code
//Arduino UNO

#include <Adafruit_NeoPixel.h>
#define PIN 7

const int NUMBER_OF_PIXELS = 90;
const int MAX_RGB_VAL = 255;

float current_rgb[3];
float future_rgb[3];
char INDEX_TO_COLOR_CHAR = {'r', 'g', 'b'};
int MINIMUM_VALS = {10, 5, 0};

float *delta_rgb[3];
for(int index=0; index<3; index++){
    delta_rgb[index] = new float[NUMBER_OF_PIXELS];
}

uint8_t past_rgb[3];

int random_rgb[3];

int new_flame_direction;
int steps_to_future_color;
int shift_pixels;
int flame_layers = NUMBER_OF_PIXELS / 2;

int pixel_counter;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMBER_OF_PIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {

  strip.begin();
  strip.show();
}

void loop() {
    random_future_color();
    future_flame_direction();

    steps_to_future_color = (steps_to_future_color + random(5,26)) / 2;

    for(int j = 0; j < NUMBER_OF_PIXELS; j++){
        pixel_counter = j;
        shift_pixels = (j + new_flame_direction) % NUMBER_OF_PIXELS;
        future_color_gradient_across_layers();
        change_in_color_per_pixel_per_step();
    }
    fade_from_past_to_future();
}


uint8_t read_current_color (uint32_t c, char value){
    switch (value) {
        case 'r':
            return (uint8_t)(c >> 16);
        case 'g':
            return (uint8_t)(c >>  8);
        case 'b':
            return (uint8_t)(c >>  0);
        default:
            return 0;
    }
}


void future_flame_direction(){

    int old_flame_direction = new_flame_direction;
    new_flame_direction = random(NUMBER_OF_PIXELS);

    if(old_flame_direction - new_flame_direction > flame_layers || new_flame_direction - old_flame_direction > flame_layers){
        new_flame_direction = ((old_flame_direction + new_flame_direction + NUMBER_OF_PIXELS) / 2) % NUMBER_OF_PIXELS;
    }
    else{
        new_flame_direction = (new_flame_direction + old_flame_direction) / 2;
    }
}


void random_future_color(){
    random_rgb[0]=random(210,245);
    random_rgb[1]=random(75,110);
    random_rgb[2]=random(5,20);
}


void future_color_gradient_across_layers(){

    for(int color_index = 0; color_index < 3; color_index++){
        char color_char = INDEX_TO_COLOR_CHAR[color_index];
        past_rgb[color_index] = read_current_color(strip.getPixelColor(shift_pixels), color_char);

        if(pixel_counter <= flame_layers){
            future_rgb[color_index] = MINIMUM_VALS[color_index] + (past_rgb[color_index] + (random_rgb[color_index] - (random_rgb[color_index] / flame_layers * pixel_counter))) / 2;
        }
        else {
            future_rgb[color_index] = MINIMUM_VALS[color_index] + (past_rgb[color_index] + (random_rgb[color_index] - ((random_rgb[color_index] / flame_layers) * (flame_layers - (pixel_counter - flame_layers))))) / 2;
        }
    }
}


void change_in_color_per_pixel_per_step(){
    for(int color_index = 0; color_index < 3; color_index++){
        delta_rgb[color_index] = (future_rgb[color_index] - past_rgb[color_index]) / steps_to_future_color;
    }
}


void fade_from_past_to_future(){

    for(int q = 1; q <= steps_to_future_color; q++){
        for(int n = 0; n < NUMBER_OF_PIXELS; n++){
            for(int color_index = 0; color_index < 3; color_index++){

                char color_char = INDEX_TO_COLOR_CHAR[color_index];
                past_rgb[color_index] = read_current_color(strip.getPixelColor(n), color_char);

                future_rgb[color_index] = past_rgb[color_index] - delta_rgb[color_index][n] * (q - 1);
                current_rgb[color_index] = future_rgb[color_index] + delta_rgb[color_index][n] * q;

                if(current_rgb[color_index] < MINIMUM_VALS[color_index]){
                    current_rgb[color_index] = MINIMUM_VALS[color_index];
                }
                if(current_rgb[color_index] > MAX_RGB_VAL){
                    current_rgb[color_index] = MAX_RGB_VAL;
                }

            }

            strip.setPixelColor(n, (int)current_rgb[0], (int)current_rgb[1], (int)current_rgb[2]);
        }
        strip.show();
    }
}
