#include <string.h>
#include <stdbool.h>
#include "pragmas.h"

typedef struct{
    volatile unsigned int occupied;
    volatile unsigned int reserved;
    uint8_t entry_time;
    volatile int car_plate;
}parking_slot;

volatile unsigned int i = 0;
// ============================ //
//          GLOBALS             //
// ============================ //
char level_to_show = 0;

volatile unsigned int prev_rb4 = 0;
volatile unsigned int current_rb4 = 0;
volatile unsigned int counter_for_500ms=0;
volatile unsigned int result = 0;

 unsigned int number_plate = 0;
volatile unsigned int level = 0;
unsigned int location = 0;

volatile unsigned int run_program = 0;


volatile uint8_t index = 0;
volatile unsigned int flag1 = 0;
volatile unsigned int accumulated_money = 0;

parking_slot parking_lot[4][10];

volatile unsigned long counter_for_fee = 0;
uint8_t counter_for_100ms = 0;

uint8_t empty_spaces=40;

uint8_t empty_in_A = 10;
uint8_t empty_in_B = 10;
uint8_t empty_in_C = 10;
uint8_t empty_in_D = 10;



uint8_t subscription_fee=50;

inline void disable_rxtx(void) { PIE1bits.RC1IE = 0; PIE1bits.TX1IE = 0; }
inline void enable_rxtx(void)  { PIE1bits.RC1IE = 1; PIE1bits.TX1IE = 1; }





const unsigned char segment_map[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

char rcvd_string[10] = {0};
uint8_t display_mode = 0; // 0 = money, 1 = level


typedef enum {INBUF = 0, OUTBUF = 1} buf_t;

#define BUFSIZE 128        /* Static buffer size. Maximum amount of data */
uint8_t inbuf[BUFSIZE];   /* Preallocated buffer for incoming data */
uint8_t outbuf[BUFSIZE];  /* Preallocated buffer for outgoing data  */
uint8_t head[2] = {0, 0}; /* head for pushing, tail for popping */
uint8_t tail[2] = {0, 0};



#define CAR_QUEUE_SIZE 16

volatile int carQueue[CAR_QUEUE_SIZE];
volatile uint8_t car_head = 0;
volatile uint8_t car_tail = 0;



#pragma interrupt_level 2 // Prevents duplication of function
uint8_t buf_isempty( buf_t buf ) { return (head[buf] == tail[buf])?1:0; }

void buf_push( uint8_t v, buf_t buf) {
    if (buf == INBUF) inbuf[head[buf]] = v;
    else outbuf[head[buf]] = v;
    head[buf]++;
    if (head[buf] == BUFSIZE) head[buf] = 0;
}

#pragma interrupt_level 2 // Prevents duplication of function
uint8_t buf_pop( buf_t buf ) {
    uint8_t v;
    if (buf_isempty(buf)) {

    } else {
        if (buf == INBUF) v = inbuf[tail[buf]];
        else v = outbuf[tail[buf]];
        tail[buf]++;
        if (tail[buf] == BUFSIZE) tail[buf] = 0;
        return v;
    }
}

uint8_t car_isempty() {
    return (car_head == car_tail) ? 1 : 0;
}

uint8_t car_isfull() {
    return ((car_head + 1) % CAR_QUEUE_SIZE == car_tail) ? 1 : 0;
}

void enqueueCar(int plate_number) {

    if (!car_isfull()) {
        carQueue[car_head] = plate_number;
        car_head = (car_head + 1) % CAR_QUEUE_SIZE;
    }
    return;
}

int dequeueCar() {
    if (!car_isempty()) {
        int plate_number = carQueue[car_tail];
        car_tail = (car_tail + 1) % CAR_QUEUE_SIZE;

        return plate_number;
    }
    return -1; // Indicates queue was empty
}


// ============================ //
//          FUNCTIONS           //
// ============================ //


//Initializations

void init_ports(){

    TRISJ=0x00;
    LATJ=0x00;
    PORTJ=0x00;

    TRISB = 0x10;
    LATB = 0x00;
    PORTB = 0x00;

    TRISH = 0x00;
    LATH = 0x00;
    PORTH = 0x00;
    PORTDbits.RD1 = 1;
    return;

}

void init_interrupt(){

    RCON = 0x00;

    INTCON = 0x00;
    INTCONbits.GIE = 1;
    INTCONbits.TMR0IE = 1;
    INTCONbits.RBIE = 1;
    INTCONbits.PEIE = 1;



    T0CON = 0x00;
    T0CONbits.TMR0ON = 1;
    T0CONbits.PSA = 1;


        TMR0H=0x3C;
        TMR0L=0xC3;

    PORTDbits.RD2 = 1;
    return;

}

void init_usart(){

    TRISCbits.RC7=1;
    TRISCbits.RC6=0;

    TXSTA1 = 0x00;
    TXSTA1bits.SYNC = 0; //SYNC MODE
    TXSTA1bits.TXEN = 0; //TRANSM?TTER ENABLES
    TXSTA1bits.BRGH = 1; //H?GH SPEED MODE
    TXSTA1bits.TX9 = 0; //8 bit

    RCSTA1 = 0x00;
    RCSTA1bits.SPEN=1; // serial port enabled
    RCSTA1bits.CREN = 1; //cont recieve enabled
    RCSTA1bits.RX9 = 0; //8 bit
    RCSTA1bits.SREN = 0;

    BAUDCON1bits.BRG16 = 0;
    SPBRG1 = 21;                                                       //--------------------------------------------------------

    PIE1bits.TX1IE = 1;
    PIE1bits.RC1IE = 1;
    PIR1 = 0x00;
    PORTDbits.RD4 = 1;
    return;

}

void init_adc(){

    TRISH=0x10;   //portH4 set as input

    ADCON0 = 0x31; // Channel 12; Turn on AD Converter
    ADCON1 = 0x00; // All analog pins
    ADCON2 = 0xAA; // Right Align | 12 Tad | Fosc/32
    ADRESH = 0x00;
    ADRESL = 0x00;

    PIR1bits.ADIF=0;
    PIE1bits.ADIE=1;
    PORTDbits.RD5 = 1;
    return;

}

void init_state(){

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 10; j++) {
            parking_lot[i][j].occupied = 0;
            parking_lot[i][j].reserved = 0;
            parking_lot[i][j].car_plate = -1;
            parking_lot[i][j].entry_time = -1;
        }
    }


