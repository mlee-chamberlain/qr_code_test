/**
 * @file lcd_driver.c
 *
 * @brief LCD Display driver for 5 predefined lines
 * @author Ilya Mushkirov
 *
 * @copyright 2022 The Chamberlain Group, LLC. All rights reserved. All
 * information within this file and associated files, including all
 * information and files transmitted with this file are CONFIDENTIAL
 * and the proprietary property of The Chamberlain Group, LLC.
 *
 * In using the Licensed Software, Company shall not use with, combine, or
 * incorporate any viral open source software with or into any of the Licensed
 * Software in a manner that would require any portion of the Licensed software to
 * be (i) disclosed or distributed in source code form; (ii) licensed for the
 * purpose of making derivative works; or (iii) distributable or redistributable
 * at no charge.
 */

#include <string.h>
#include <stdlib.h>

#include <lcd_font_4_22.h>
#include "lcd_spi.h"
#include "base_sercomm_driver.h"
#include "lcd_gpio.h"
#include "lcd.h"

#include "em_common.h"
#include "sl_sleeptimer.h"
#include "lcd_font_4_22.h"
/*--------------------------- UC1601s display driver for 5 predefined lines: -----------------------------------*/

typedef struct
{
    uint8_t line[NUM_PIX_COL_PER_ROW_BYTES];
    uint8_t state;
} line_def;

typedef struct
{
    bool    is_blinking;
    uint8_t my_char;
    uint8_t state;
    uint8_t buffer_shift;
    uint8_t line_size;
    uint8_t position;
} char_context_t;

static base_driver * lcd_sercomm_instance;

#define LCD_SET_COL_L 0x00 // set column address LSB, CA[3:0]
#define LCD_SET_COL_H 0x10 // set column address MSB, CA[7:4]
#define LCD_SET_TC 0x24    // set temperature compensation, TC[1:0]
#define LCD_SET_POWER 0x28 // set power control, PC[2:0]
#define LCD_SET_ADV 0x30   // set advanced program control, APC[R][7:0]
#define LCD_SET_SL 0x40    // set scroll line, SL[5:0]
#define LCD_SET_PAGE 0xB0  // set page address, PA[3:0]
#define LCD_SET_BIAS 0x81  // set Vbias, PM[7:0]
#define LCD_SET_PART 0x84  // set partial display control, LC[4]
#define LCD_SET_RAMA 0x88  // set RAM address control, AC[2:0]
#define LCD_SET_FR 0xA0    // set frame rate, LC[3]
#define LCD_SET_PON 0xA4   // set all pixel on, DC[1]
#define LCD_SET_INV 0xA6   // set inverse display, DC[0]
#define LCD_SET_EN 0xAE    // set display enable, DC[2]
#define LCD_SET_MAP 0xC0   // set LCD mapping control, LC[2:1]
#define LCD_RESET 0xE2     // system reset
#define LCD_NOP 0xE3       // NOP
#define LCD_SET_TEST 0xE4  // set test control
#define LCD_SET_BR 0xE8    // set LCD bias ratio, BR[1:0]
#define LCD_SET_CEN 0xF1   // set COM end, CEN[6:0]
#define LCD_SET_DST 0xF2   // set partial display start, DST[6:0]
#define LCD_SET_DEN 0xF3   // set partial display end, DEN[6:0]

// TC[1:0]:

#define LCD_TC0 0x00 // TC[1:0] = 0: temp. coeff.  -0.05%/°C (default)
#define LCD_TC1 0x01 // TC[1:0] = 1: temp. coeff.  -0.10%/°C
#define LCD_TC2 0x02 // TC[1:0] = 2: temp. coeff.  -0.15%/°C
#define LCD_TC3 0x03 // TC[1:0] = 3: temp. coeff.  -0.00%/°C

// PC[2:0]

#define LCD_15N 0x00      // PC[0] = 0, panel loading < 15 nF (default)
#define LCD_24N 0x01      // PC[0] = 1, panel loading 15 - 24 nF
#define LCD_EXT_VLCD 0x00 // PC[2:1] = 00, external Vlcd
#define LCD_INT_VLCD 0x06 // PC[2:1] = 11, internal Vlcd (x7) (default)

// LC[4:1]:

