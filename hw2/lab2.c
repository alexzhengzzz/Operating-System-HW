#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ktime.h>
MODULE_LICENSE("Dual BSD/GPL");

static void format_time(int *hour, int *min, int *sec)
{
 	s64 uptime_ms;
 	s64 ts;
 	s64 seconds_per_day;
 	s64 last_day_second;

 	/* convert uptime to second */
    	uptime_ms = ktime_to_ms(ktime_get_boottime());
	ts = uptime_ms / 1000;

    	seconds_per_day = 60*60*24;
    	last_day_second = ts % seconds_per_day;

    	*hour = last_day_second / 3600;
    	*min  = (last_day_second-*hour*3600) / 60;
    	*sec  = (last_day_second-*hour*3600-*min*60); 
}

/* This function is called when the module is loaded. */
static int hello_init(void)
{
	int hour, min, sec;


    	format_time(&hour, &min, &sec);

    	printk(KERN_ALERT "Hello, world\n");
    	printk(KERN_ALERT"system uptime is %02d:%02d:%02d\n", hour, min, sec);

    	return 0;
}

/* This function is called when the module is removed. */
static void hello_exit(void) {

	int hour, min, sec;

    	format_time(&hour, &min, &sec);

 	printk(KERN_ALERT "Goodbye, cruel world\n");
	printk(KERN_ALERT"system uptime is %02d:%02d:%02d\n", hour, min, sec);
}

/* Macros for registering module entry and exit points. */
module_init( hello_init );
module_exit( hello_exit );