return;
}




// ============================ //
//   INTERRUPT SERVICE ROUTINE  //
// ============================ //

void increment_empty_spaces_of_level(char level){
    if(level == 'A'){
        empty_in_A++;
    }
    else if(level == 'B'){
        empty_in_B++;
    }
    else if(level == 'C'){
        empty_in_C++;
    }
    else if(level == 'D'){
        empty_in_D++;
    }
}

void decrement_empty_spaces_of_level(char level){
    if(level == 'A'){
        empty_in_A--;
    }
    else if(level == 'B'){
        empty_in_B--;
    }
    else if(level == 'C'){
        empty_in_C--;
    }
    else if(level == 'D'){
        empty_in_D--;
    }
}


int empty_spaces_of_level(char level){
    if(level == 'A'){
        return empty_in_A;
    }
    else if(level == 'B'){
        return empty_in_B;
    }
    else if(level == 'C'){
        return empty_in_C;
    }
    else if(level == 'D'){
        return empty_in_D;
    }
}




/*
void update_timings(int i, int j){

    parking_lot[i][j].entry_time = counter_for_fee;

    return;
}*/



void enqueueReserveSuccessMsg() {


    disable_rxtx();

    buf_push('$', OUTBUF);
    buf_push('R', OUTBUF);
    buf_push('E', OUTBUF);
    buf_push('S', OUTBUF);
    buf_push(('0' + (number_plate / 100) % 10), OUTBUF);
    buf_push(('0' + (number_plate / 10) % 10), OUTBUF);
    buf_push(('0' + (number_plate % 10)), OUTBUF);
    buf_push('5', OUTBUF);
    buf_push('0', OUTBUF);
    buf_push('#', OUTBUF);

    enable_rxtx();

    return;

}


void enqueueReserveUnsuccessMsg() {


    disable_rxtx();

    buf_push('$', OUTBUF);
    buf_push('R', OUTBUF);
    buf_push('E', OUTBUF);
    buf_push('S', OUTBUF);
    buf_push(('0' + (number_plate / 100) % 10), OUTBUF);
    buf_push(('0' + (number_plate / 10) % 10), OUTBUF);
    buf_push(('0' + (number_plate % 10)), OUTBUF);
    buf_push('0', OUTBUF);
    buf_push('0', OUTBUF);
    buf_push('#', OUTBUF);

    enable_rxtx();

    return;

}