#define LCD_MX 0x02     // LC[1] = 1, X mirror
#define LCD_MY 0x04     // LC[2] = 1, Y mirror
#define LCD_80FPS 0x00  // LC[3] = 0, 80 frames per second (default)
#define LCD_100FPS 0x01 // LC[3] = 1, 100 frames per second
#define LCD_DIS_PD 0x00 // LC[4] = 0, disable partial display,
// mux rate = CEN + 1 (DST, DEN not used)
#define LCD_ENA_PD 0x01 // LC[4] = 1, enable partial display,
// mux rate = DEN - DST + 1

// AC[2:0]:

#define LCD_WA 0x01   // AC[0] = 1, wrap around
#define LCD_AINC 0x02 // AC[1] = 1, auto increment
#define LCD_PID 0x04  // AC[2] = 1, page auto increment direction

// DC[2:0]:

#define LCD_INVERSE 0x01 // DC[0] = 1, inverse display
#define LCD_ALL_ON 0x01  // DC[1] = 1, all pixels on
#define LCD_ENABLE 0x01  // DC[2] = 1, enable display

// BR[1:0]:

#define LCD_BR_6 0x00 // BR[1:0] = 0: bias ratio = 6
#define LCD_BR_7 0x01 // BR[1:0] = 1: bias ratio = 7
#define LCD_BR_8 0x02 // BR[1:0] = 2: bias ratio = 8
#define LCD_BR_9 0x03 // BR[1:0] = 3: bias ratio = 9 (default)

#define LCD_INIT_TIMEOUT (10) // Timeout for the display initialization sequence
#define LINE_INVERT_MASK_DEF (0xFFFF) // Inversion mask for 2 bytes line definition
#define LAST_ASCII_CHAR_DEF (127) // Definiton for characters end scope
#define BLINKING_TASK_DELAY (900) // Delay definition for blinking task timeout
#define ASCII_CHAR_ENABLED (0xFF) // Definition for enabled byte value
#define PIXELS_BEF_RIGHT_BUTTON (3) //definition for pixels before rightmost button
#define INVERTED_LINE_GAP ((LCD_LINE_PIXEL_HEIGHT - CHARACTER_HEIGHT)/2) // Defintion for inverted line gap between character and a border of line

#define LEFT_ALIGNMENT_BYTE (0) // Definition of number left alignment thing/button
#define RIGHT_ALIGNMENT_BYTE (LCD_CHAR_NUM - 1) // Definition of number right alignment thing/button
#define FORMAT_BYTE_CHAR (1) // Definition of number format byte position
#define PRINT_LINE_CHARS (2) // Definition start number of printable characters
#define BIT_SHIFT_COMPENSATION (2)
#define SCREEN_UPDATE_INTERLEAVE_CNT (8) // It requires to not update lcd buffer lines very frequently to avoid data racing

#define DEFAULT_ALIGNMENT (al_left) // Default alignment value def


//Line processing state definition
#define LINE_INVERTED           (1)         // definition for line inversion flag
#define LINE_ALIGNMENT_LEFT     (1 << 1)    // definition for left alignment flag
#define LINE_ALIGNMENT_RIGHT    (1 << 2)    // definition for right alignment flag
#define LINE_ALIGNMENT_CENTER   (LINE_ALIGNMENT_LEFT + LINE_ALIGNMENT_RIGHT) // definition for center alignment flag - combination of left and right
#define LINE_BLINKED            (1 << 7)    // definition for line blinking flag

/*Local Prototypes*/

// Default display layout definition
static lcd_line_t internal_lcd_layout[LCD_LINE_NUM] = {{0, LCD_LINE_PIXEL_HEIGHT}, {4, LCD_LINE_PIXEL_HEIGHT}, {0, LCD_LINE_PIXEL_HEIGHT}, {2, LCD_LINE_PIXEL_HEIGHT}};

// Just a buffer to prevent display from unwanted update
static uint8_t cached_str[LCD_LINE_NUM][LCD_CHAR_NUM] = {0};

// Buffer for 8 bit rows screen
static line_def line_buf[NUM_PIX_ROW_PER_COL_BYTES];

// Buffer to save lines states
static uint8_t line_update_state[NUM_PIX_ROW_PER_COL_BYTES] = {1,1,1,1,1,1};

// blinking tasks - 1 per line
static char_context_t blink_tasks[LCD_LINE_NUM] = {0};

static sl_sleeptimer_timer_handle_t task_timer_handler;

