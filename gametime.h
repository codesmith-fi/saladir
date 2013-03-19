/* gametime.h for Saladir */

#ifndef _GAMETIME_H_DEFINED
#define _GAMETIME_H_DEFINED

#define TIME_DAYS    29
#define TIME_MONTHS  12
#define TIME_MINUTES 60
#define TIME_HOURS   24

/* how many ticks in minute */
#define TIME_1MIN 2800

/* season numbers */
#define SEASON_WINTER 0
#define SEASON_SPRING 1
#define SEASON_SUMMER 2
#define SEASON_AUTUMN 3

class Gametime
{
   typedef struct
   {
	 int16s min;
	 int16s hour;
	 int16s day;
	 int16s month;
	 int16s year;      
	 int32u ticks;     
   } Tgametime;

   friend class Weather;

  private:
   Tgametime tm;
   bool eventmessages;
   
   void add_minute();
   void add_hour();
   void add_day();
   void add_month();
   void add_year();
   
   void sub_minute();
   void sub_hour();
   void sub_day();
   void sub_month();
   void sub_year();
   
  public:
   Gametime();
   ~Gametime();
   void set_events(bool state);
   void setrandom(int16s data);
   void set(int16s ryear, int16s rmonth, int16s rday, int16s rhour, int16s rmin);
   void reset();
   int16s compare_time(int16s rhour, int16s rmin);
   int16s compare_date(int16s ryear, int16s rmonth, int16s rday);
   void add_minutes(int16s minutes);
   void tick(int32u tickspassed);
   void tickback(int32u tickspassed);
   void show_passed();
   void show_worldtime();
   void show_timer(char *msg);
   void print_worldtime();
   sint report_season(void); 

   bool save(_FH_ fh, int16u hdr);
   bool load(_FH_ fh);

   bool is_night(void);
};

#endif
