/*
 * weather.cc 
 *
 * Legend of Saladir (C) 1997/1998 by Erno Tuomainen
 *
 */

#include "weather.h"

#include "file.h"

Weather::Weather()
{
   current.temperature=20;
   current.rain = WEATHER_NORAIN;
   current.wind = WEATHER_NOWIND;
   current.duration = 0;
   current.ticks=0;

   destination = current;



}
Weather::~Weather()
{

}

bool Weather::save(_FH_ fh)
{
   ssize_t res;

   if(fh < 0)
      return false;

   if(!write_header(SAVE_WTHRTYPE, fh))
      return false;

   res = write(fh, &current, sizeof(TWeather));
   if(res != sizeof(TWeather))
      return false;

   res = write(fh, &destination, sizeof(TWeather));
   if(res != sizeof(TWeather))
      return false;

   return true;
}


bool Weather::load(_FH_ fh)
{
   ssize_t res;

   if(fh < 0)
      return false;

   res = read(fh, &current, sizeof(TWeather));
   if(res != sizeof(TWeather))
      return false;

   res = read(fh, &destination, sizeof(TWeather));
   if(res != sizeof(TWeather))
      return false;

   return true;
}

void Weather::passtime(int32u tickspassed, Gametime & mt)
{
   current.ticks+=tickspassed;

   while(current.ticks >= TIME_1MIN ) {

      current.ticks-=CONFIGVARS.ticksperminute;

      change_weather(mt);
   }

}

/* Air is hot and the wind is blowing calmly */
/* It's raining softly */
/* It's winding mildly */
/* The wind is storming */
/* The temperature is warm */
/* Air is calm. */


/* 
 * change weather conditions,
 *  call this every game minute
 *
 */
void Weather::change_weather(Gametime & mt)
{
   bool report;

   modify();

   if(current.duration) {
      current.duration--;
      return;
   }

   /* get new weather duration */
   current.duration = WEATHER_MINDURA + RANDU(WEATHER_MAXDURA);

   if(player.dungeon==0 || player.levptr->dtype==DTYPE_TOWN)
      report=true;
   else
      report=false;

   change_wind(mt, report);
   change_temperature(mt, report);
   change_rain(mt, report);
}

void Weather::modify(void)
{
   if(current.wind > destination.wind)
      current.wind--;
   else if(current.wind < destination.wind)
	   current.wind++;

   if(current.temperature > destination.temperature)
      current.temperature--;
   else if(current.temperature < destination.temperature)
	   current.temperature++;

   if(current.rain > destination.rain)
      current.rain--;
   else if(current.rain < destination.rain)
	   current.rain++;
}

bool Weather::change_temperature(Gametime & mt, bool report)
{
   int16u seas;
   int16s mintemp;

   seas = mt.report_season();

   if(seas==SEASON_WINTER) {
      mintemp=WINTER_MINTEMP;

   }
   else if(seas==SEASON_SPRING) {
      mintemp=SPRING_MINTEMP;

   }
   else if(seas==SEASON_SUMMER) {
      mintemp=SUMMER_MINTEMP;

   }
   else {
      mintemp=AUTUMN_MINTEMP;
   }


   if( RANDU(100) < 20) {      
      destination.temperature=RANDU(WEATHER_TEMPRANGE) - mintemp;
   }
   else
      return false;

   if(!report)
      return true;

   if(destination.temperature > current.temperature)
      msg.newmsg("It's getting warmer.", C_WHITE);
   else if(destination.temperature < current.temperature)
      msg.newmsg("It's getting colder.", C_WHITE);

   return true;

}

bool Weather::change_wind(Gametime & mt, bool report)
{   
   if( RANDU(100) < 20) {      
      destination.wind=RANDU(WEATHER_MAXWIND);
   }
   else
      return false;

   if(!report)
      return true;

   if(destination.wind > current.wind)
      msg.newmsg("The wind is getting stronger.", C_WHITE);
   else if(destination.wind < current.wind)
      msg.newmsg("The wind is calming a bit.", C_WHITE);
   
   return true;

}

bool Weather::change_rain(Gametime & mt, bool report)
{
   int16u seas;
   int16u prob;

   seas = mt.report_season();

   if(seas==SEASON_WINTER) {
      prob=WINTER_RAINPROB;
   }
   else if(seas==SEASON_SPRING) {
      prob=SPRING_RAINPROB;
   }
   else if(seas==SEASON_SUMMER) {
      prob=SUMMER_RAINPROB;
   }
   else {
      prob=AUTUMN_RAINPROB;
   }

   /* should it rain */
   if( RANDU(100) < prob) {

      /* get stronger or ? */
      if(RANDU(100) < prob) {
	 if(destination.rain < WEATHER_MAXRAIN)
	    destination.rain++;
      }
      else
	 if(destination.rain > WEATHER_NORAIN)
	    destination.rain--;
   }
   else
      return false;

   if(!report)
      return true;

   if((current.rain==WEATHER_NORAIN) && (destination.rain!=WEATHER_NORAIN)) {
      if(current.temperature > 0)
	 msg.newmsg("It's starting to rain water.", C_WHITE);
      else
	 msg.newmsg("It's starting to snow.", C_WHITE);	 
   }   
   else {
      if(current.temperature > 0) {
	 if(destination.rain > current.rain)
	    msg.newmsg("The rain is getting stronger.", C_WHITE);
	 else if(destination.rain < current.rain)
	    msg.newmsg("The rain is slowing down.", C_WHITE);
      }
      else {
	 if(destination.rain > current.rain)
	    msg.newmsg("The snowing gets stronger.", C_WHITE);
	 else if(destination.rain < current.rain)
	    msg.newmsg("The snowing is slowing down.", C_WHITE);
      }
   }

   return true;
}

void Weather::report(void)
{
  


}