/* static uint8_t reverse_byte(uint8_t value)
{
    // The hack was brought from here - https://graphics.stanford.edu/~seander/bithacks.html#BitReverseObvious
    return (value * 0x0202020202ULL & 0x010884422010ULL) % 1023;
} */

static void wr_8bit_command(uint8_t data)
{
    uint16_t _data = data;
    EFM_ASSERT(lcd_sercomm_instance->write_non_blocking(lcd_sercomm_instance->handle, (uint8_t *)&_data, 1, 0) == 0);
}

static void wr_9bit_data(const uint8_t * data)
{
    uint16_t _data = 0x100 | *data;
    EFM_ASSERT(lcd_sercomm_instance->write_non_blocking(lcd_sercomm_instance->handle, (uint8_t *)&_data, 1, 0) == 0);
}

static uint8_t generate_mask(uint8_t start_value, uint8_t size)
{
    uint8_t res = 0;
    for(uint8_t cnt = 0; cnt < size; cnt++)
    {
        res += 1 << (start_value + cnt);
    }
    return res;
}

// the function writes 10 bits value to 6x8x128 bits array
static void write_buff_8_bits(uint16_t value, uint8_t start_bit, uint8_t size, uint8_t pos)
{
    #define BIT_8_CONVERSION_DEF (8)
    #define RECURSION_LEVEL (2)

    static uint8_t current_line[RECURSION_LEVEL] = {255, 255};
    static uint8_t start_mask[RECURSION_LEVEL];
    static uint8_t bits_to_write[RECURSION_LEVEL];
    static uint8_t mask[RECURSION_LEVEL];
    uint8_t rlevel = 0;
    static div_t div_context;

    if(size <= 0)
    {
        return;
    }

    if (size < LCD_LINE_PIXEL_HEIGHT) {
        rlevel = 1;
    }

    div_context = div(start_bit, BIT_8_CONVERSION_DEF);
    uint8_t line_num   = div_context.quot;

    if(line_num != current_line[rlevel])
    {
        current_line[rlevel] = line_num;

        start_mask[rlevel] = div_context.rem;

        bits_to_write[rlevel] = BIT_8_CONVERSION_DEF - start_mask[rlevel];

        if(bits_to_write[rlevel] > size)
        {
            bits_to_write[rlevel] = size;
        }

        mask[rlevel] = generate_mask(start_mask[rlevel], bits_to_write[rlevel]);
    }

    line_buf[line_num].line[pos] &= ~mask[rlevel];
    line_buf[line_num].line[pos] ^= (value << start_mask[rlevel]) & mask[rlevel];

    if (!(pos % SCREEN_UPDATE_INTERLEAVE_CNT)) {
        line_buf[line_num].state++;
    }

    write_buff_8_bits(value >> bits_to_write[rlevel], start_bit + bits_to_write[rlevel], size - bits_to_write[rlevel], pos);
}

//The function checks the icon borders
static bool get_icon_borders(const font_char *in_icon, uint8_t *left_border, uint8_t *right_border)
{
    if(!in_icon || !in_icon->trimmable)
    {
        return false;
    }
    uint8_t right = in_icon->size;
    uint8_t left  = 0;

    while(!in_icon->arr[left] && !in_icon->arr[right])
    {
        if(right <= left)
        {
            return false;
        }
        left++;
        right--;
    }
    *left_border  = left;
    *right_border = right;
    return true;
}

static uint16_t get_inversion(const uint8_t *state)
{
    if(*state & LINE_INVERTED)
    {
        return LINE_INVERT_MASK_DEF;
    }
    else
    {
        return 0;
    }
}

static alignment_t get_alignment(const uint8_t *state)
{
    uint8_t alignment = *state & LINE_ALIGNMENT_CENTER;

    switch (alignment) {
        case LINE_ALIGNMENT_CENTER:
            return al_center;
            break;
        case LINE_ALIGNMENT_LEFT:
            return al_left;
            break;
        case LINE_ALIGNMENT_RIGHT:
            return al_right;
            break;
        default:
            return DEFAULT_ALIGNMENT;
            break;
    }
}