void enqueueParkMsg() {

    disable_rxtx();

    buf_push('$', OUTBUF);
    buf_push('S', OUTBUF);
    buf_push('P', OUTBUF);
    buf_push('C', OUTBUF);
    buf_push(('0' + (number_plate / 100) % 10), OUTBUF);
    buf_push(('0' + (number_plate / 10) % 10), OUTBUF);
    buf_push(('0' + (number_plate % 10)), OUTBUF);
    buf_push(level, OUTBUF);
    buf_push(('0' + (location / 10) % 10), OUTBUF);
    buf_push(('0' + (location % 10)), OUTBUF);
    buf_push('#', OUTBUF);

    enable_rxtx();

    return;

}

void enqueueFeeMsg(int fee) {

    disable_rxtx();

    buf_push('$', OUTBUF);
    buf_push('F', OUTBUF);
    buf_push('E', OUTBUF);
    buf_push('E', OUTBUF);
    buf_push(('0' + (number_plate / 100) % 10), OUTBUF);
    buf_push(('0' + (number_plate / 10) % 10), OUTBUF);
    buf_push(('0' + (number_plate % 10)), OUTBUF);
    buf_push(('0' + (fee / 100) % 10), OUTBUF);
    buf_push(('0' + (fee / 10) % 10), OUTBUF);
    buf_push(('0' + (fee % 10)), OUTBUF);
    buf_push('#', OUTBUF);

    enable_rxtx();

    return;
}




int calculate_fee(int i, int j){
    int money = 0;
        if(parking_lot[i][j].car_plate == number_plate && parking_lot[i][j].reserved == 0){
            money = ((counter_for_fee - parking_lot[i][j].entry_time)/50 + 1);
            accumulated_money = accumulated_money + money;
            return money;
        }
        else if (parking_lot[i][j].car_plate == number_plate && parking_lot[i][j].reserved == 1){
            return 0;
    }
}
int flag = 0;

void recieved_go(){
    PORTEbits.RE0=1;
    run_program = 1;

return;

}

void recieved_end(){
    run_program = 0;
    PORTH=0x00;
    return;
}

void recieved_prk() {

    if(run_program == 1){
        int found = 0;


        for (uint8_t i = 0; i < 4 && !found && empty_spaces!=0 ; i++) {           // Levels A-D
            for (uint8_t j = 0; j < 10 && !found && empty_spaces!=0; j++) {      // Spots 00-09

                if (parking_lot[i][j].car_plate == number_plate && parking_lot[i][j].reserved == 1 && parking_lot[i][j].occupied == 0){
                    parking_lot[i][j].occupied = 1;
                    level = 'A' + i;
                    location = j+1;
                   // parking_lot[i][j].entry_time = counter_for_fee;
                    empty_spaces--;
                    decrement_empty_spaces_of_level(level);
                    found = 1;
                }
            }
        }


        if(found == 0){
        // Search for the first available, unreserved, unoccupied spot
            for (uint8_t i = 0; i < 4 && !found && empty_spaces!=0; i++) {           // Levels A-D
                for (uint8_t j = 0; j < 10 && !found && empty_spaces!=0; j++) {      // Spots 00-09

                    if (parking_lot[i][j].occupied == 0 && parking_lot[i][j].reserved == 0) {
                        parking_lot[i][j].occupied = 1;
                        parking_lot[i][j].car_plate = number_plate;
                        parking_lot[i][j].entry_time = counter_for_fee;
                        level = 'A' + i;
                        location = j+1;


                        empty_spaces--;
                        decrement_empty_spaces_of_level(level);
                        found = 1;
                    }
                }
            }
        }



        if (found) {
            // Format level character from index (0=A, 1=B, ...)

            // Prepare SPC message in $SPCXXXYZZ# format
            enqueueParkMsg();



        }
        else {
            // No spot found, add to waiting queue
            enqueueCar(number_plate);
        }
    }
    return;
}

