/*

 Saladir time

*/

#include "saladir.h"

#include "output.h"
#include "options.h"
#include "file.h"

char *time_hourtxt[]=
{
     "It's midnight.",
     "",
     "",
     "",
     "",
     "",
     "It's morning.",
     "",
     "",
     "",
     "",
     "",
     "It's midday.",
     "",
     "",
     "",
     "",
     "",
     "It's evening.",
     "",
     "",
     "",
     "",
     "",
     NULL,
};

char *time_months[]=
{
     "New year's dawn",
     "White glimmer",
     "Snowcone",
     "Rapid water",
     "Seed",
     "Sundance",
     "Festival",
     "Harvest",
     "Thunder",
     "Snow",
     "Ice",
     "Old Year's eve",
     NULL
};

/* seasons */
char *time_seasons[]=
{
   "winter",
   "spring",
   "summer",
   "autumn",
   NULL
};

Gametime::Gametime()
{
     eventmessages=false;
     reset();
}

Gametime::~Gametime()
{
}

void Gametime::set_events(bool state)
{
     eventmessages=state;
}

void Gametime::setrandom(int16s syear)
{
     tm.min=RANDU(TIME_MINUTES);
     tm.hour=RANDU(TIME_HOURS);
     tm.day=RANDU(TIME_DAYS);
     tm.month=RANDU(TIME_MONTHS);
     tm.year=syear;
     tm.ticks=0;
}


void Gametime::reset()
{
     tm.min=0;
     tm.hour=0;
     tm.day=0;
     tm.month=0;
     tm.year=0;
     tm.ticks=0;
}

void Gametime::set(int16s ryear, int16s rmonth, int16s rday, 
		   int16s rhour, int16s rmin)
{
     tm.year=ryear;
     tm.month=rmonth;
     tm.day=rday;
     tm.hour=rhour;
     tm.min=rmin;
     tm.ticks=0;
}

int16s Gametime::compare_date(int16s ryear, int16s rmonth, int16s rday)
{
     if(tm.year>=ryear && tm.month>=rmonth && tm.day>=rday) {
	  return 1;
     }
     return 0;
}

int16s Gametime::compare_time(int16s rhour, int16s rmin)
{
   rhour-=tm.hour;
   rmin-=tm.min;
   if(rhour==0)
      return rmin;
   return rhour;
}


void Gametime::add_minute()
{
   tm.min++;

   if(tm.min>= TIME_MINUTES)
      add_hour();
}

void Gametime::sub_minute()
{
   if(tm.hour>0 || tm.day>0 || tm.month>0 || tm.year>0 || tm.min>0)
      tm.min--;

   if(tm.min < 0 )
      sub_hour();
}


void Gametime::add_hour()
{

   bool prec;

   tm.min=0;
   prec=is_night();

   tm.hour++;

   if(tm.hour >= TIME_HOURS)
      add_day();

   if(eventmessages) {
      if( strlen(time_hourtxt[tm.hour])>0 )
	 msg.newmsg(time_hourtxt[tm.hour], C_WHITE);

      if(prec!=is_night()) {
	 GAME_NOTIFYFLAGS|=GAME_DO_REDRAW;

	 if(is_night())
	    msg.newmsg("The sun goes down.", C_WHITE);
	 else
	    msg.newmsg("The sun rises.", C_WHITE);
      }
   }

}

void Gametime::sub_hour()
{
   if(tm.hour>0 || tm.day>0 || tm.month>0 || tm.year>0) {
      tm.min=TIME_MINUTES-1;
      tm.hour--;
   }
   else
      tm.hour=0;

   if(tm.hour < 0 )
      sub_day();
}

void Gametime::add_day()
{
   tm.hour=0;
   tm.day++;
   if(tm.day >= TIME_DAYS)
      add_month();

}

void Gametime::sub_day()
{
   if(tm.day>0 || tm.month>0 || tm.year>0) {
      tm.hour=TIME_HOURS-1;
      tm.day--;
   }
   else tm.day=0;

   if(tm.day < 0 )
      sub_month();

}

void Gametime::add_month()
{
   tm.day=0;
   tm.month++;

   if(tm.month >= TIME_MONTHS)
      add_year();
}

void Gametime::sub_month()
{
   if(tm.month>0 || tm.year>0) {
      tm.day=TIME_DAYS-1;
      tm.month--;
   }
   else tm.month=0;

   if(tm.month < 0 )
      sub_year();
}

void Gametime::add_year()
{
	tm.month=0;
   tm.year++;

}

void Gametime::sub_year()
{
   if(tm.year>0) {
      tm.month=TIME_MONTHS-1;
      tm.year--;
   }
   else tm.year=0;
}

void Gametime::add_minutes(int16s minutes)
{
   while(minutes!=0) {
      if(minutes>0) {
	 add_minute();
	 minutes--;
      }
      if(minutes<0) {
	 sub_minute();
         minutes++;
      }
   }
}