// the function fills specific range of the lcd pixel line buffer with characters font specific information
static uint8_t stuff_char(const char_context_t * context, uint8_t max_pos)
{
    uint16_t text_inversion = get_inversion(&context->state); // inverted line definition
    uint8_t  i_font         = 0;                                                     // byte number index
    uint8_t  left_border    = 0;                                                     // leftmost byte index
    uint8_t  right_border   = font_array[context->my_char]->size;                             // rightmost byte index
    uint8_t  blink_byte     = ASCII_CHAR_ENABLED;                                    // All pixels are on by default;
    uint8_t  pos            = context->position;
    // To get icons borders without spaces
    if(context->my_char > LAST_ASCII_CHAR_DEF)
    {
        get_icon_borders(font_array[context->my_char], &left_border, &right_border);
        if((context->state & LINE_BLINKED) && font_array[context->my_char]->size && font_array[context->my_char]->is_blinking)
        {
            blink_byte = 0;
        }
    }

    // To check a character pixels length is not exceeds a line pixels length
    if(right_border + pos > max_pos)
    {
        return right_border - left_border;
    }

    // foreach font value in the character
    for(i_font = left_border; i_font < right_border; i_font++)
    {
        write_buff_8_bits(text_inversion ^
                              (uint16_t)((uint8_t)(blink_byte & font_array[context->my_char]->arr[i_font]) << (uint8_t)INVERTED_LINE_GAP),
                          context->buffer_shift, context->line_size, pos);
        pos++;
    }

    return right_border - left_border;
}

// callback for periodic timer
static void task_callback(sl_sleeptimer_timer_handle_t *handle, void *data)
{
    *(uint8_t*)data = (uint8_t)0;
    if(handle == &task_timer_handler)
    {
        // lookup for blinking task and blinking task current state
        for(uint8_t cnt = 0; cnt < LCD_LINE_NUM; cnt++)
        {
            if(blink_tasks[cnt].my_char)
            {
                if(blink_tasks[cnt].is_blinking)
                {
                    // to hide char
                    blink_tasks[cnt].state |= LINE_BLINKED;
                    blink_tasks[cnt].is_blinking = false;
                }
                else
                {
                    // to display char
                    blink_tasks[cnt].is_blinking = true;
                    blink_tasks[cnt].state &= ~LINE_BLINKED;
                }
                stuff_char(&blink_tasks[cnt], NUM_PIX_COL_PER_ROW_BYTES);
            }
        }
    }
}

static void task_worker(bool enable)
{
    if(enable)
    {
        sl_sleeptimer_start_periodic_timer_ms(&task_timer_handler, 1000, &task_callback, 0, 0, 0);
    }
    else
    {
        sl_sleeptimer_stop_timer(&task_timer_handler);
    }
}

// the function add blinking task from the task definiton buffer
static bool blink_task_add(const char_context_t * context, uint8_t line)
{
    bool ret = false;

    if(line < LCD_LINE_NUM)
    {
        blink_tasks[line] = *context;
        task_worker(true);
        ret = true;
    }

    return ret;
}

bool blink_task_remove(uint8_t line)
{
    bool ret = false;

    if(line < LCD_LINE_NUM)
    {
        blink_tasks[line].my_char = 0;
        ret = true;
    }

    return ret;
}

//The function returns length of a print line in pixels
static uint8_t pixel_distant_measure(const uint8_t *lpc_line_index)
{
    uint8_t cnt          = 0;
    uint8_t res          = 0;
    uint8_t left_border  = 0;
    uint8_t right_border = 0;
    uint8_t my_char      = 0;
    uint8_t un_cnt_space = 0;

    for(cnt = PRINT_LINE_CHARS; cnt < RIGHT_ALIGNMENT_BYTE; cnt++)
    {
        my_char = lpc_line_index[cnt];

        if(get_icon_borders(font_array[my_char], &left_border, &right_border))
        {
            res += (right_border - left_border);
            res++;
            un_cnt_space = 0;
        }
        else
        {
            // to cnt spaces
            if(my_char == ' ')
            {
                res += font_array[my_char]->size;
                un_cnt_space += font_array[my_char]->size;
            }
            else
            {
                res += font_array[my_char]->size;
                un_cnt_space = 0;
                res++;
            }
        }
    }

    return res - un_cnt_space;
}

