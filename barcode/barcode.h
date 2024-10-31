// barcode.h
#ifndef barcode_H
#define barcode_H



#define IR_SENSOR_PIN 26
#define RESET_BUTTON_PIN 20  // GP20 for reset button
#define SAMPLE_SIZE 10    
#define THRESHOLD 0.5     
#define BARS_PER_CHAR 9   
#define WIDE_BARS_PER_CHAR 3  
#define OUTLIER_RATIO 5.0  // If largest width is this many times bigger than second largest, it's an outlier
#define MAX_MESSAGE_LENGTH 50


extern char decoded_message[MAX_MESSAGE_LENGTH];
extern int message_length;

void init_gpio();
void reset_message();
void add_to_message(char decoded_char);
void track_bars();
void init_barcode();
#endif // barcode.h
