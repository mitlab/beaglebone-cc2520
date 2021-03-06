#ifndef DEBUG_H
#define DEBUG_H

// Define different levels of debug printing

// print nothing
#define DEBUG_PRINT_OFF 0
// print only when something goes wrong
#define DEBUG_PRINT_ERR 1
// print occasional messages about interesting things
#define DEBUG_PRINT_INFO 2
// print a good amount of debugging output
#define DEBUG_PRINT_DBG 3

// Defines the level of debug output
extern uint8_t debug_print;

// Define the printk macros.
#define ERR(tag, ...)  do {if (debug_print >= DEBUG_PRINT_ERR)  { printk(tag "[cc2520] " __VA_ARGS__); }} while (0)
#define INFO(tag, ...) do {if (debug_print >= DEBUG_PRINT_INFO) { printk(tag "[cc2520] " __VA_ARGS__); }} while (0)
#define DBG(tag, ...)  do {if (debug_print >= DEBUG_PRINT_DBG)  { printk(tag "[cc2520] " __VA_ARGS__); }} while (0)

#endif