static uint16_t stuff_font(uint8_t        line,
                           const uint8_t *lpc_line_index,
                           size_t         size,
                           bool           internal,
                           uint8_t        state) // uses [] to force size of array
{
    char_context_t char_context = {
        .my_char = 0, .state = state, .line_size = 0, .position = 0, .buffer_shift = 0, .is_blinking = 0};
    //uint8_t pixel_column_count = 0; // pixel column count
    uint16_t text_inversion = 0; // increments on each font column appended
    //uint8_t  my_char;        // character data

    uint8_t char1; // the index to the first character in a string
    uint8_t charN; // the index to the last (+1) character in a string
    uint8_t i_char; // the index of the current character
    uint8_t cnt = 0; // array index
    // uint16_t i_font, font1, fontN; // font lookup
    bool task_existed = false; // definition for blinking character in a line - true if any blinking character was found
    alignment_t alignment = DEFAULT_ALIGNMENT;
    uint8_t right_border = NUM_PIX_COL_PER_ROW_BYTES;
    uint8_t left_border = 0;

    // caller of this function sets the starting count, normally zero
    // pixel_column_count = start; // (non-zero from the recursive call)

    /********************************* START OF TEXT LINE *********************************/

    // No characters found
    if(size < 1)
        return 0;

    char1 = PRINT_LINE_CHARS;

    charN = RIGHT_ALIGNMENT_BYTE; // incomming string

    // Calculating buffer shift
    char_context.line_size = (uint8_t)internal_lcd_layout[line].height; // current line height

    // Calculating a height shift for the current line
    for(cnt = 0; cnt < line; cnt++)
    {
        char_context.buffer_shift += internal_lcd_layout[cnt].upper_indent + internal_lcd_layout[cnt].height;
    }
    char_context.buffer_shift += internal_lcd_layout[line].upper_indent;

    // To process leftmost character
    char_context.my_char = lpc_line_index[LEFT_ALIGNMENT_BYTE];
    // To check if we don't have alignment character before
    if (char_context.my_char != ' ') {
        left_border = stuff_char(&char_context, right_border);
    }

    // To process rightmost character
    char_context.my_char = lpc_line_index[RIGHT_ALIGNMENT_BYTE];
    if(char_context.my_char > LAST_ASCII_CHAR_DEF)
    {
        char_context_t rightmost_icon = char_context;
        right_border                  = NUM_PIX_COL_PER_ROW_BYTES - font_array[rightmost_icon.my_char]->size;
        rightmost_icon.position       = right_border;
        // To clear a space before rightmost button
        for(cnt = right_border - PIXELS_BEF_RIGHT_BUTTON; cnt < right_border; cnt++)
        {
            write_buff_8_bits(text_inversion, rightmost_icon.buffer_shift, rightmost_icon.line_size, cnt);
        }
        stuff_char(&rightmost_icon, NUM_PIX_COL_PER_ROW_BYTES);
        right_border -= PIXELS_BEF_RIGHT_BUTTON;
    }

    char_context.state = lpc_line_index[FORMAT_BYTE_CHAR]; // retrieving format byte value

    alignment = get_alignment(&char_context.state); // gets alignment value

    text_inversion = get_inversion(&char_context.state); // gets contrast value of text

    // To process different alignment
    switch(alignment)
    {
        case al_left:
            char_context.position = left_border;
            break;
        case al_right:
            char_context.position = NUM_PIX_COL_PER_ROW_BYTES - pixel_distant_measure(lpc_line_index) - BIT_SHIFT_COMPENSATION;
            for(cnt = left_border; cnt < char_context.position; cnt++)
            {
                write_buff_8_bits(text_inversion, char_context.buffer_shift, char_context.line_size, cnt);
            }
            break;
        case al_center:
            char_context.position = (NUM_PIX_COL_PER_ROW_BYTES - pixel_distant_measure(lpc_line_index)) / 2 - BIT_SHIFT_COMPENSATION;
            for(cnt = left_border; cnt < char_context.position; cnt++)
            {
                write_buff_8_bits(text_inversion, char_context.buffer_shift, char_context.line_size, cnt);
            }
            break;
        default:
            char_context.position = left_border;
            break;
    }

    // foreach character in the line...
    for(i_char = char1; i_char < charN; i_char++)
    {
        // space between characters
        write_buff_8_bits(text_inversion, char_context.buffer_shift, char_context.line_size, char_context.position);
        char_context.position++;

        char_context.my_char = lpc_line_index[i_char];

        // To check if blinking character exists
        if(!internal && font_array[char_context.my_char]->size && font_array[char_context.my_char]->is_blinking)
        {
            task_existed           = true;
            blink_task_add(&char_context, line);
        }

        // incrementing pixel column count by the character size
        char_context.position += stuff_char(&char_context, right_border);

        // checking for the current line overflow
        if (char_context.position >= right_border) {
            break;
        }
    }

    /********************************* END OF TEXT LINE *********************************/

    // This space is not included in "pixel_column_count"

    for(cnt = char_context.position; cnt < right_border; cnt++)
    {
        write_buff_8_bits(text_inversion, char_context.buffer_shift, char_context.line_size, cnt);
    }

    if(!internal && line == 0 && !task_existed)
    {
        if(task_existed)
        {
            task_worker(true);
        }
        else
        {
            blink_task_remove(line);
        }
    }

    // return count of font data added, not extra space nor the starting offset
    return 0;
}