void Gametime::tick(int32u tickspassed)
{

   tm.ticks+=tickspassed;

   while(tm.ticks >= CONFIGVARS.ticksperminute) {
      tm.ticks-=CONFIGVARS.ticksperminute;
      add_minute();
   }

}

void Gametime::tickback(int32u tickspassed)
{
   tm.ticks+=tickspassed;

   while(tm.ticks >= CONFIGVARS.ticksperminute) {
      tm.ticks-=CONFIGVARS.ticksperminute;
      sub_minute();
   }

}

bool Gametime::save(_FH_ fh, int16u hdr)
{
   ssize_t res;

   if(fh < 0)
      return false;

   if(!write_header(hdr, fh))
      return false;

   res = write(fh, &this->tm, sizeof(Tgametime));

   if(res != sizeof(Tgametime))
      return false;

   return true;
}

bool Gametime::load(_FH_ fh)
{
   ssize_t res;

   if(fh < 0)
      return false;
     
   res = read(fh, &this->tm, sizeof(Tgametime));

   if(res != sizeof(Tgametime))
      return false;

   return true;
}

bool Gametime::is_night(void)
{
   if(tm.hour >= 0 && tm.hour < 5)
      return true;

   if(tm.hour >= 20)
      return true;

   return false;
}

sint Gametime::report_season(void)
{
   if( tm.month >= 0 && tm.month < 3)
      return SEASON_WINTER;
   else if(tm.month >= 3 && tm.month < 5)
      return SEASON_SPRING;
   else if(tm.month >= 5 && tm.month < 8)
      return SEASON_SUMMER;
   else if(tm.month >= 8 && tm.month < 11)
      return SEASON_AUTUMN;
   else return SEASON_WINTER;      
}

/* Several time display routines */
void Gametime::show_passed()
{
     char timestr[200];
     char timetmp[20];

     timestr[0]=0;

     strncpy(timestr, "You've been adventuring ", sizeof(timestr)-1);

     if(tm.year>0) {
	  sprintf(timetmp, "%d years, ", tm.year);
	  strncat(timestr, timetmp, sizeof(timestr)-1);
     }
     if(tm.month>0) {
	  sprintf(timetmp, "%d months, ", tm.month);
	  strncat(timestr, timetmp, sizeof(timestr)-1);
     }
     if(tm.day>0) {
	  sprintf(timetmp, "%d days, ", tm.day);
	  strncat(timestr, timetmp, sizeof(timestr)-1);
     }
     if(tm.hour>0) {
	  sprintf(timetmp, "%d hours, ", tm.hour);
	  strncat(timestr, timetmp, sizeof(timestr)-1);
     }

     sprintf(timetmp, "%d minutes ", tm.min);
     strncat(timestr, timetmp, sizeof(timestr)-1);

     strncat(timestr, "measured in Saladir time.", sizeof(timestr)-1);
     timestr[sizeof(timestr)-1]=0;

     msg.newmsg(timestr, C_WHITE);
}

void Gametime::show_worldtime()
{
   char timestr[100];
   char mtmp[10];

   if(tm.day==0)
      strcpy(mtmp, "st");
   else if(tm.day==1)
      strcpy(mtmp, "nd");
   else if(tm.day==2)
      strcpy(mtmp, "rd");
   else strcpy(mtmp, "th");

   sprintf(timestr, "Time is %02d:%02d, %02d%s of %s in the year of %04d (%s).",
	   tm.hour, tm.min, tm.day+1, mtmp, time_months[tm.month], tm.year,
	   time_seasons[report_season()]);

   msg.newmsg(timestr, C_WHITE);
}

void Gametime::show_timer(char *msgtxt)
{
     const char *defstr="Timer is now ";

     char timestr[40];
     char *outstr;

     sprintf(timestr, "%d:%d of %d/%d/%d.", tm.hour, tm.min, 
	     tm.day, tm.month, tm.year);

     if(msgtxt!=NULL) {
	  outstr = new char[strlen(msgtxt)+strlen(timestr)+1];
     }
     else {
	  outstr = new char[strlen(timestr)+strlen(defstr)+1];
     }

     if(outstr) {
	  outstr[0]=0;

	  if(msgtxt)
	       strcpy(outstr, msgtxt);
	  else
	       strcpy(outstr, defstr);

	  strcat(outstr, timestr);

	  msg.newmsg(outstr, C_WHITE);

	  delete [] outstr;
     }
     else
	  msg.newmsg("Out of memory error in Gametime::show_timer()!", CHB_RED);
}

void Gametime::print_worldtime()
{
     char mtmp[10];

     if(tm.day==0)
	  strcpy(mtmp, "st");
     else if(tm.day==1)
	  strcpy(mtmp, "nd");
     else if(tm.day==2)
	  strcpy(mtmp, "rd");
     else strcpy(mtmp, "th");

     my_printf("%02d%s of %s in the year of %04d at %02d:%02d",
	       tm.day+1, mtmp, time_months[tm.month], 
	       tm.year, tm.hour, tm.min);
}