void recieved_ext(){

    int found1= 0;
    if(run_program == 1){
        for (int i = 0; i < 4 && !found1; i++) {
            for (int j = 0; j < 10 && !found1; j++) {
                if(parking_lot[i][j].car_plate == number_plate){

                    if(parking_lot[i][j].reserved == 1){
                        parking_lot[i][j].occupied = 0;
                        level = 'A' + i;
                        //location = j+1;
                        empty_spaces++;
                        increment_empty_spaces_of_level(level);
                        enqueueFeeMsg(0);
                        found1=1;
                    }

                    else{
                        int calculated_fee = calculate_fee(i,j);
                        enqueueFeeMsg(calculated_fee);
                        //accumulated_money = accumulated_money + calculated_fee;
                        parking_lot[i][j].occupied = 0;
                        parking_lot[i][j].entry_time = -1;
                        parking_lot[i][j].car_plate = -1;
                        level = 'A' + i;
                        //location = j+1;
                        empty_spaces++;
                        increment_empty_spaces_of_level(level);

                         found1=1;
                        if(car_isempty() == 0){
                            number_plate = dequeueCar();
                            parking_lot[i][j].occupied = 1;
                            parking_lot[i][j].entry_time = counter_for_fee;
                            parking_lot[i][j].car_plate = number_plate;
                            parking_lot[i][j].reserved = 0;
                            level = 'A' + i;
                            location = j+1;
                            empty_spaces--;
                            decrement_empty_spaces_of_level(level);
                           // update_timings(i,j);
                            enqueueParkMsg();
                    }





                    }

                }
            }
        }
    }
    return;
}


void recieved_sub(){

    if(run_program == 1){
        int level_index = level - 'A';


        if (parking_lot[level_index][location-1].occupied == 0 && parking_lot[level_index][location-1].reserved == 0) {

                parking_lot[level_index][location-1].car_plate = number_plate;
                parking_lot[level_index][location-1].reserved = 1;

                accumulated_money = accumulated_money+50;
                enqueueReserveSuccessMsg();


        }
        else{
            enqueueReserveUnsuccessMsg();
        }

    }
}


void receive_string() {


    disable_rxtx();

    char ch;

    ch = RCREG1;  // Read character

    if (ch == '$') {
        i = 0;
    }

    else if (ch == '#') {
       // rcvd_string[i] = '\0';
        flag = 1;
    }


    else   { rcvd_string[i++] = ch;}


    enable_rxtx();
    return;
}




void process_command() {

        flag = 0;
        if (rcvd_string[0] =='G' && rcvd_string[1] =='O') {
            recieved_go();
        }
        else if (rcvd_string[0] == 'E' && rcvd_string[1] =='N' && rcvd_string[2] =='D') {
            recieved_end();
        }
        else if (rcvd_string[0] == 'P' && rcvd_string[1] =='R' && rcvd_string[2] =='K') {
            // Handle PRKXXX
            char prk_num[4] = {0};  // 3 digits + null terminator
            prk_num[0] = rcvd_string[3];
            prk_num[1] = rcvd_string[4];
            prk_num[2] = rcvd_string[5];


            number_plate = atoi(prk_num);
            recieved_prk();
        }
        else if (rcvd_string[0] == 'E' && rcvd_string[1] =='X' && rcvd_string[2] =='T') {
            // Handle EXTXXX
            char ext_num[4] = {0};
            ext_num[0] = rcvd_string[3];
            ext_num[1] = rcvd_string[4];
            ext_num[2] = rcvd_string[5];
            number_plate = atoi(ext_num);
            recieved_ext();
        }
        else if (rcvd_string[0] == 'S' && rcvd_string[1] =='U' && rcvd_string[2] =='B') {

            char sub_num[4] = {0};
            sub_num[0] = rcvd_string[3];
            sub_num[1] = rcvd_string[4];
            sub_num[2] = rcvd_string[5];
           // sub_num[3] = '\0';
            number_plate = atoi(sub_num);

            level = rcvd_string[6];

            char location_str[3] = {0};
            location_str[0] = rcvd_string[7];
            location_str[1] = rcvd_string[8];
            //location_str[2] = '\0';
            location = atoi(location_str);

            recieved_sub();
        }


    return;
}






void receive_isr() {
    PIR1bits.RC1IF = 0;
    receive_string();

    if (flag==1){
    process_command();}




return;
}




void transmit_isr() {

    PIR1bits.TX1IF = 0;    // Acknowledge interrupt
    const char msg = buf_pop(OUTBUF);
    PORTEbits.RE4=1;
    TXREG1 = msg;

    if (msg == '#') {
        while (!TXSTAbits.TRMT);
        TXSTA1bits.TXEN = 0;
    }

    return;
}