void lcd_init(base_driver *sercomm_instance, const lcd_line_t *lcd_layout)
{
    if(lcd_sercomm_instance == 0)
    {
        lcd_sercomm_instance = sercomm_instance;
        sl_sleeptimer_delay_millisecond(LCD_INIT_TIMEOUT);
        lcd_gpio_reset_on();
        lcd_gpio_reset_off();
        sl_sleeptimer_delay_millisecond(LCD_INIT_TIMEOUT);
        wr_8bit_command(LCD_RESET);  // System Reset
        wr_8bit_command(LCD_SET_SL); // cmd #10: set start / scroll line = 0
        //------------------------------------------------------------------------------
        // bit3 CUM=1 CA increment on write only
        // bit2 PID=1 and don't understand the description ... H:-1 ???????
        // bit1 auto-increment order=0 means Column (CA) first
        // bit0 WA=1 means automatic column/page wrap around (ON)
        //------------------------------------------------------------------------------
        wr_8bit_command(LCD_SET_RAMA | LCD_PID | LCD_WA); // cmd #13: set ram address control (see above description)
        wr_8bit_command(LCD_SET_FR); // cmd #14: set frame rate:a0 80pbs;a1 100pbs (Frame rates don't match latest spec)
        wr_8bit_command(LCD_SET_PON);           // cmd #15: set all pixells on:OFF
        wr_8bit_command(LCD_SET_INV);           // cmd #16: set inverse display:OFF
        wr_8bit_command(LCD_SET_EN);            // cmd #17: turn display on
        wr_8bit_command(LCD_SET_MAP | LCD_MX);  // cmd #18: set lcd mapping control:mx=1;my=0
        wr_8bit_command(LCD_SET_BR | LCD_BR_8); // cmd #22: 0xea:bias=1/8;0xeb:bias=1/9;
        wr_8bit_command(LCD_SET_TC);            // cmd #6: set temp compensation tc1:tc0=0,0:-0.05%/c
        sl_sleeptimer_delay_millisecond(LCD_INIT_TIMEOUT);
        wr_8bit_command(LCD_SET_EN); // display enable
    };

    if(lcd_layout != 0)
    {
        for(size_t cnt = 0; cnt < LCD_LINE_NUM; cnt++)
        {
            internal_lcd_layout[cnt] = lcd_layout[cnt];
        }
    }
}

/*Local Prototypes end*/

bool lcd_update(bool lcd_status)
{
    static uint16_t i_col_s = 0;
    static uint8_t  i_lin_s = 0;
    bool            return_code;

    if(i_col_s == 0)
    {
        if(++i_lin_s >= NUM_PIX_ROW_PER_COL_BYTES)
        {
            i_lin_s = 0;
        }

        if(line_update_state[i_lin_s] == line_buf[i_lin_s].state)
        {
            return false;
        }
        else
        {
            line_update_state[i_lin_s] = line_buf[i_lin_s].state;
            // wr_8bit_command(LCD_SET_EN); //if it requires to off the display while it's updating
            wr_8bit_command(LCD_SET_PAGE | i_lin_s); // top page/row
        }

        // wr_8bit_command(LCD_SET_RAMA | LCD_AINC);
    }

    if(lcd_status == true)
    {
        if(i_col_s < NUM_PIX_COL_PER_ROW_BYTES) // protect because this function is entered multiple times
        {
            wr_8bit_command(LCD_SET_COL_L + (i_col_s & 0xF));

            wr_8bit_command(LCD_SET_COL_H + (i_col_s >> 4));

            wr_9bit_data(&line_buf[i_lin_s].line[i_col_s]);
        }
    }

    if(i_col_s == NUM_PIX_COL_PER_ROW_BYTES)
    {
        wr_8bit_command(LCD_SET_EN | LCD_ENABLE); // turn display on
        i_col_s     = 0;
        return_code = true;
    }
    else if(i_col_s > NUM_PIX_COL_PER_ROW_BYTES) // just a boundry protection
    {
        i_col_s     = 0;
        return_code = false;
    }
    else
    {
        i_col_s++;
        return_code = false;
    }

    return return_code;
}