void toggle_7_segment_display(){

    if(display_mode == 1){
        display_mode = 0;
    }
    else if(display_mode == 0){
        display_mode = 1;
    }
    return;
}


void is_rb4_released(){

    current_rb4 = PORTBbits.RB4;

    if (prev_rb4 == 1 && (current_rb4 == 0)) {

        toggle_7_segment_display();
    }

    prev_rb4 = current_rb4;
    return;
}









void enqueue_default_msg() {
    PORTEbits.RE1=1;

    disable_rxtx();

    buf_push('$', OUTBUF);
    buf_push('E', OUTBUF);
    buf_push('M', OUTBUF);
    buf_push('P', OUTBUF);
    buf_push(('0' + (empty_spaces / 10)), OUTBUF);
    buf_push(('0' + (empty_spaces % 10)), OUTBUF);
    buf_push('#', OUTBUF);

    enable_rxtx();

    return;
}



void determine_level(){
    if(result < 256){
        level_to_show = 'A';
    }
    else if (result < 512){
        level_to_show = 'B';
    }
    else if (result < 768){
        level_to_show = 'C';
    }
    else{
        level_to_show = 'D';
    }return;
}


void seven_segment_D0()
{
    PORTHbits.RH3=1;
    if(display_mode == 1){
        LATJ = segment_map[empty_spaces_of_level(level_to_show) % 10];
    }
    else{
        LATJ=segment_map[accumulated_money%10];
    }return;
}
void seven_segment_D1()
{
    PORTHbits.RH2=1;
    if(display_mode == 1){
        LATJ = segment_map[((empty_spaces_of_level(level_to_show))/10) % 10];
    }
    else{

        LATJ=segment_map[(accumulated_money/10)%10];

    }return;
}
void seven_segment_D2()
{
    if(display_mode == 1){
        PORTHbits.RH1=0;
    }
    else{
        PORTHbits.RH1=1;
        LATJ=segment_map[(accumulated_money/100)%10];
    }return;
}
void seven_segment_D3()
{
    if(display_mode == 1){
        PORTHbits.RH0=0;
    }
    else{
        PORTHbits.RH0=1;
        LATJ=segment_map[(accumulated_money/1000)%10];
    }return;
}
volatile unsigned int current_digit=0;
uint8_t flag3=0;
void __interrupt(high_priority) highPriorityISR(void) {


    if (PIR1bits.RC1IF){
        receive_isr();
    }

    if (PIR1bits.TX1IF) {
        if(run_program == 1) transmit_isr();
    }



    if(INTCONbits.RBIF){
        PORTAbits.RA7 = 1;
        is_rb4_released();
        INTCONbits.RBIF = 0;
    }





    if(INTCONbits.TMR0IF == 1){
        INTCONbits.TMR0IF = 0;

        TMR0H=0x3C;
        TMR0L=0xC3;
        counter_for_fee++;



        counter_for_500ms++;
        if (counter_for_500ms == 100){
            counter_for_500ms = 0;
            GODONE=1;
        }

        counter_for_100ms++;
        if(counter_for_100ms == 20){
            counter_for_100ms = 0;
                if(run_program == 1){

                    flag3=1;

            }
        }



       PORTH=0x00;

        if(run_program == 1){


            if(current_digit==0)
            {
                seven_segment_D0();
            }
            else if(current_digit==1)
            {
                seven_segment_D1();
            }
            else if(current_digit==2)
            {
                seven_segment_D2();
            }
            else if(current_digit==3)
            {
                seven_segment_D3();
            }

            current_digit=(current_digit+1)%4;
        }
    }

    if(PIR1bits.ADIF){
        PIR1bits.ADIF=0;
        result = (ADRESH << 8) + ADRESL;
        determine_level();
    }
}


// ============================ //
//            MAIN              //
// ============================ //
void main()
{
    init_ports();
    init_interrupt();
    init_state();
    init_usart();
    init_adc();

    TRISE = 0;

    while(1){
        if (flag3){
            if (buf_isempty(OUTBUF)) {

                enqueue_default_msg();

            }
                TXSTA1bits.TXEN =1;

                flag3=0;
            }

        }
    }