bool lcd_put_line(const uint8_t *str, const size_t size, const uint8_t line, language_e language)
{
    (void)language;
    // Setup indexing into the text "line_index_table"
    // clear the whole "line buffer"
    if(size > LCD_CHAR_NUM)
    {
        return false; // size exceeds supported line length
    }
    if(line >= LCD_LINE_NUM)
    {
        return false; // line doens't match to lines supported number
    }

    if(strncmp((const char *)cached_str[line], (const char *)str, size))
    {
        memcpy(cached_str[line], str, size);
        stuff_font(line, cached_str[line], size, 0, false);
    }

    return true;
}

/* bool lcd_put_big_number(uint16_t num, uint8_t offset, uint8_t index, uint8_t contrast)
{
    if(index > BIG_FONT_NUM_COL)
    {
        return false;
    }

    if(offset >= NUM_PIX_COL_PER_ROW_BYTES)
    {
        return false;
    }

    for(uint8_t line = 0; line < LCD_LINE_NUM; line++)
    {
        line_buf[line][offset] = big_font_table[num].value[line][index] ^ contrast;
    }
    return true;
} */

bool lcd_put_raw_data(uint8_t data, uint8_t line, uint8_t offset)
{
    if(line > LCD_LINE_NUM)
    {
        return false;
    }
    if(offset >= NUM_PIX_COL_PER_ROW_BYTES)
    {
        return false;
    }

    line_buf[line].line[offset] = data;
    return true;
}

void lcd_clear()
{
    memset(line_buf, 0, sizeof(line_buf));
}

void lcd_adjust_contrast(uint8_t value)
{
    wr_8bit_command(LCD_SET_BIAS); // set Vbias
    wr_8bit_command(value);
}

void lcd_backlight_on()
{
    lcd_gpio_backlight_on();
}

void lcd_backlight_off()
{
    lcd_gpio_backlight_off();
}

void lcd_all_pixels_on()
{
    wr_8bit_command(LCD_SET_PON | LCD_ENABLE);
}

void lcd_all_pixels_off()
{
    wr_8bit_command(LCD_SET_PON);
}

bool lcd_put_qr_code(uint8_t qr_version_number, uint16_t num, uint8_t offset, uint8_t index, uint8_t contrast)
{
    if(index > BIG_FONT_NUM_COL)
    {
        return false;
    }

    if(offset >= NUM_PIX_COL_PER_ROW_BYTES)
    {
        return false;
    }

    /*__flash*/ qr_code_t * qr_to_print;

    switch(qr_version_number)
    {
        case 3:
            qr_to_print = qr_code_myq_v3;
            break;
        case 4:
            qr_to_print = qr_code_myq_v4;
            break;

        case 5:
            qr_to_print = qr_code_myq_v5;
            break;
        
        case 6:
            qr_to_print = qr_code_myq_v6;
            break;
        
        case 7:
            qr_to_print = qr_code_myq_v7;
            break;
        
        default:
            qr_to_print = qr_code_myq_v4;
            break;

    }
    for (uint8_t line = 0; line < QR_CODE_NUM_ROW; line++)
    {
        for (uint8_t ix = 0; ix < QR_CODE_NUM_COL; ix++)
        {
            index = ix;

            // line_buf[line][index + offset] = \
            //         qr_code_myq[num].value[line][index] ^ contrast;  ///// qr_code_myq [][] ES EL PIXEL MAP

            line_buf[line].line[index + offset] = \
                    qr_to_print[num].value[line][index] ^ contrast;  ///// qr_code_myq [][] ES EL PIXEL MAP

        }
    }

    return true;

}

//----------------------------------------------------------------------------
